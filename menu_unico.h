//menu_unico.h

#pragma once

#include <Arduino.h>
#include <vector>
#include "zona_menu.h"
#include "hardware_config.h"
#include "controles.h"

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
  SUBMENU_SUSTAIN,
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
// ESTRUCTURA DE ESTADO DEL MENÚ
// ================================

struct EstadoMenuUnico {
  ZonaMenu zonaActiva = ZONA_SUPERIOR;
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
};

extern EstadoMenuUnico estadoMenuUnico;

// ================================
// FUNCIONES DEL MENÚ UNIFICADO
// ================================

void inicializarMenuUnico();
void avanzarZonaMenuUnico();
void clickCortoMenuUnico();
void confirmarAccionMenuUnico();
void cancelarAccionMenuUnico();
void actualizarTextosMenuUnico();
void cambiarValorInferior(int delta);
void corregirSubmenuInvalido();

// ================================
// PRESETS SURFACE / SEQUENCER
// ================================

extern std::vector<String> archivosPresetsSurface;
extern std::vector<String> archivosPresetsSequencer;

void cargarListaPresetsSurface();
void cargarListaPresetsSequencer();
