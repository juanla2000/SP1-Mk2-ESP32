// configuracion.h

#pragma once
#include <Arduino.h>

// ==========================
// VARIABLES GLOBALES DE CONFIGURACIÓN
// ==========================
extern uint16_t estadoTempo;
extern bool bpmSyncEnabled;              // ✅ NUEVO: permite activar sincronía externa MIDI

extern volatile uint8_t midiClockTicks;  // ✅ NUEVO: contador de ticks MIDI Clock
extern const uint8_t ticksPorStep;       // ✅ NUEVO: cuántos ticks avanzan un paso (por defecto 6 = 1/16 nota)

// Estado global del secuenciador
extern bool secuenciadorGlobalActivo;
extern bool muteSequencerNotes;

// Enrutado del secuenciador
extern bool secuenciadorRutaActiva;
extern bool secuenciadorRutaSolo;

// Control de entrada desde teclado al secuenciador
extern bool tecladoSecuenciaEnabled;

// Variables adicionales utilizadas en el menú de configuración
extern bool tecladoActivo;
extern bool secuenciadorZonaActiva;
extern bool superficieActiva;

// ==========================
// FUNCIÓN DE CONFIGURACIÓN
// ==========================

void cargarConfig();
