// pantalla_inicio.cpp

#include <Adafruit_SSD1351.h>
#include "pantalla_inicio.h"

void showStartupAnimation(Adafruit_SSD1351& display) {
  display.fillScreen(0x0000); // Negro

  display.setCursor(10, 20);
  display.setTextColor(0xFFFF); // Blanco
  display.setTextSize(1);
  display.print("FIRMWARE MIDI SP1");

  display.setCursor(10, 40);
  display.setTextColor(0x07E0); // Verde
  display.print("Cargando...");

  delay(1000); // Simula carga
}
