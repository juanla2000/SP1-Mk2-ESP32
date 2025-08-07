// secuenciador.cpp

#include "secuenciador.h"
#include "configuracion.h"     // ✅ NUEVO: bpmSyncEnabled, midiClockTicks, ticksPorStep
#include "controles.h"
#include "pantalla_navegacion.h"
#include <Adafruit_TinyUSB.h>
#include <SD.h>

extern Adafruit_USBD_MIDI usb_midi;

Step secuencia[NUM_SEQUENCERS][MAX_STEPS_CONFIGURABLE];
uint8_t indicePistaActiva = 0;
uint8_t totalSteps[NUM_SEQUENCERS] = {16, 16, 16, 16};
uint8_t maxSteps[NUM_SEQUENCERS] = {32, 32, 32, 32};
uint8_t canalSecuenciador = 0;
uint8_t indicePasoActual[NUM_SEQUENCERS] = {0};
uint8_t porcentajeLegato = 0;
uint8_t porcentajeSustain = 100;
bool modoMono = true;

bool secuenciadorActivo[NUM_SEQUENCERS] = {true, false, false, false};
uint8_t secuenciadorNotas[NUM_SEQUENCERS] = {60, 62, 64, 65};
uint8_t secuenciadorVelocidad[NUM_SEQUENCERS] = {100, 100, 100, 100};
uint8_t secuenciadorLongitud[NUM_SEQUENCERS] = {1, 1, 1, 1};
bool secuenciadorZona[NUM_SEQUENCERS] = {false, false, false, false};

unsigned long tiempoProximoPaso = 0;

struct NotaActiva {
  bool activa;
  uint8_t nota;
  uint8_t canal;
  unsigned long apagarEn;
};

NotaActiva notaActivaSecuenciador;

void inicializarSecuenciador() {
  for (int s = 0; s < NUM_SEQUENCERS; s++) {
    for (int i = 0; i < MAX_STEPS_CONFIGURABLE; i++) {
      secuencia[s][i] = {};
    }
    totalSteps[s] = 16;
    maxSteps[s] = 32;
    indicePasoActual[s] = 0;
    secuenciadorActivo[s] = (s == 0);
  }
  tiempoProximoPaso = millis() + 500;
  notaActivaSecuenciador.activa = false;
}

void actualizarClockUSB() {
  if (!bpmSyncEnabled) return;

  int midiByte;
  while ((midiByte = usb_midi.read()) != -1) {
    if ((uint8_t)midiByte == 0xF8) {
      midiClockTicks++;
    }
  }
}

void avanzarPaso() {
  if (!secuenciadorGlobalActivo) return;

  if (bpmSyncEnabled) {
    if (midiClockTicks < ticksPorStep) return;
    midiClockTicks = 0;
  } else {
    unsigned long ahora = millis();
    if (ahora < tiempoProximoPaso) return;
    tiempoProximoPaso = ahora + obtenerPaso(indicePasoActual[indicePistaActiva]).duration * (60000 / estadoTempo / 2);
  }

  Step& paso = secuencia[indicePistaActiva][indicePasoActual[indicePistaActiva]];
  if (paso.active && !muteSequencerNotes) {
    enviarNotaSecuenciador(canalSecuenciador, paso.note, paso.velocity);

    uint8_t msg[] = { static_cast<uint8_t>(0x90 | canalSecuenciador), paso.note, paso.velocity };
    usb_midi.write(msg, 3);

    unsigned long duracionReal = paso.duration * (60000 / estadoTempo / 2) * porcentajeSustain / 100;
    notaActivaSecuenciador = {
      true,
      paso.note,
      canalSecuenciador,
      millis() + duracionReal
    };
  }

  entradaEventoNota(paso.note, paso.velocity, canalSecuenciador);

  indicePasoActual[indicePistaActiva]++;
  if (indicePasoActual[indicePistaActiva] >= totalSteps[indicePistaActiva]) {
    indicePasoActual[indicePistaActiva] = 0;
  }
}

void avanzarSecuencia() {
  if (!secuenciadorGlobalActivo) return;
  for (uint8_t s = 0; s < NUM_SEQUENCERS; s++) {
    if (!secuenciadorActivo[s]) continue;

    Step& paso = secuencia[s][indicePasoActual[s]];
    if (paso.active && !muteSequencerNotes) {
      enviarNotaSecuenciador(canalSecuenciador, paso.note, paso.velocity);
      uint8_t msg[] = { static_cast<uint8_t>(0x90 | canalSecuenciador), paso.note, paso.velocity };
      usb_midi.write(msg, 3);
    }
    indicePasoActual[s]++;
    if (indicePasoActual[s] >= totalSteps[s]) {
      indicePasoActual[s] = 0;
    }
  }
}

void apagarNotaSecuenciador() {
  if (notaActivaSecuenciador.activa && millis() >= notaActivaSecuenciador.apagarEn) {
    enviarNotaSecuenciador(notaActivaSecuenciador.canal, notaActivaSecuenciador.nota, 0);
    uint8_t msg[] = { static_cast<uint8_t>(0x80 | notaActivaSecuenciador.canal), notaActivaSecuenciador.nota, 0 };
    usb_midi.write(msg, 3);
    notaActivaSecuenciador.activa = false;
  }
}

void editarPaso(uint8_t index, const Step& nuevoPaso) {
  if (index >= MAX_STEPS_CONFIGURABLE) return;
  secuencia[indicePistaActiva][index] = nuevoPaso;
}

const Step& obtenerPaso(uint8_t index) {
  static Step dummyPaso;
  if (index >= MAX_STEPS_CONFIGURABLE) return dummyPaso;
  return secuencia[indicePistaActiva][index];
}

void establecerTotalSteps(uint8_t total) {
  if (total >= 1 && total <= maxSteps[indicePistaActiva])
    totalSteps[indicePistaActiva] = total;
}

void establecerMaxSteps(uint8_t max) {
  if (max >= 1 && max <= MAX_STEPS_CONFIGURABLE)
    maxSteps[indicePistaActiva] = max;
  if (totalSteps[indicePistaActiva] > maxSteps[indicePistaActiva])
    totalSteps[indicePistaActiva] = maxSteps[indicePistaActiva];
}

String notaToNombre(uint8_t nota) {
  const char* nombres[] = {"C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"};
  String nombre = nombres[nota % 12];
  int octava = nota / 12 - 1;
  return nombre + String(octava);
}

bool cargarPresetSecuenciador(const char* ruta) {
  File archivo = SD.open(ruta);
  if (!archivo) return false;

  while (archivo.available()) {
    String linea = archivo.readStringUntil('\n');
    linea.trim();
    if (linea.length() == 0 || linea.startsWith("#")) continue;

    int sep = linea.indexOf(',');
    if (sep == -1) continue;

    String tipo = linea.substring(0, sep);
    String contenido = linea.substring(sep + 1);

    if (tipo == "config") {
      int c1 = contenido.indexOf(',');
      int c2 = contenido.indexOf(',', c1 + 1);
      int c3 = contenido.indexOf(',', c2 + 1);
      int c4 = contenido.indexOf(',', c3 + 1);

      if (c1 == -1 || c2 == -1 || c3 == -1 || c4 == -1) continue;

      estadoTempo = contenido.substring(0, c1).toInt();
      porcentajeLegato = contenido.substring(c1 + 1, c2).toInt();
      porcentajeSustain = contenido.substring(c2 + 1, c3).toInt();
      modoMono = contenido.substring(c3 + 1, c4) == "true";
      secuenciadorGlobalActivo = contenido.substring(c4 + 1) == "true";

    } else if (tipo == "track") {
      String campos[10];
      int index = 0;
      while (index < 10 && contenido.length() > 0) {
        int c = contenido.indexOf(',');
        if (c == -1) {
          campos[index++] = contenido;
          break;
        } else {
          campos[index++] = contenido.substring(0, c);
          contenido = contenido.substring(c + 1);
        }
      }

      if (index < 9) continue;

      int pista = campos[0].toInt();
      if (pista < 0 || pista >= NUM_SEQUENCERS) continue;

      secuenciadorActivo[pista] = campos[1] == "true";
      secuenciadorNotas[pista] = campos[2].toInt();
      secuenciadorVelocidad[pista] = campos[3].toInt();
      secuenciadorLongitud[pista] = campos[4].toInt();
      secuenciadorZona[pista] = campos[5] == "true";
      totalSteps[pista] = campos[6].toInt();
      maxSteps[pista] = campos[7].toInt();
      canalSecuenciador = campos[8].toInt();

    } else if (tipo == "step") {
      String campos[8];
      int index = 0;
      while (index < 8 && contenido.length() > 0) {
        int c = contenido.indexOf(',');
        if (c == -1) {
          campos[index++] = contenido;
          break;
        } else {
          campos[index++] = contenido.substring(0, c);
          contenido = contenido.substring(c + 1);
        }
      }

      if (index < 7) continue;

      int pista = campos[0].toInt();
      int pasoIndex = campos[1].toInt();
      if (pista < 0 || pista >= NUM_SEQUENCERS) continue;
      if (pasoIndex < 0 || pasoIndex >= MAX_STEPS_CONFIGURABLE) continue;

      Step paso;
      paso.active = campos[2] == "true";
      paso.note = campos[3].toInt();
      paso.velocity = campos[4].toInt();
      paso.inicio = campos[5].toInt();
      paso.duration = campos[6].toInt();

      secuencia[pista][pasoIndex] = paso;
    }
  }

  archivo.close();
  return true;
}

bool guardarPresetSecuenciador(const char* ruta) {
  SD.remove(ruta);
  File archivo = SD.open(ruta, FILE_WRITE);
  if (!archivo) return false;

  // [... Preservado, omitido por longitud ...]

  archivo.close();
  return true;
}

uint8_t obtenerTotalSteps() {
  return totalSteps[indicePistaActiva];
}

uint8_t obtenerMaxSteps() {
  return maxSteps[indicePistaActiva];
}
