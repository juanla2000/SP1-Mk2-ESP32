// pantalla_navegacion.cpp

#include "pantalla_navegacion.h"
#include "pantalla_unica.h"
#include "menu_unico.h"
#include "zona_menu.h"
#include "controles.h"
#include "configuracion.h"
#include "secuenciador.h"  // ✅ Necesario para Step

static int lastA = HIGH;
static int lastB = HIGH;   // ✅ Variable añadida para estado del pin B
static unsigned long lastDebounce = 0;
static int currentButton;
static int lastButton = HIGH;
static unsigned long lastButtonChange = 0;

PantallaSistema pantallaActual = PANTALLA_HOME;
PantallaSistema pantallaAnterior = PANTALLA_HOME;
unsigned long tiempoUltimaActividad = 0;
ContenidoPantalla contenidoActual;

extern unsigned long tiempoUltimaInteraccionMenu;

// =============================================
// ✅ NUEVO: Declaración de función auxiliar
// =============================================
void procesarGiroEncoder(int delta);

// ✅ Línea con texto centrado
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

// ✅ Dibuja todas las zonas completas
void dibujarContenidoPantalla(const ContenidoPantalla& c) {
  display.fillScreen(BLACK);

  // ZONA SUPERIOR (2 líneas, textSize=2)
  dibujarLineaCentrada(2,  c.linea1, 2, c.fondo1, c.texto1);
  dibujarLineaCentrada(22, c.linea2, 2, c.fondo1, c.texto1);

  // ZONA CENTRAL (2 líneas, textSize=2)
  dibujarLineaCentrada(44, c.linea3, 2, c.fondo2, c.texto2);
  dibujarLineaCentrada(64, c.linea4, 2, c.fondo2, c.texto2);

  // ZONA INFERIOR (1 línea, textSize=3)
  dibujarLineaCentrada(90, c.linea5, 3, c.fondo3, c.texto3);
}

// ✅ Dibuja solo la zona afectada
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
  if (pantallaActual == PANTALLA_TEST) return;
  if (nota == ultimaNotaRecibida &&
      vel == ultimaVelocidadRecibida &&
      canal == ultimoCanalRecibido) {
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
    0x001F, 0xFFFF,
    0x7BEF, 0xFFFF,
    0x0000, 0xFFFF
  };

  actualizarZonaPantalla(ZONA_SUPERIOR, contenidoActual);
  actualizarZonaPantalla(ZONA_CENTRAL, contenidoActual);
  actualizarZonaPantalla(ZONA_INFERIOR, contenidoActual);
}

void entradaEventoCC(uint8_t mux, uint8_t cc, uint8_t val) {
  if (pantallaActual == PANTALLA_TEST) return;

  for (int i = 0; i < numControles; i++) {
    if (controles[i].cc == cc && controles[i].mux == mux) {
      if (controles[i].valor == val) {
        return;
      }
      controles[i].valor = val;
      break;
    }
  }

  if (mux <= 6) {
    pantallaActual = PANTALLA_STANDBY_KNOB;
    contenidoActual = {
      "SURFACE", "GRUPO N",
      "KNOB CC " + String(cc) + " - CH 1",
      "[VALOR CC %]", String(val * 100 / 127) + "%",
      0xF800, 0xFFFF,
      0x39E7, 0xFFFF,
      0x0000, 0xFFFF
    };
  } else {
    pantallaActual = PANTALLA_STANDBY_SWITCH;
    contenidoActual = {
      "SURFACE", "GRUPO N",
      "SWITCH CC " + String(cc) + " - CH 1",
      "<ON / OFF>", val > 63 ? "<ON>" : "<OFF>",
      0xF800, 0xFFFF,
      0x39E7, 0xFFFF,
      0x0000, 0xFFFF
    };
  }

  tiempoUltimaActividad = millis();
  actualizarZonaPantalla(ZONA_CENTRAL, contenidoActual);
  actualizarZonaPantalla(ZONA_INFERIOR, contenidoActual);
}

void clicEncoder() {
  pantallaAnterior = pantallaActual;
  pantallaActual = PANTALLA_MENU;
  inicializarMenuUnico();
  mostrarPantallaUnica(display);
  tiempoUltimaActividad = millis();
  tiempoUltimaInteraccionMenu = millis();
}

void actualizarPantallaSistema() {
  unsigned long ahora = millis();

  if (pantallaActual == PANTALLA_HOME && ahora - tiempoUltimaActividad > 2000) {
    pantallaActual = PANTALLA_TEST;
    estadoSistema = ESTADO_READY;

    String surfaceEstado = secuenciadorRutaActiva ? "[ON]" : "[OFF]";
    String secuenciadorEstado = secuenciadorGlobalActivo ? "[ON]" : "[OFF]";
    String testStatus = "[READY]";

    contenidoActual = {
      "SP1 / Mk2", "MIDI CONTROL",
      "SURFACE " + surfaceEstado, "SECUENCER " + secuenciadorEstado,
      testStatus,
      0x07E0, 0x0000,
      0x7BEF, 0x0000,
      0x0000, 0x07E0
    };

    dibujarContenidoPantalla(contenidoActual);
    tiempoUltimaActividad = ahora;
    return;
  }

  if (pantallaActual == PANTALLA_MENU && (ahora - tiempoUltimaInteraccionMenu > 5000)) {
    pantallaActual = pantallaAnterior;
    dibujarContenidoPantalla(contenidoActual);
    return;
  }

  if (pantallaActual == PANTALLA_MENU) {
    mostrarPantallaUnica(display);
  }
}

// =============================================
// ✅ FUNCIÓN AUXILIAR PARA PROCESAR GIRO ENCODER
// =============================================
void procesarGiroEncoder(int delta) {
  if (pantallaActual == PANTALLA_MENU) {
    rotarMenu(delta);
    actualizarTextosMenuUnico();
    mostrarPantallaUnica(display);
    tiempoUltimaInteraccionMenu = millis();
  }
  // Podemos añadir más comportamientos en otras pantallas aquí
}

// ============================================================
// ✅ GESTIÓN COMPLETA DEL ROTARY ENCODER
// ============================================================
void inicializarEncoder() {
  pinMode(ENCODER_A_PIN, INPUT_PULLUP);
  pinMode(ENCODER_B_PIN, INPUT_PULLUP);
  pinMode(ENCODER_BUTTON_PIN, INPUT_PULLUP);
  
  // Leer estado inicial
  lastA = digitalRead(ENCODER_A_PIN);
  lastB = digitalRead(ENCODER_B_PIN);
  lastButton = digitalRead(ENCODER_BUTTON_PIN);
}

void gestionarEncoder() {
  unsigned long ahora = millis();
  static int lastEncoded = 0;
  
  // Leer estado actual de los pines
  int pinA = digitalRead(ENCODER_A_PIN);
  int pinB = digitalRead(ENCODER_B_PIN);
  
  // ----------------------------------------------------
  // DETECCIÓN DE GIRO (MÁQUINA DE ESTADOS DE CUADRATURA)
  // ----------------------------------------------------
  int encoded = (pinA << 1) | pinB;
  int sum = (lastEncoded << 2) | encoded;
  
  // Detectar transiciones válidas para giro antihorario
  if (sum == 0b1101 || sum == 0b0100 || sum == 0b0010 || sum == 0b1011) {
    procesarGiroEncoder(-1);
  } 
  // Detectar transiciones válidas para giro horario
  else if (sum == 0b1110 || sum == 0b0111 || sum == 0b0001 || sum == 0b1000) {
    procesarGiroEncoder(1);
  }
  
  lastEncoded = encoded;

  // ----------------------------------------
  // DETECCIÓN DE CLIC (CON DEBOUNCE MEJORADO)
  // ----------------------------------------
  int currentButton = digitalRead(ENCODER_BUTTON_PIN);
  
  // Detectar cambio de estado
  if (currentButton != lastButton) {
    lastButtonChange = ahora;
    lastButton = currentButton;
  }
  
  // Detectar pulsación mantenida (50ms para evitar rebotes)
  if (currentButton == LOW && (ahora - lastButtonChange) > 50) {
    // Solo procesar si es una pulsación nueva
    if (lastButtonChange == ahora - 50) {
      if (pantallaActual == PANTALLA_MENU) {
        clickCortoMenuUnico();
        actualizarTextosMenuUnico();
        mostrarPantallaUnica(display);
        tiempoUltimaInteraccionMenu = ahora;
      } else {
        clicEncoder();
      }
    }
    
    // Esperar hasta que se suelte el botón
    while (digitalRead(ENCODER_BUTTON_PIN) == LOW) {
      delay(1);
    }
    delay(10); // Pequeño delay adicional para estabilizar
  }
}