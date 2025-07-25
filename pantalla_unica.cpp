//pantalla_unica.cpp

#define BLACK 0x0000
#define WHITE 0xFFFF

#include "pantalla_unica.h"
#include "menu_unico.h"
#include "controles.h"
#include <Adafruit_SSD1351.h>

extern Adafruit_SSD1351 display;
extern EstadoMenuUnico estadoMenuUnico;

void mostrarPantallaUnica(Adafruit_SSD1351& d) {
  d.fillScreen(estadoMenuUnico.colorFondo);

  // -------------------------
  // ZONA SUPERIOR
  // -------------------------
  d.setCursor(0, 0);
  d.setTextColor(WHITE);
  if (estadoMenuUnico.zonaActiva == ZONA_SUPERIOR &&
      estadoMenuUnico.subzonaActiva == 0 &&
      estadoMenuUnico.parpadeo) {
    d.setTextColor(BLACK);
  }
  d.print(estadoMenuUnico.nombreSuperior1);

  d.setCursor(0, 10);
  d.setTextColor(WHITE);
  if (estadoMenuUnico.zonaActiva == ZONA_SUPERIOR &&
      estadoMenuUnico.subzonaActiva == 1 &&
      estadoMenuUnico.parpadeo) {
    d.setTextColor(BLACK);
  }
  d.print(estadoMenuUnico.nombreSuperior2);

  // -------------------------
  // ZONA CENTRAL
  // -------------------------
  d.setCursor(0, 30);
  d.setTextColor(WHITE);
  if (estadoMenuUnico.zonaActiva == ZONA_CENTRAL &&
      estadoMenuUnico.subzonaActiva == 0 &&
      estadoMenuUnico.parpadeo) {
    d.setTextColor(BLACK);
  }
  d.print(estadoMenuUnico.nombreCentral1);

  d.setCursor(0, 40);
  d.setTextColor(WHITE);
  if (estadoMenuUnico.zonaActiva == ZONA_CENTRAL &&
      estadoMenuUnico.subzonaActiva == 1 &&
      estadoMenuUnico.parpadeo) {
    d.setTextColor(BLACK);
  }
  d.print(estadoMenuUnico.nombreCentral2);

  // -------------------------
  // ZONA INFERIOR
  // -------------------------
  d.setCursor(0, 60);
  d.setTextColor(WHITE);
  if (estadoMenuUnico.zonaActiva == ZONA_INFERIOR &&
      estadoMenuUnico.parpadeo) {
    d.setTextColor(BLACK);
  }
  d.print(estadoMenuUnico.valorInferiorSeleccionado);
}
