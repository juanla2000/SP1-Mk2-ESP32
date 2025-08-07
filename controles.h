// controles.h

#pragma once

#include <Arduino.h>
#include <Adafruit_SSD1351.h>
#include "hardware_config.h"

extern HardwareSerial midiSerial;
String obtenerNombreGrupo(uint8_t indice);
String obtenerNombreSubgrupo(uint8_t grupoIndex);
String obtenerNombreControl(uint8_t grupoIndex, uint8_t idControl);

// Estructura para definir un control físico
struct Control {
  String origen;
  String grupo;
  String nombre;
  uint8_t canal;
  uint8_t cc;
  uint8_t valor;
  uint8_t mux; // número de MUX (0–8), define si es KNOB (0–6) o SWITCH (7–8). NO editable.
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

// ✅ NUEVAS VARIABLES para detectar cambios en entradaEventoNota()
extern uint8_t ultimaNotaRecibida;
extern uint8_t ultimaVelocidadRecibida;
extern uint8_t ultimoCanalRecibido;

// Funciones
void enviarNotaSecuenciador(uint8_t canal, uint8_t nota, uint8_t velocidad);
void actualizarValor(uint8_t cc, uint8_t valor);
void inicializarControles();
void cargarPresetInicial();
void enviarNota(uint8_t nota, uint8_t velocidad);
void enviarTodosLosCCsConParametros();
void guardarPresetActual(const char* ruta);
void procesarComandoUART(const String& comando);
void actualizarUART();

// ✅ ESTADO GLOBAL DEL SISTEMA (BOOT / READY)
enum EstadoSistema {
  ESTADO_BOOT,
  ESTADO_READY
};

extern EstadoSistema estadoSistema;
