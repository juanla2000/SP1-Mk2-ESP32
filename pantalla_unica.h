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
// Paleta de colores oficial (RGB565)
#define COLOR_FONDO_SUPERIOR 0x001F   // Azul oscuro
#define COLOR_TEXTO_SUPERIOR 0xFFFF   // Blanco
#define COLOR_FONDO_CENTRAL  0x7BEF   // Gris
#define COLOR_TEXTO_CENTRAL  0x0000   // Negro
#define COLOR_FONDO_INFERIOR 0x0000   // Negro
#define COLOR_TEXTO_INFERIOR 0xFFFF   // Blanco

// ================================
// FUNCIONES DE PANTALLA ÚNICA
// ================================
void mostrarPantallaUnica(Adafruit_SSD1351& d);

// ✅ NUEVA función para dibujar texto centrado a diferentes tamaños
void dibujarTextoCentrado(uint8_t y, const String& texto, uint8_t textSize, uint16_t color);

void dibujarLineaConParpadeo(uint8_t y, const String& texto, bool esActiva, bool parpadeando, 
                            uint16_t colorFondo, uint16_t colorTexto);
