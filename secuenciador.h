//secuenciador.h

#pragma once

#include <Arduino.h>

#define NUM_SEQUENCERS 4
#define MAX_STEPS_CONFIGURABLE 64

struct Step {
  bool active = false;
  uint8_t note = 60;
  uint8_t velocity = 100;
  uint16_t duration = 1; // duración en ticks dentro del step
  uint8_t inicio = 0;     // tick de inicio dentro del step
};

// Variables principales
extern Step secuencia[NUM_SEQUENCERS][MAX_STEPS_CONFIGURABLE];
extern uint8_t indicePistaActiva;
extern uint8_t totalSteps[NUM_SEQUENCERS];  // Era uint8_t
extern uint8_t maxSteps[NUM_SEQUENCERS];    // Era uint8_t
extern uint8_t canalSecuenciador;
extern uint8_t indicePasoActual[NUM_SEQUENCERS];
extern uint8_t porcentajeLegato;
extern uint8_t porcentajeSustain;
extern bool modoMono;

extern bool secuenciadorActivo[NUM_SEQUENCERS];
extern uint8_t secuenciadorNotas[NUM_SEQUENCERS];
extern uint8_t secuenciadorVelocidad[NUM_SEQUENCERS];
extern uint8_t secuenciadorLongitud[NUM_SEQUENCERS];
extern bool secuenciadorZona[NUM_SEQUENCERS];

// Inicialización y lógica de pasos
void inicializarSecuenciador();
void avanzarPaso();
void avanzarSecuencia();
void apagarNotaSecuenciador();

// Edición y lectura
void editarPaso(uint8_t index, const Step& nuevoPaso);
const Step& obtenerPaso(uint8_t index);
void establecerTotalSteps(uint8_t total);
void establecerMaxSteps(uint8_t max);
uint8_t obtenerTotalSteps();     // ✅ Añadido
uint8_t obtenerMaxSteps();       // ✅ Añadido
String notaToNombre(uint8_t nota);

// Presets
bool guardarPresetSecuenciador(const char* ruta);
bool cargarPresetSecuenciador(const char* ruta);

// ✅ NUEVA FUNCIÓN USB MIDI CLOCK
void actualizarClockUSB();

