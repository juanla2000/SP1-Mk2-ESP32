// pantalla_unica.cpp

#define BLACK 0x0000
#define WHITE 0xFFFF

#include "pantalla_unica.h"
#include "menu_unico.h"
#include "controles.h"
#include <Adafruit_SSD1351.h>

extern Adafruit_SSD1351 display;
extern EstadoMenuUnico estadoMenuUnico;

// ✅ Dibuja texto centrado
void dibujarTextoCentrado(uint8_t y, const String& texto, uint8_t textSize, uint16_t color) {
  display.setTextSize(textSize);
  int16_t x1, y1;
  uint16_t w, h;
  display.getTextBounds(texto, 0, y, &x1, &y1, &w, &h);
  int16_t xCentrado = (SCREEN_WIDTH - w) / 2;
  display.setCursor(xCentrado, y);
  display.setTextColor(color);
  display.print(texto);
}

void mostrarPantallaUnica(Adafruit_SSD1351& d) {
  d.fillScreen(estadoMenuUnico.colorFondo);

  // -------------------------
  // ZONA SUPERIOR (2 líneas, tamaño 2)
  // -------------------------
  {
    uint16_t color1 = (estadoMenuUnico.zonaActiva == ZONA_SUPERIOR &&
                       estadoMenuUnico.subzonaActiva == 0 &&
                       estadoMenuUnico.parpadeo) ? BLACK : WHITE;

    uint16_t color2 = (estadoMenuUnico.zonaActiva == ZONA_SUPERIOR &&
                       estadoMenuUnico.subzonaActiva == 1 &&
                       estadoMenuUnico.parpadeo) ? BLACK : WHITE;

    dibujarTextoCentrado(2,  estadoMenuUnico.nombreSuperior1, 2, color1);
    dibujarTextoCentrado(22, estadoMenuUnico.nombreSuperior2, 2, color2);
  }

  // -------------------------
  // ZONA CENTRAL (2 líneas, tamaño 2)
  // -------------------------
  {
    uint16_t color1 = (estadoMenuUnico.zonaActiva == ZONA_CENTRAL &&
                       estadoMenuUnico.subzonaActiva == 0 &&
                       estadoMenuUnico.parpadeo) ? BLACK : WHITE;

    uint16_t color2 = (estadoMenuUnico.zonaActiva == ZONA_CENTRAL &&
                       estadoMenuUnico.subzonaActiva == 1 &&
                       estadoMenuUnico.parpadeo) ? BLACK : WHITE;

    dibujarTextoCentrado(44, estadoMenuUnico.nombreCentral1, 2, color1);
    dibujarTextoCentrado(64, estadoMenuUnico.nombreCentral2, 2, color2);
  }

  // -------------------------
  // ZONA INFERIOR (1 línea, tamaño 3)
  // -------------------------
  {
    uint16_t color = (estadoMenuUnico.zonaActiva == ZONA_INFERIOR &&
                      estadoMenuUnico.parpadeo) ? BLACK : WHITE;

    dibujarTextoCentrado(90, estadoMenuUnico.valorInferiorSeleccionado, 3, color);
  }
}
