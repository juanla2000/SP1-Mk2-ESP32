// pantalla_navegacion.cpp

#include "pantalla_navegacion.h"
#include "pantalla_unica.h"
#include "menu_unico.h"
#include "controles.h"
#include "configuracion.h"

static int lastA = HIGH;
static int currentA;
static unsigned long lastDebounce = 0;
static int currentButton;
static int lastButton = HIGH;
static unsigned long lastButtonChange = 0;

PantallaSistema pantallaActual = PANTALLA_HOME;
PantallaSistema pantallaAnterior = PANTALLA_HOME;
unsigned long tiempoUltimaActividad = 0;
ContenidoPantalla contenidoActual;

extern unsigned long tiempoUltimaInteraccionMenu;

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

// ============================================================
// ✅ NUEVO BLOQUE: ROTARY ENCODER (LECTURA DIGITAL POLLING)
// ============================================================

void inicializarEncoder() {
  pinMode(ENCODER_A_PIN, INPUT_PULLUP);
  pinMode(ENCODER_B_PIN, INPUT_PULLUP);
  pinMode(ENCODER_BUTTON_PIN, INPUT_PULLUP);
}

void gestionarEncoder() {
  // ... código existente ...
  if (currentA != lastA && ahora - lastDebounce > 5) {
    if (currentA == LOW) {
      int delta = (currentB == HIGH) ? +1 : -1;
      
      if (pantallaActual == PANTALLA_MENU) {
        cambiarValorInferior(delta);
        actualizarTextosMenuUnico();
        mostrarPantallaUnica(display);
        tiempoUltimaInteraccionMenu = ahora;
      }
    }
  }
  lastA = currentA;

  // ----------------------------------------
  // DETECCIÓN DE CLIC
  // ----------------------------------------
  static unsigned long lastButtonChange = 0;
  if (currentButton != lastButton) {
    lastButtonChange = ahora;
    lastButton = currentButton;
  }

  if (lastButton == LOW && (ahora - lastButtonChange) > 50) {
    // Clic detectado
    if (pantallaActual == PANTALLA_MENU) {
      clickCortoMenuUnico();
      actualizarTextosMenuUnico();
      mostrarPantallaUnica(display);
      tiempoUltimaInteraccionMenu = ahora;
    } else {
      clicEncoder();  // entra en menú desde fuera
    }

    while (digitalRead(ENCODER_BUTTON_PIN) == LOW);  // espera a soltar
    delay(10);  // rebote
  }
}
