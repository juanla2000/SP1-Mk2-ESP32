//pantalla_unica.h

#pragma once

#include <Arduino.h>
#include <Adafruit_SSD1351.h>
#include <vector>
#include "zona_menu.h"
#include "hardware_config.h"
#include "controles.h"
#include "menu_unico.h"

extern Adafruit_SSD1351 display;

// ================================
// DEFINICIONES DE COLOR
// ================================
#define BLACK 0x0000
#define WHITE 0xFFFF
#define COLOR_FONDO_MENU 0x0000   // Negro
#define COLOR_TEXTO_NORMAL 0xFFFF // Blanco
#define COLOR_TEXTO_PARPADEO 0x0000 // Negro (para parpadeo)

// ================================
// FUNCIONES DE PANTALLA ÚNICA
// ================================
void mostrarPantallaUnica(Adafruit_SSD1351& d);

// ✅ NUEVA función para dibujar texto centrado a diferentes tamaños
void dibujarTextoCentrado(uint8_t y, const String& texto, uint8_t textSize, uint16_t color);
