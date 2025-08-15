//zona_menu.h

#pragma once

#include <Arduino.h>
#include "hardware_config.h"
#include "zona_menu.h"
#include "menu_unico.h"

// Enum para representar la zona activa del menú
enum ZonaMenu {
  ZONA_SUPERIOR,
  ZONA_CENTRAL,
  ZONA_INFERIOR
};

// Declaración de función para validar si un submenu pertenece al menú actual

bool submenuValidoParaMenu(uint8_t menu, uint8_t submenu) {
  // Implementación idéntica a la de menu_unico.cpp
  switch (menu) {
    case MENU_PRESETS_SURFACE:
    case MENU_PRESETS_SEQUENCER:
      return submenu == SUBMENU_PRESETS;
    case MENU_SEQUENCER:
      return (submenu >= SUBMENU_TRIGGER && submenu <= SUBMENU_LEGATO) ||
             submenu == SUBMENU_BPM || submenu == SUBMENU_BPM_SYNC;
    case MENU_CONFIG_KEYBOARD:
    case MENU_CONFIG_SEQUENCER:
    case MENU_CONFIG_SURFACE:
      return submenu >= SUBMENU_CONFIG_SURFACE_SEQUENCER && 
             submenu <= SUBMENU_CONFIG_SEQUENCER_OUTPUT;
    default:
      return false;
  }
}