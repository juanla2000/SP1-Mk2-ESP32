// pantalla_navegacion.cpp

#include "pantalla_navegacion.h"
#include "pantalla_unica.h"
#include "menu_unico.h"
#include "zona_menu.h"
#include "controles.h"
#include "configuracion.h"
#include "secuenciador.h"
#include <Adafruit_SSD1351.h>

extern Adafruit_SSD1351 display;
extern bool necesitaRefrescoParcial;

PantallaSistema pantallaActual = PANTALLA_HOME;
PantallaSistema pantallaAnterior = PANTALLA_HOME;
unsigned long tiempoUltimaActividad = 0;
ContenidoPantalla contenidoActual;

extern unsigned long tiempoUltimaInteraccionMenu;
extern EstadoSistema estadoSistema;
extern const unsigned long TIMEOUT_MENU;

// Variables para optimización
static bool pantallaDirty = true;
static PantallaSistema ultimaPantallaRedibujada = PANTALLA_INVALIDA;
static ContenidoPantalla ultimoContenido;


// ============================================================
// FUNCIONES AUXILIARES
// ============================================================

void dibujarLineaCentrada(uint8_t y, const String& texto, uint8_t textSize, uint16_t fondo, uint16_t color) {
    display.setTextSize(textSize);
    int16_t x1, y1;
    uint16_t w, h;
    display.getTextBounds(texto, 0, y, &x1, &y1, &w, &h);
    int16_t xCentrado = (SCREEN_WIDTH - w) / 2;
    display.fillRect(0, y, SCREEN_WIDTH, h + 2, fondo);
    display.setCursor(xCentrado, y);
    display.setTextColor(color);
    display.print(texto);
}

// ============================================================
// FUNCIONES PRINCIPALES (OPTIMIZADAS)
// ============================================================

void actualizarZonaPantalla(ZonaMenu zona, const ContenidoPantalla& c) {
    switch (zona) {
        case ZONA_SUPERIOR:
            dibujarLineaCentrada(2,  c.linea1, 2, c.fondo1, c.texto1);
            dibujarLineaCentrada(22, c.linea2, 2, c.fondo1, c.texto1);
            break;
        case ZONA_CENTRAL:
            dibujarLineaCentrada(44, c.linea3, 2, c.fondo2, c.texto2);
            dibujarLineaCentrada(64, c.linea4, 2, c.fondo2, c.texto2);
            break;
        case ZONA_INFERIOR:
            dibujarLineaCentrada(90, c.linea5, 3, c.fondo3, c.texto3);
            break;
    }
}

void entradaEventoNota(uint8_t nota, uint8_t vel, uint8_t canal) {
    if (estadoSistema != ESTADO_READY) return;
    if (pantallaActual == PANTALLA_TEST) return;
    
    // Evitar redibujados innecesarios
    if (nota == ultimaNotaRecibida && vel == ultimaVelocidadRecibida && canal == ultimoCanalRecibido) {
        return;
    }
    
    ultimaNotaRecibida = nota;
    ultimaVelocidadRecibida = vel;
    ultimoCanalRecibido = canal;

    pantallaActual = PANTALLA_STANDBY_NOTE;
    tiempoUltimaActividad = millis();

    contenidoActual = {
        "MIDI", "CH " + String(canal + 1),
        "NOTE - VELOCITY", "NOTE " + String(nota),
        String(nota) + " - " + String(vel),
        COLOR_FONDO_SUPERIOR, COLOR_TEXTO_SUPERIOR,
        COLOR_FONDO_CENTRAL, COLOR_TEXTO_CENTRAL,
        COLOR_FONDO_INFERIOR, COLOR_TEXTO_INFERIOR
    };
    
    pantallaDirty = true;
}

void entradaEventoCC(uint8_t mux, uint8_t cc, uint8_t val) {
    if (estadoSistema != ESTADO_READY) return;
    if (pantallaActual == PANTALLA_TEST) return;
    
    // Buscar control correspondiente
    bool valorCambiado = false;
    for (int i = 0; i < numControles; i++) {
        if (controles[i].cc == cc && controles[i].mux == mux) {
            if (controles[i].valor != val) {
                controles[i].valor = val;
                valorCambiado = true;
            }
            break;
        }
    }
    
    if (!valorCambiado) return;

    if (mux <= 6) {
        pantallaActual = PANTALLA_STANDBY_KNOB;
        contenidoActual = {
            "SURFACE", obtenerNombreSubgrupo(0),
            "KNOB CC " + String(cc) + " - CH 1",
            "[VALOR CC %]", String(val * 100 / 127) + "%",
            COLOR_FONDO_SUPERIOR, COLOR_TEXTO_SUPERIOR,
            COLOR_FONDO_CENTRAL, COLOR_TEXTO_CENTRAL,
            COLOR_FONDO_INFERIOR, COLOR_TEXTO_INFERIOR
        };
    } else {
        pantallaActual = PANTALLA_STANDBY_SWITCH;
        contenidoActual = {
            "SURFACE", obtenerNombreSubgrupo(0),
            "SWITCH CC " + String(cc) + " - CH 1",
            "<ON / OFF>", val > 63 ? "<ON>" : "<OFF>",
            COLOR_FONDO_SUPERIOR, COLOR_TEXTO_SUPERIOR,
            COLOR_FONDO_CENTRAL, COLOR_TEXTO_CENTRAL,
            COLOR_FONDO_INFERIOR, COLOR_TEXTO_INFERIOR
        };
    }

    tiempoUltimaActividad = millis();
    pantallaDirty = true;
}

void clicEncoder() {
    pantallaAnterior = pantallaActual;
    
    // Solo entrar en menú si no estamos ya en él
    if (pantallaActual != PANTALLA_MENU) {
        pantallaActual = PANTALLA_MENU;
        inicializarMenuUnico();
    }
    
    tiempoUltimaActividad = millis();
    tiempoUltimaInteraccionMenu = millis();
    pantallaDirty = true;
}

// ============================================================
// NÚCLEO DE ACTUALIZACIÓN DE PANTALLA (OPTIMIZADO)
// ============================================================

void actualizarPantallaSistema() {
    unsigned long ahora = millis();
    
    // 1. Gestión de estado BOOT → READY
    if (pantallaActual == PANTALLA_HOME && ahora - tiempoUltimaActividad > 2000) {
        pantallaActual = PANTALLA_READY;
        estadoSistema = ESTADO_READY;
        
        String surfaceEstado = secuenciadorRutaActiva ? "[ON]" : "[OFF]";
        String secuenciadorEstado = secuenciadorGlobalActivo ? "[ON]" : "[OFF]";
        String testStatus = "[READY]";

        contenidoActual = {
            "SP1 / Mk2", "MIDI CONTROL",
            "SURFACE " + surfaceEstado, "SEQUENCER " + secuenciadorEstado,
            testStatus,
            COLOR_FONDO_SUPERIOR, COLOR_TEXTO_SUPERIOR,
            COLOR_FONDO_CENTRAL, COLOR_TEXTO_CENTRAL,
            COLOR_FONDO_INFERIOR, COLOR_TEXTO_INFERIOR
        };
        
        pantallaDirty = true;
        tiempoUltimaActividad = ahora;
    }
    
    // 2. Timeout para pantallas standby (3 segundos)
    if ((pantallaActual == PANTALLA_STANDBY_NOTE || 
         pantallaActual == PANTALLA_STANDBY_KNOB || 
         pantallaActual == PANTALLA_STANDBY_SWITCH) &&
        ahora - tiempoUltimaActividad > 3000) {
        
        pantallaActual = PANTALLA_READY;
        pantallaDirty = true;
        tiempoUltimaActividad = ahora;
    }
    
    // 3. Timeout para menú (5 segundos)
    if (pantallaActual == PANTALLA_MENU && 
        (ahora - tiempoUltimaInteraccionMenu > TIMEOUT_MENU)) {
        
        pantallaActual = pantallaAnterior;
        pantallaDirty = true;
        return;
    }
    
    // 4. Redibujado optimizado
    if(pantallaDirty) {
        if(pantallaActual == PANTALLA_MENU) {
            mostrarPantallaUnica(display);
        } else {
            // Solo actualiza zonas modificadas
            if(memcmp(&contenidoActual.linea1, &ultimoContenido.linea1, sizeof(String)*2) != 0) {
                actualizarZonaPantalla(ZONA_SUPERIOR, contenidoActual);
            }
            if(memcmp(&contenidoActual.linea3, &ultimoContenido.linea3, sizeof(String)*2) != 0) {
                actualizarZonaPantalla(ZONA_CENTRAL, contenidoActual);
            }
            if(contenidoActual.linea5 != ultimoContenido.linea5) {
                actualizarZonaPantalla(ZONA_INFERIOR, contenidoActual);
            }
        }
        
        ultimoContenido = contenidoActual;
        pantallaDirty = false;
    }
}

// ============================================================
// MANEJO DE GIRO DEL ENCODER (OPTIMIZADO)
// ============================================================

void procesarGiroEncoder(int delta) {
    if (pantallaActual == PANTALLA_MENU) {
        rotarMenu(delta);
        pantallaDirty = true;
        tiempoUltimaInteraccionMenu = millis();
    }
    // Comportamiento en otras pantallas puede añadirse aquí
}

// ============================================================
// FUNCIÓN PARA DIBUJAR CONTENIDO COMPLETO (OPTIMIZADA)
// ============================================================

void dibujarContenidoPantalla(const ContenidoPantalla& c) {
    // Zona Superior
    display.fillRect(0, 0, SCREEN_WIDTH, 40, c.fondo1);
    dibujarLineaCentrada(2,  c.linea1, 2, c.fondo1, c.texto1);
    dibujarLineaCentrada(22, c.linea2, 2, c.fondo1, c.texto1);
    
    // Zona Central
    display.fillRect(0, 40, SCREEN_WIDTH, 40, c.fondo2);
    dibujarLineaCentrada(44, c.linea3, 2, c.fondo2, c.texto2);
    dibujarLineaCentrada(64, c.linea4, 2, c.fondo2, c.texto2);
    
    // Zona Inferior
    display.fillRect(0, 80, SCREEN_WIDTH, 48, c.fondo3);
    dibujarLineaCentrada(90, c.linea5, 3, c.fondo3, c.texto3);
    
    // Separadores
    display.drawFastHLine(0, 40, SCREEN_WIDTH, 0x0000);
    display.drawFastHLine(0, 80, SCREEN_WIDTH, 0x0000);
}