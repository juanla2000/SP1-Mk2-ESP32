// pantalla_inicio.cpp

#include <Adafruit_SSD1351.h>
#include "controles.h" 
#include "pantalla_inicio.h"
#include "pantalla_unica.h"  // ✅ Para dibujarTextoCentrado()

void showStartupAnimation(Adafruit_SSD1351& display) {
  display.fillScreen(0x0000); // Fondo negro

  // Título superior (centrado, textSize=2)
  dibujarTextoCentrado(22, "FIRMWARE MIDI SP1", 2, 0xFFFF); // Blanco

  // Mensaje inferior (centrado, textSize=3)
  dibujarTextoCentrado(90, "CARGANDO...", 3, 0x07E0); // Verde

  delay(2000); // Mostrar durante 2 segundos completos
}
