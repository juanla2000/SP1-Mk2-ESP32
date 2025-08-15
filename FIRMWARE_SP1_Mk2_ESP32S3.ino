// FIRMWARE_SP1_Mk2_ESP32S3.ino

#define CFG_TUD_MIDI 1
#include <Arduino.h>
#include <SPI.h>
#include "hardware_config.h"
#include "pantalla_inicio.h"
#include "pantalla_navegacion.h"
#include "pantalla_unica.h"
#include "menu_unico.h"
#include "controles.h"
#include "configuracion.h"
#include "secuenciador.h"
#include "zona_menu.h"
#include <Adafruit_TinyUSB.h>

Adafruit_USBD_MIDI usb_midi;

void setup() {
  Serial.begin(115200);
  midiSerial.begin(MIDI_BAUDRATE);

  // Inicialización SPI personalizada para OLED y SD
  SPI.begin(12, 13, 11); // SCK, MISO, MOSI

  usb_midi.begin();

  display.begin();

  // ✅ Mostrar pantalla de bienvenida (diseño exacto)
  ContenidoPantalla bienvenida = {
    "SP/1", "Mk2",               // Superior
    "TANGIBLE", "Ver. 0.6.1",    // Central
    "MIDI Control",              // Inferior
    0xF800, 0x0000,              // Fondo rojo, texto negro
    0x0000, 0xFFFF,              // Fondo negro, texto blanco
    0xC618, 0x0000               // Fondo gris claro, texto negro
  };
  dibujarContenidoPantalla(bienvenida);

  delay(2000); // mantener pantalla de inicio 2 segundos

  inicializarControles();
  cargarConfig();
  inicializarSecuenciador();
  inicializarEncoder(); // ✅ NUEVO: inicializa pines encoder

  pantallaActual = PANTALLA_HOME;
  pantallaAnterior = PANTALLA_HOME;
  tiempoUltimaActividad = millis();

  // ✅ Se deja que actualizarPantallaSistema controle el flujo
}

void loop() {
  actualizarClockUSB();         // Sincronización desde USB MIDI Clock
  actualizarUART();             // UART MIDI
  avanzarPaso();                // Avance de pasos
  apagarNotaSecuenciador();     // Apaga nota activa si ha vencido

  // -----------------------------------------
  // ✅ NUEVO: Control inteligente de parpadeo
  // -----------------------------------------
  if (pantallaActual == PANTALLA_MENU) {
    if (estadoMenuUnico.zonaActiva == ZONA_SUPERIOR ||
        estadoMenuUnico.zonaActiva == ZONA_CENTRAL ||
        estadoMenuUnico.zonaActiva == ZONA_INFERIOR) {

      if (millis() - tiempoUltimoParpadeo > 500) {
        estadoParpadeo = !estadoParpadeo;
        tiempoUltimoParpadeo = millis();
        estadoMenuUnico.parpadeo = estadoParpadeo;
      }

    } else {
      estadoParpadeo = false;
      estadoMenuUnico.parpadeo = false;
    }
  } else {
    estadoParpadeo = false;
    estadoMenuUnico.parpadeo = false;
  }

  gestionarEncoder();           // ✅ NUEVO: leer giro y pulsación del encoder
  actualizarPantallaSistema();  // Gestión visual
}

void enviarCCUSB(uint8_t canal, uint8_t cc, uint8_t valor) {
  uint8_t msg[] = { static_cast<uint8_t>(0xB0 | (canal & 0x0F)), cc, valor };
  usb_midi.write(msg, 3);
}

void enviarNotaUSB(uint8_t canal, uint8_t nota, uint8_t velocidad) {
  uint8_t msg[] = { static_cast<uint8_t>(0x90 | (canal & 0x0F)), nota, velocidad };
  usb_midi.write(msg, 3);
}

void apagarNotaUSB(uint8_t canal, uint8_t nota) {
  uint8_t msg[] = { static_cast<uint8_t>(0x80 | (canal & 0x0F)), nota, 0 };
  usb_midi.write(msg, 3);
}
