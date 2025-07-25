// configuracion.cpp

#include "configuracion.h"

// Variables locales a configuración (NO duplicadas)
bool secuenciadorGlobalActivo = true;
bool secuenciadorRutaActiva   = true;
bool secuenciadorRutaSolo     = false;

void cargarConfig() {
  // Configuración inicial por defecto
  secuenciadorGlobalActivo = true;
  secuenciadorRutaActiva   = true;
  secuenciadorRutaSolo     = false;
}
