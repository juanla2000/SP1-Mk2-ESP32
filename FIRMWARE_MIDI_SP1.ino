// FIRMWARE_MIDI_SP1.ino - Archivo principal

#include <Arduino.h>
#include <Adafruit_SSD1351.h>
#include <SPI.h>
#include <Wire.h>
#include <SD.h>
#include <FS.h>
#include <EncoderButton.h>
#include <Encoder.h>
#include <Bounce2.h>
#include <MIDI.h>

#include "hardware_config.h"
#include "controles.h"
#include "zona_menu.h"
#include "menu_unico.h"
#include "pantalla_unica.h"
#include "secuenciador.h"
#include "configuracion.h"
#include "pantalla_inicio.h"

EncoderButton encoder(ENCODER_A_PIN, ENCODER_B_PIN, ENCODER_BUTTON_PIN);

bool enModoEdicion = false;

void setup() {
  midiSerial.begin(MIDI_BAUDRATE, SERIAL_8N1, MIDI_UART_RX, MIDI_UART_TX);
  display.begin();
  showStartupAnimation(display);
  SD.begin(SD_CS);

  cargarConfig();  // Carga configuraciones desde archivo
  cargarPresetInicial(); // Carga valores iniciales de controles

  inicializarControles();
  inicializarMenuUnico();
  inicializarSecuenciador();
}

void loop() {
  if (millis() - lastStepTime >= stepInterval) avanzarSecuencia();

  if (millis() - tiempoUltimoParpadeo > 300) {
    estadoParpadeo = !estadoParpadeo;
    tiempoUltimoParpadeo = millis();
  }

  encoder.update();

  if (!enModoEdicion) {
    if (encoder.increment()) {
      clickCortoMenuUnico();
    }
  } else {
    if (encoder.increment()) {
      int delta = encoder.increment() ? 1 : -1;
      if (estadoMenuUnico.zonaActiva == ZONA_CENTRAL && estadoMenuUnico.subzonaActiva == 0) {
        estadoMenuUnico.indiceCentral1 = constrain(estadoMenuUnico.indiceCentral1 + delta, 0, estadoMenuUnico.numOpcionesCentral1 - 1);
      } else if (estadoMenuUnico.zonaActiva == ZONA_CENTRAL && estadoMenuUnico.subzonaActiva == 1) {
        estadoMenuUnico.indiceCentral2 = constrain(estadoMenuUnico.indiceCentral2 + delta, 0, estadoMenuUnico.numOpcionesCentral2 - 1);
      } else if (estadoMenuUnico.zonaActiva == ZONA_INFERIOR) {
        estadoMenuUnico.confirmando = !estadoMenuUnico.confirmando;
      }
    }
  }

  if (encoder.isPressed()) {
    enModoEdicion = !enModoEdicion;
    if (!enModoEdicion && estadoMenuUnico.zonaActiva == ZONA_INFERIOR) {
      if (estadoMenuUnico.confirmando) confirmarAccionMenuUnico();
      else cancelarAccionMenuUnico();
    } else {
      avanzarZonaMenuUnico();
    }
  }

  if (midiSerial.available() >= 3) {
    uint8_t status = midiSerial.read();
    uint8_t data1 = midiSerial.read();
    uint8_t data2 = midiSerial.read();
    uint8_t type = status & 0xF0;
    uint8_t channel = (status & 0x0F) + 1;

    if (midiThruEnabled) {
      midiSerial.write(status);
      midiSerial.write(data1);
      midiSerial.write(data2);
    }

    switch (type) {
      case 0xB0:
        if (pantallaActiva == 0) actualizarValor(data1, data2);
        if (pantallaActiva == 1) {
          editarPaso(data1, data2);
          mostrarPantallaUnica(display);
        }
        break;
    }
  }

  switch (pantallaActiva) {
    case 1:
      mostrarPantallaUnica(display);
      break;
    default:
      mostrarPantallaUnica(display);
      break;
  }
}
