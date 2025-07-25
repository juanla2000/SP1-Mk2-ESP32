//controles.cpp

#include "controles.h"
#include <SD.h>
#include <MIDI.h>

// ===============================
// Objetos instanciados externamente
// ===============================

HardwareSerial midiSerial(1);  // UART1 del ESP32-S3
Adafruit_SSD1351 display = Adafruit_SSD1351(SCREEN_WIDTH, SCREEN_HEIGHT, &SPI, OLED_CS, OLED_DC, OLED_RST);

// ===============================
// Definición de variables globales compartidas
// ===============================

bool secuenciaTecladoLinkeada = false;
uint8_t pantallaActiva = 0;
bool estadoParpadeo = false;
unsigned long tiempoUltimoParpadeo = 0;
bool midiThruEnabled = true;
bool shiftPulsado = false;
unsigned long tiempoShift = 0;
bool cambiandoZona = false;
bool tecladoSecuenciaEnabled = true;
bool muteSequencerNotes = false;

// ===============================
// Definición del array de controles
// ===============================

Control controles[numControles];

// ===============================
// Inicializar controles
// ===============================

void inicializarControles() {
  for (int i = 0; i < numControles; i++) {
    controles[i].valor = 64;
  }
}

// ===============================
// Actualizar valor de control
// ===============================

void actualizarValor(uint8_t cc, uint8_t valor) {
  for (int i = 0; i < numControles; i++) {
    if (controles[i].cc == cc) {
      controles[i].valor = valor;
      break;
    }
  }
}

// ===============================
// Cargar preset desde SD
// ===============================

void cargarPresetInicial() {
  File archivo = SD.open("/Presets Surface/SERUM_SP1_SURFACE_PRESET01.csv");
  if (!archivo) {
    Serial.println("No se pudo abrir el preset inicial.");
    return;
  }

  int i = 0;
  while (archivo.available() && i < numControles) {
    String linea = archivo.readStringUntil('\n');
    linea.trim();
    if (linea.length() == 0 || linea.startsWith("#")) continue;

    String partes[6];
    int campo = 0;
    int inicio = 0;
    int fin = linea.indexOf(',');

    while (fin != -1 && campo < 5) {
      partes[campo++] = linea.substring(inicio, fin);
      inicio = fin + 1;
      fin = linea.indexOf(',', inicio);
    }
    partes[campo] = linea.substring(inicio);

    controles[i].origen = partes[0];
    controles[i].grupo = partes[1];
    controles[i].nombre = partes[2];
    controles[i].canal = partes[3].toInt();
    controles[i].cc = partes[4].toInt();
    controles[i].valor = partes[5].toInt();
    i++;
  }

  archivo.close();
}

// ===============================
// Enviar nota desde el secuenciador
// ===============================

void enviarNotaSecuenciador(uint8_t canal, uint8_t nota, uint8_t velocidad) {
  if (muteSequencerNotes) return;
  midiSerial.write(0x90 | (canal & 0x0F));
  midiSerial.write(nota);
  midiSerial.write(velocidad);
}

void enviarNota(uint8_t nota, uint8_t velocidad) {
  uint8_t canal = 0;
  enviarNotaSecuenciador(canal, nota, velocidad);
}
