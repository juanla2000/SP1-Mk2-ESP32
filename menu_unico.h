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
  SUBMENU_NOTE,
  SUBMENU_CC,
  SUBMENU_TRIGGER,
  SUBMENU_PRESETS,
  SUBMENU_ROUTE,
  SUBMENU_ACTIVE,
  SUBMENU_SEQUENCER,
  SUBMENU_ZONE,
  SUBMENU_LONG,  // Este era requerido para SEQUENCER
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
  uint8_t indiceSuperior2 = SUBMENU_TRIGGER;
  uint8_t indiceCentral1 = 0;
  uint8_t indiceCentral2 = 0;

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
