// FIRMWARE_SP1_Mk2_ESP32S3.ino

#define CFG_TUD_MIDI 1
#include <Arduino.h>
#include <SPI.h>
#include <EncoderButton.h>
#include "hardware_config.h"
#include "pantalla_inicio.h"
#include "pantalla_navegacion.h"
#include "pantalla_unica.h"
#include "menu_unico.h"
#include "controles.h"
#include "configuracion.h"
#include "secuenciador.h"
#include "zona_menu.h"
#include <Adafruit_TinyUSB.h>

// Instancia global de EncoderButton
EncoderButton eb(ENCODER_DT_PIN, ENCODER_CLK_PIN, ENCODER_BUTTON_PIN);

Adafruit_USBD_MIDI usb_midi;

// Variables para gestión de tiempo
unsigned long tiempoUltimaInteraccionMenu = 0;
bool lastButtonState = false; // Estado anterior del botón para detección de flanco

void setup() {
  Serial.begin(115200);
  midiSerial.begin(MIDI_BAUDRATE);

  // Inicialización SPI
  SPI.begin(PIN_SPI_SCK, PIN_SPI_MISO, PIN_SPI_MOSI);

  usb_midi.begin();
  display.begin();

  // Pantalla de bienvenida
  ContenidoPantalla bienvenida = {
    "SP/1", "Mk2",
    "TANGIBLE", "Ver. 0.6.1",
    "MIDI Control",
    0xF800, 0x0000,
    0x0000, 0xFFFF,
    0xC618, 0x0000
  };
  dibujarContenidoPantalla(bienvenida);
  delay(2000);

  inicializarControles();
  cargarConfig();
  inicializarSecuenciador();

  // Configuración del encoder (con resistencias físicas externas)
  pinMode(ENCODER_BUTTON_PIN, INPUT); // Sin pull-up interno

  pantallaActual = PANTALLA_HOME;
  pantallaAnterior = PANTALLA_HOME;
  tiempoUltimaActividad = millis();

  Serial.println("Sistema iniciado");
}

void loop() {
  actualizarClockUSB();
  actualizarUART();
  avanzarPaso();
  apagarNotaSecuenciador();

  // -----------------------------------------
  // Gestión del encoder - VERSIÓN CORREGIDA
  // -----------------------------------------
  eb.update();  // Actualiza estado del encoder
  
  // 1. Detectar giro del encoder con manejo correcto
  static int32_t lastEncoderValue = 0;
  int32_t currentEncoderValue = eb.position();
  
  if (currentEncoderValue != lastEncoderValue) {
    int32_t incremento = currentEncoderValue - lastEncoderValue;
    lastEncoderValue = currentEncoderValue;
    
    // Solo procesar cambios de 1 paso (clic por clic)
    if (incremento != 0) {
      Serial.printf("Giro encoder detectado: %d\n", incremento);
      
      // Normalizar a +1 o -1
      int dir = (incremento > 0) ? 1 : -1;
      procesarGiroEncoder(dir);
    }
  }
  
  // 2. Detectar pulsación del botón (detección de flanco)
  bool currentButtonState = eb.isPressed();
  if (currentButtonState && !lastButtonState) {
    Serial.println("Botón encoder presionado");
    if (pantallaActual == PANTALLA_MENU) {
      clickCortoMenuUnico();
      actualizarTextosMenuUnico();
      mostrarPantallaUnica(display);
      tiempoUltimaInteraccionMenu = millis();
    } else {
      clicEncoder();
    }
  }
  lastButtonState = currentButtonState;

  // -----------------------------------------
  // Control de parpadeo
  // -----------------------------------------
  if (pantallaActual == PANTALLA_MENU) {
    if (estadoMenuUnico.zonaActiva == ZONA_SUPERIOR ||
        estadoMenuUnico.zonaActiva == ZONA_CENTRAL ||
        estadoMenuUnico.zonaActiva == ZONA_INFERIOR) {

      if (millis() - tiempoUltimoParpadeo > 500) {
        estadoParpadeo = !estadoParpadeo;
        tiempoUltimoParpadeo = millis();
        estadoMenuUnico.parpadeo = estadoParpadeo;
      }

    } else {
      estadoParpadeo = false;
      estadoMenuUnico.parpadeo = false;
    }
  } else {
    estadoParpadeo = false;
    estadoMenuUnico.parpadeo = false;
  }

  actualizarPantallaSistema();
  
  // Monitoreo del sistema
  static unsigned long lastPrint = 0;
  if(millis() - lastPrint > 1000) {
    lastPrint = millis();
    Serial.printf("[SISTEMA] Uptime: %lu seg\n", lastPrint/1000);
    Serial.printf("[ENCODER] Valor actual: %ld\n", eb.position());
  }
  
  static unsigned long lastMemCheck = 0;
  if(millis() - lastMemCheck > 5000) {
    lastMemCheck = millis();
    printMemoryStats();
  }
}

void actualizarClockUSB() {
  if (!bpmSyncEnabled) return;

  while (usb_midi.available()) {
    uint8_t byte = usb_midi.read();
    if (byte == 0xF8) {
      midiClockTicks++;
    }
  }
}

void enviarCCUSB(uint8_t canal, uint8_t cc, uint8_t valor) {
  uint8_t msg[] = { static_cast<uint8_t>(0xB0 | (canal & 0x0F)), cc, valor };
  usb_midi.write(msg, 3);
}

void enviarNotaUSB(uint8_t canal, uint8_t nota, uint8_t velocidad) {
  uint8_t msg[] = { static_cast<uint8_t>(0x90 | (canal & 0x0F)), nota, velocidad };
  usb_midi.write(msg, 3);
}

void apagarNotaUSB(uint8_t canal, uint8_t nota) {
  uint8_t msg[] = { static_cast<uint8_t>(0x80 | (canal & 0x0F)), nota, 0 };
  usb_midi.write(msg, 3);
}

void printMemoryStats() {
  Serial.printf("[MEMORIA] Free heap: %d bytes\n", ESP.getFreeHeap());
  Serial.printf("[MEMORIA] Min free heap: %d bytes\n", ESP.getMinFreeHeap());
  Serial.printf("[MEMORIA] Heap size: %d bytes\n", ESP.getHeapSize());
}