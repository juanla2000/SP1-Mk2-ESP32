//zona_menu.h

#pragma once

#include <Arduino.h>

// Enum para representar la zona activa del menú
enum ZonaMenu {
  ZONA_SUPERIOR,
  ZONA_CENTRAL,
  ZONA_INFERIOR
};

// Declaración de función para validar si un submenu pertenece al menú actual

bool submenuValidoParaMenu(uint8_t menu, uint8_t submenu); // Solo declaración