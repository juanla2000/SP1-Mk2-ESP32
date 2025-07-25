//pantalla_unica.h

#pragma once

#include <Arduino.h>
#include <Adafruit_SSD1351.h>
#include <vector>
#include "zona_menu.h"
#include "hardware_config.h"
#include "controles.h"
#include "menu_unico.h"

// ================================
// DEFINICIONES DE COLOR
// ================================
#define BLACK 0x0000
#define WHITE 0xFFFF

// ================================
// FUNCIONES DE PANTALLA ÚNICA
// ================================
void mostrarPantallaUnica(Adafruit_SSD1351& d);