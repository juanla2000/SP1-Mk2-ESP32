//controles.h

#pragma once

#include <Arduino.h>
#include <Adafruit_SSD1351.h>
#include "hardware_config.h"

extern HardwareSerial midiSerial;

// Estructura para definir un control físico
struct Control {
  String origen;
  String grupo;
  String nombre;
  uint8_t canal;
  uint8_t cc;
  uint8_t valor;
};

const uint8_t numControles = 108;
extern Control controles[numControles];

// Variables globales compartidas entre módulos
extern bool secuenciaTecladoLinkeada;
extern uint8_t pantallaActiva;
extern bool estadoParpadeo;
extern unsigned long tiempoUltimoParpadeo;
extern bool midiThruEnabled;
extern bool shiftPulsado;
extern unsigned long tiempoShift;
extern bool cambiandoZona;
extern bool tecladoSecuenciaEnabled;
extern bool muteSequencerNotes;
extern unsigned long lastStepTime;
extern unsigned long stepInterval;

extern Adafruit_SSD1351 display;

// Funciones
void enviarNotaSecuenciador(uint8_t canal, uint8_t nota, uint8_t velocidad);
void actualizarValor(uint8_t cc, uint8_t valor);
void inicializarControles();
void cargarPresetInicial();
void enviarNota(uint8_t nota, uint8_t velocidad);
