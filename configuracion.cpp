// configuracion.cpp

#include "configuracion.h"

// Variables locales a configuración (NO duplicadas)
bool secuenciadorGlobalActivo = true;   // Activa y desactiva el secuenciador
bool secuenciadorRutaActiva   = true;   // Activa y desactiva...
bool secuenciadorRutaSolo     = false;  // Activa y desactiva que solo salgan notas del secuenciador
bool tecladoSecuenciaEnabled  = false;  // Activa que las notas del teclado afecten al secuenciador
uint16_t estadoTempo = 174;             // valor por defecto
bool bpmSyncEnabled = false;            // ✅ NUEVO: sincronía MIDI externa desactivada por defecto
volatile uint8_t midiClockTicks = 0;    // ✅ NUEVO: contador de ticks de reloj MIDI externo
const uint8_t ticksPorStep = 6;         // ✅ NUEVO: 6 ticks MIDI = 1/16 nota → 16 pasos por compás

uint8_t porcentajeLegato = 0;
uint8_t porcentajeSustain = 100;
bool modoMono = true;
bool muteSequencerNotes = false;  // ✅ Coherencia con menu_unico

// Variables adicionales requeridas por menu_unico.cpp
bool tecladoActivo = true;          // Activa y desactiva la entrada MIDI de un teclado o instrumento externo
bool secuenciadorZonaActiva = true; // Activa la zona dedicada en la superficie de control al secuenciador
bool superficieActiva = true;       // Activa y desactiva los controles de la superficie de control sin afectar a la zona del secuenciador

//Grupo de saltos del secuenciador maximo permitido, puede configurarse con 8 pasos, 16 pasos, 24 pasos, etc...
const uint8_t saltosMaxSteps[] = {
  8, 16, 24, 32, 40, 48, 56, 64,
  72, 80, 88, 96, 104, 112, 120, 128
};

// Configuración inicial por defecto
void cargarConfig() {
  secuenciadorGlobalActivo = true;      // 
  secuenciadorRutaActiva   = true;      // 
  secuenciadorRutaSolo     = false;     // 
  tecladoSecuenciaEnabled  = false;     // 
  bpmSyncEnabled           = false;     // ✅ coherente con el uso en menú
  midiClockTicks           = 0;         // ✅ reinicio del contador al arrancar
}
