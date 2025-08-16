// configuracion.h

#pragma once

#include <Arduino.h>
#include "configuracion.h"  // Para muteSequencerNotes

// ==========================
// VARIABLES GLOBALES DE CONFIGURACIÓN
// ==========================

const unsigned long TIEMPO_PARPADEO = 200;  // ms para la pantalla
const unsigned long TIMEOUT_MENU = 5000;    // ms para salir del menu

extern uint16_t estadoTempo;
extern bool bpmSyncEnabled;              // ✅ NUEVO: permite activar sincronía externa MIDI

extern volatile uint8_t midiClockTicks;  // ✅ NUEVO: contador de ticks MIDI Clock
extern const uint8_t ticksPorStep;       // ✅ NUEVO: cuántos ticks avanzan un paso (por defecto 6 = 1/16 nota)

// Variables del secuenciador
extern uint8_t porcentajeLegato;
extern uint8_t porcentajeSustain;
extern bool modoMono;
extern bool muteSequencerNotes;

// Estado global del secuenciador
extern bool secuenciadorGlobalActivo;

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
// SISTEMA DE MENÚ (NUEVO)
// ==========================

// Estados de navegación del menú
enum EstadoMenu {
    MENU_INACTIVO,
    MENU_FILA1,
    MENU_FILA2,
    MENU_FILA3,
    MENU_FILA4
};

// Estructura para guardar el estado del menú
struct MenuState {
    EstadoMenu estadoActual;
    uint8_t seleccionFila1;
    uint8_t seleccionFila2;
    uint8_t seleccionFila3;
    uint8_t seleccionFila4;
    unsigned long ultimaInteraccion;
};

// Variable global del estado del menú
extern MenuState menuState;

// ==========================
// FUNCIÓN DE CONFIGURACIÓN
// ==========================

void cargarConfig();
