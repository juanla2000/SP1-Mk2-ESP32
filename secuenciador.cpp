// secuenciador.cpp

#include "secuenciador.h"
#include "controles.h"

Step secuencia[numSecuencias][totalSteps];
uint8_t currentStep = 0;
uint8_t currentSecuencia = 0;
unsigned long lastStepTime = 0;
unsigned long stepInterval = 200;

bool secuenciadorActivo[numSecuencias] = {false};
uint8_t secuenciadorNotas[numSecuencias] = {60}; // C4
bool secuenciadorZona[numSecuencias] = {false};

// ===========================
// Función auxiliar para notas
// ===========================

String notaToNombre(uint8_t nota) {
  static const char* nombres[] = {
    "C", "C#", "D", "D#", "E", "F",
    "F#", "G", "G#", "A", "A#", "B"
  };
  uint8_t octava = nota / 12;
  uint8_t notaBase = nota % 12;
  return String(nombres[notaBase]) + String(octava);
}

// ===========================
// Inicialización
// ===========================

void inicializarSecuenciador() {
  for (uint8_t seq = 0; seq < numSecuencias; seq++) {
    for (uint8_t step = 0; step < totalSteps; step++) {
      secuencia[seq][step] = {false, 60, 100, 200}; // Nota C4
    }
    secuenciadorActivo[seq] = false;
    secuenciadorNotas[seq] = 60;
    secuenciadorZona[seq] = false;
  }
  currentStep = 0;
  lastStepTime = millis();
}

// ===========================
// Avanzar paso en secuencia
// ===========================

void avanzarSecuencia() {
  unsigned long now = millis();
  if (now - lastStepTime >= stepInterval) {
    currentStep = (currentStep + 1) % totalSteps;
    lastStepTime = now;
  }
}

// ===========================
// Editar paso según control
// ===========================

void editarPaso(uint8_t cc, uint8_t valor) {
  if (cc == 20) {
    secuencia[currentSecuencia][currentStep].active = (valor > 63);
  }
  if (cc == 21) {
    secuencia[currentSecuencia][currentStep].note = valor;
  }
  if (cc == 22) {
    secuencia[currentSecuencia][currentStep].velocity = valor;
  }
  if (cc == 23) {
    secuencia[currentSecuencia][currentStep].duration = valor * 10; // ejemplo de escala
  }
}
