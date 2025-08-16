// pantalla_navegacion.h

#pragma once

#include <Arduino.h>
#include <Adafruit_SSD1351.h>
#include "pantalla_unica.h"
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
  PANTALLA_MENU,
  PANTALLA_READY,  // ✅ Asegurarse de que esta línea existe
  PANTALLA_INVALIDA = 0
};

// ===========================
// ESTRUCTURA PARA PANTALLA
// ===========================
struct ContenidoPantalla {
  String linea1, linea2, linea3, linea4, linea5;
  uint16_t fondo1, texto1;
  uint16_t fondo2, texto2;
  uint16_t fondo3, texto3;
  ContenidoPantalla() = default;
  ContenidoPantalla(String l1, String l2, String l3, String l4, String l5,
                   uint16_t f1, uint16_t t1, uint16_t f2, uint16_t t2,
                   uint16_t f3, uint16_t t3) : 
    linea1(l1), linea2(l2), linea3(l3), linea4(l4), linea5(l5),
    fondo1(f1), texto1(t1), fondo2(f2), texto2(t2), fondo3(f3), texto3(t3) {}
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
// ✅ FUNCIÓN PARA PROCESAR GIRO DEL ENCODER
// =====================================================
void procesarGiroEncoder(int delta);