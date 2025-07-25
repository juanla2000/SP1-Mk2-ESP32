// configuracion.h

#pragma once
#include <Arduino.h>

// ==========================
// VARIABLES GLOBALES DE CONFIGURACIÓN
// ==========================

// Estado global del secuenciador
extern bool secuenciadorGlobalActivo;
extern bool muteSequencerNotes;

// Enrutado del secuenciador
extern bool secuenciadorRutaActiva;
extern bool secuenciadorRutaSolo;

// Control de entrada desde teclado al secuenciador
extern bool tecladoSecuenciaEnabled;

// ==========================
// FUNCIÓN DE CONFIGURACIÓN
// ==========================

void cargarConfig();