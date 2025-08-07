// pantalla_navegacion.h

#pragma once

#include <Arduino.h>
#include <Adafruit_SSD1351.h>
#include "zona_menu.h"         // ✅ necesario para ZonaMenu
#include "hardware_config.h"   // ✅ por consistencia general del proyecto

// ===========================
// ENUM DE ESTADO DE PANTALLA
// ===========================
enum PantallaSistema {
  PANTALLA_HOME = 1,
  PANTALLA_TEST,
  PANTALLA_STANDBY_KNOB,
  PANTALLA_STANDBY_SWITCH,
  PANTALLA_STANDBY_NOTE,
  PANTALLA_STANDBY_CC,
  PANTALLA_MENU
};

// ===========================
// ESTRUCTURA PARA PANTALLA
// ===========================
struct ContenidoPantalla {
  String linea1, linea2, linea3, linea4, linea5;
  uint16_t fondo1, texto1;
  uint16_t fondo2, texto2;
  uint16_t fondo3, texto3;
};

// ===========================
// VARIABLES GLOBALES
// ===========================
extern PantallaSistema pantallaActual;
extern PantallaSistema pantallaAnterior;
extern unsigned long tiempoUltimaActividad;
extern ContenidoPantalla contenidoActual;

// ===========================
// FUNCIONES PRINCIPALES
// ===========================
void actualizarPantallaSistema();
void entradaEventoNota(uint8_t nota, uint8_t vel, uint8_t canal);
void entradaEventoCC(uint8_t mux, uint8_t cc, uint8_t val);
void clicEncoder();
void dibujarContenidoPantalla(const ContenidoPantalla& contenido);

// ✅ NUEVA FUNCIÓN: redibujo por zona específica
void actualizarZonaPantalla(ZonaMenu zona, const ContenidoPantalla& contenido);

// =====================================================
// ✅ NUEVO BLOQUE: GESTIÓN DE ROTARY ENCODER
// =====================================================

// Inicializa pines del encoder y estados previos
void inicializarEncoder();

// Llama periódicamente desde loop(): detecta giro y clics
void gestionarEncoder();
