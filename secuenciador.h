//secuenciador.h

#pragma once

#include <Arduino.h>

// ==============================
// PARÁMETROS DEL SECUENCIADOR
// ==============================

const uint8_t totalSteps = 16;       // Pasos por secuencia
const uint8_t numSecuencias = 4;     // Número de pistas/sec. independientes

// ==============================
// ESTRUCTURA DE CADA PASO
// ==============================

struct Step {
  bool active = false;       // ¿Está activo este paso?
  uint8_t note = 60;         // Nota MIDI por defecto C4
  uint8_t velocity = 100;    // Velocidad MIDI 0-127
  uint16_t duration = 200;   // Duración en milisegundos
};

// ==============================
// VARIABLES DEL ESTADO GLOBAL
// ==============================

// Matriz de pasos: [secuencia][step]
extern Step secuencia[numSecuencias][totalSteps];

// Índices de ejecución
extern uint8_t currentStep;
extern uint8_t currentSecuencia;   // futura ampliación para varias pistas
extern uint32_t lastStepTime;
extern unsigned long stepInterval;

// Variables de control por secuencia
extern bool secuenciadorActivo[numSecuencias];       // Secuencia activa
extern uint8_t secuenciadorNotas[numSecuencias];     // Nota por secuencia
extern uint8_t secuenciadorVelocidad[numSecuencias]; // Vel. por secuencia
extern uint8_t secuenciadorLongitud[numSecuencias];  // Duración por secuencia
extern bool secuenciadorZona[numSecuencias];         // Zona dedicada ON/OFF

// Estados globales
extern bool secuenciadorGlobalActivo;
extern bool secuenciadorRutaSolo;
extern bool secuenciadorRutaActiva;
extern bool tecladoSecuenciaEnabled;

// ==============================
// FUNCIONES PRINCIPALES
// ==============================

void inicializarSecuenciador();
void avanzarSecuencia();
void editarPaso(uint8_t cc, uint8_t valor);
String notaToNombre(uint8_t nota);
