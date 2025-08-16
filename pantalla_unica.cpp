// pantalla_unica.cpp

#define BLACK 0x0000
#define WHITE 0xFFFF

#include "pantalla_unica.h"
#include "menu_unico.h"
#include <Adafruit_SSD1351.h>
#include <Adafruit_GFX.h>

extern Adafruit_SSD1351 display;
extern EstadoMenuUnico estadoMenuUnico;

// ===========================================
// FUNCIÓN PARA DIBUJAR TEXTO CENTRADO
// ===========================================
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

// ===========================================
// FUNCIÓN PARA DIBUJAR UNA LÍNEA CON PARPADEO
// ===========================================
void dibujarLineaConParpadeo(uint8_t y, const String& texto, bool esActiva, bool parpadeando, 
                            uint16_t colorFondo, uint16_t colorTexto) {
    // Calcular altura de texto
    display.setTextSize(2);
    int16_t x1, y1;
    uint16_t w, h;
    display.getTextBounds("A", 0, y, &x1, &y1, &w, &h);
    
    // Limpiar solo el área necesaria
    display.fillRect(0, y, SCREEN_WIDTH, h + 4, colorFondo);
    
    // Aplicar parpadeo solo si es activa y está habilitado
    if(esActiva && parpadeando) {
        display.setTextColor(colorFondo, colorTexto);
        display.fillRect(0, y, SCREEN_WIDTH, h + 4, colorTexto);
    } else {
        display.setTextColor(colorTexto, colorFondo);
    }
    
    // Dibujar texto centrado (modificado para no usar getTextColor)
    display.setTextSize(2);
    int16_t x1_txt, y1_txt;
    uint16_t w_txt, h_txt;
    display.getTextBounds(texto, 0, y, &x1_txt, &y1_txt, &w_txt, &h_txt);
    int16_t xCentrado = (SCREEN_WIDTH - w_txt) / 2;
    display.setCursor(xCentrado, y + 2);
    display.print(texto);
}

// ===========================================
// FUNCIÓN PRINCIPAL PARA MOSTRAR PANTALLA
// ===========================================
void mostrarPantallaUnica(Adafruit_SSD1351& d) {
    // Obtener estado actual de navegación
    bool esZonaSuperiorActiva = (estadoMenuUnico.zonaActiva == ZONA_SUPERIOR);
    bool esZonaCentralActiva = (estadoMenuUnico.zonaActiva == ZONA_CENTRAL);
    bool esZonaInferiorActiva = (estadoMenuUnico.zonaActiva == ZONA_INFERIOR);
    
    // Dibujar zona superior (2 líneas)
    display.fillRect(0, 0, SCREEN_WIDTH, 40, COLOR_FONDO_SUPERIOR);
    dibujarLineaConParpadeo(2, estadoMenuUnico.nombreSuperior1, 
                          esZonaSuperiorActiva && estadoMenuUnico.subzonaActiva == 0, 
                          estadoMenuUnico.parpadeo,
                          COLOR_FONDO_SUPERIOR, COLOR_TEXTO_SUPERIOR);
    
    dibujarLineaConParpadeo(22, estadoMenuUnico.nombreSuperior2, 
                          esZonaSuperiorActiva && estadoMenuUnico.subzonaActiva == 1, 
                          estadoMenuUnico.parpadeo,
                          COLOR_FONDO_SUPERIOR, COLOR_TEXTO_SUPERIOR);
    
    // Dibujar zona central (2 líneas)
    display.fillRect(0, 40, SCREEN_WIDTH, 40, COLOR_FONDO_CENTRAL);
    dibujarLineaConParpadeo(44, estadoMenuUnico.nombreCentral1, 
                          esZonaCentralActiva && estadoMenuUnico.subzonaActiva == 0, 
                          estadoMenuUnico.parpadeo,
                          COLOR_FONDO_CENTRAL, COLOR_TEXTO_CENTRAL);
    
    dibujarLineaConParpadeo(64, estadoMenuUnico.nombreCentral2, 
                          esZonaCentralActiva && estadoMenuUnico.subzonaActiva == 1, 
                          estadoMenuUnico.parpadeo,
                          COLOR_FONDO_CENTRAL, COLOR_TEXTO_CENTRAL);
    
    // Dibujar zona inferior (1 línea grande)
    display.fillRect(0, 80, SCREEN_WIDTH, 48, COLOR_FONDO_INFERIOR);
    dibujarLineaConParpadeo(90, estadoMenuUnico.valorInferiorSeleccionado, 
                          esZonaInferiorActiva, 
                          estadoMenuUnico.parpadeo,
                          COLOR_FONDO_INFERIOR, COLOR_TEXTO_INFERIOR);
    
    // Dibujar separadores entre zonas (opcional)
    display.drawFastHLine(0, 40, SCREEN_WIDTH, 0x0000);  // Línea entre superior y central
    display.drawFastHLine(0, 80, SCREEN_WIDTH, 0x0000);  // Línea entre central e inferior
}