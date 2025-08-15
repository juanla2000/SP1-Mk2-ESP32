//menu_unico.h

#pragma once

#include <Arduino.h>
#include <vector>
#include "zona_menu.h"
#include "hardware_config.h"
#include "controles.h"

String obtenerNombreGrupo(uint8_t indice);
String obtenerNombreSubgrupo(uint8_t grupoIndex);
String obtenerNombreControl(uint8_t grupoIndex, uint8_t idControl);

// ================================
// DECLARACIONES EXTERN PARA VARIABLES COMPARTIDAS
// ================================
extern bool modoMono;
extern bool bpmSyncEnabled;
extern bool secuenciaTecladoLinkeada;
extern bool tecladoActivo;
extern bool secuenciadorZonaActiva;
extern bool superficieActiva;
extern bool muteSequencerNotes;
extern uint16_t estadoTempo;
extern uint8_t porcentajeLegato;
extern uint8_t porcentajeSustain;
extern uint8_t indicePistaActiva;
extern Step secuencia[NUM_SEQUENCERS][MAX_STEPS_CONFIGURABLE];

// ================================
// ENUMS PARA MENÚ UNIFICADO
// ================================

enum TipoMenuSuperior {
  MENU_MIDI_KEYBOARD,
  MENU_MIDI_SURFACE,
  MENU_SEQUENCER,
  MENU_PRESETS_SURFACE,
  MENU_PRESETS_SEQUENCER,
  MENU_CONFIG_KEYBOARD,
  MENU_CONFIG_SEQUENCER,
  MENU_CONFIG_SURFACE,
  NUM_MENUS_SUPERIOR1
};

enum TipoSubmenuSuperior {
  SUBMENU_NONE,
  SUBMENU_TRIGGER,
  SUBMENU_NOTE,
  SUBMENU_VELOCITY,
  SUBMENU_CURVE,
  SUBMENU_LONG,
  SUBMENU_TOTAL_STEPS,
  SUBMENU_MAX_STEPS,
  SUBMENU_MODE,
  SUBMENU_LEGATO,
  SUBMENU_SUSTAIN,  // Corregido de SUSTAIN (anteriormente SUSTAIN)
  SUBMENU_PRESETS,
  SUBMENU_CONFIG_SURFACE_SEQUENCER,
  SUBMENU_CONFIG_SURFACE_MIDI,
  SUBMENU_CONFIG_KEYBOARD_OUTPUT,
  SUBMENU_CONFIG_KEYBOARD_LINK,
  SUBMENU_CONFIG_SEQUENCER_OUTPUT,
  SUBMENU_BPM,           // ✅ NUEVO: Edición del tempo BPM
  SUBMENU_BPM_SYNC,      // ✅ NUEVO: Activar/desactivar sincronía MIDI externa
  NUM_SUBMENUS_SUPERIOR2
};

// ================================
// ESTRUCTURA DE ESTADO DEL MENÚ (ACTUALIZADA)
// ================================

struct EstadoMenuUnico {
  ZonaMenu zonaActiva = ZONA_SUPERIOR;  // ✔️ Ahora ZonaMenu está definido
  uint8_t subzonaActiva = 0;

  // Índices seleccionados
  uint8_t indiceSuperior1 = MENU_SEQUENCER;
  uint8_t submenuSuperior = SUBMENU_TRIGGER;
  uint8_t indiceSuperior2 = 0;
  uint8_t indiceCentral1 = 0;
  uint8_t indiceCentral2 = 0;
  uint8_t indiceInferior1 = 0; // Declaración ya corregida

  // Cantidad de opciones disponibles en cada zona
  uint8_t numOpcionesSuperior1 = NUM_MENUS_SUPERIOR1;
  uint8_t numOpcionesSuperior2 = NUM_SUBMENUS_SUPERIOR2;
  uint8_t numOpcionesCentral1 = 1;
  uint8_t numOpcionesCentral2 = 1;

  // Estado de interacción
  bool confirmando = false;
  bool parpadeo = false;

  // Textos para mostrar en pantalla
  String nombreSuperior1;
  String nombreSuperior2;
  String nombreCentral1;
  String nombreCentral2;
  String valorInferiorSeleccionado;

  // Color de fondo de pantalla
  uint16_t colorFondo = 0x0000;
  
  // ================================
  // NUEVOS MIEMBROS PARA NAVEGACIÓN POR PASOS
  // ================================
  uint8_t faseNavegacion = 0;  // 0: inactivo, 1: fila1, 2: fila2, 3: fila3, 4: fila4
  unsigned long ultimaInteraccion = 0;
};

extern EstadoMenuUnico estadoMenuUnico;

// ================================
// FUNCIONES DEL MENÚ UNIFICADO (ACTUALIZADAS)
// ================================

void inicializarMenuUnico();
void avanzarZonaMenuUnico();
void clickCortoMenuUnico();
void confirmarAccionMenuUnico();
void cancelarAccionMenuUnico();
void actualizarTextosMenuUnico();
void cambiarValorInferior(int delta);
void corregirSubmenuInvalido();

// Nueva función para manejar timeout
void actualizarTimeoutMenu();

// Nueva función para navegación por pasos
void avanzarFaseMenu();

// ================================
// PRESETS SURFACE / SEQUENCER
// ================================

extern std::vector<String> archivosPresetsSurface;
extern std::vector<String> archivosPresetsSequencer;

void cargarListaPresetsSurface();
void cargarListaPresetsSequencer();

// ================================
// NUEVAS FUNCIONES ESPECÍFICAS PARA NAVEGACIÓN
// ================================

// Función para manejar la rotación del encoder en modo menú
void rotarMenu(int direccion);

// Función para dibujar el estado actual con parpadeo
void dibujarMenuConParpadeo();