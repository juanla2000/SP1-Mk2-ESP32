// controles.cpp

#include "controles.h"
#include <SD.h>
#include <MIDI.h>
#include "menu_unico.h"  // Obtener nombres de grupo
#include "pantalla_navegacion.h"
#include "configuracion.h"  // ✅ acceso a bpmSyncEnabled y midiClockTicks

HardwareSerial midiSerial(1);
Adafruit_SSD1351 display = Adafruit_SSD1351(SCREEN_WIDTH, SCREEN_HEIGHT, &SPI, OLED_CS, OLED_DC, OLED_RST);

bool secuenciaTecladoLinkeada = false;
uint8_t pantallaActiva = 0;
bool estadoParpadeo = false;
unsigned long tiempoUltimoParpadeo = 0;
bool midiThruEnabled = true;
bool shiftPulsado = false;
unsigned long tiempoShift = 0;
bool cambiandoZona = false;

unsigned long tiempoUltimaInteraccionMenu = 0;
bool menuActivoAnterior = false;

Control controles[numControles];

// ✅ Variables para evitar redibujos repetidos en entradaEventoNota()
uint8_t ultimaNotaRecibida = 255;
uint8_t ultimaVelocidadRecibida = 255;
uint8_t ultimoCanalRecibido = 255;

// ✅ Estado global del sistema
EstadoSistema estadoSistema = ESTADO_BOOT;

void inicializarControles() {
  for (int i = 0; i < numControles; i++) {
    controles[i].valor = 64;
    controles[i].mux = i / 12;
  }
}

void actualizarValor(uint8_t cc, uint8_t valor) {
  for (int i = 0; i < numControles; i++) {
    if (controles[i].cc == cc) {
      controles[i].valor = valor;
      break;
    }
  }
}

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

    String partes[7];
    int campo = 0;
    int inicio = 0;
    int fin = linea.indexOf(',');

    while (fin != -1 && campo < 6) {
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
    controles[i].mux = constrain(partes[6].toInt(), 0, 8);  // validación mux
    i++;
  }

  archivo.close();
}

void enviarNotaSecuenciador(uint8_t canal, uint8_t nota, uint8_t velocidad) {
  if (muteSequencerNotes) return;
  midiSerial.write(0x90 | (canal & 0x0F));
  midiSerial.write(nota);
  midiSerial.write(velocidad);
}

void enviarNota(uint8_t nota, uint8_t velocidad) {
  enviarNotaSecuenciador(0, nota, velocidad);
}

void procesarComandoUART(const String& comando) {
  if (estadoSistema != ESTADO_READY) return;  // ✅ protección profesional

  static int controlSeleccionado = 0;

  if (comando.startsWith("#ID:")) {
    controlSeleccionado = comando.substring(4).toInt();
  } else if (comando.startsWith("#VALUE:")) {
    int valor = comando.substring(7).toInt();
    if (controlSeleccionado >= 0 && controlSeleccionado < numControles) {
      controles[controlSeleccionado].valor = constrain(valor, 0, 127);
      entradaEventoCC(controles[controlSeleccionado].mux, controles[controlSeleccionado].cc, controles[controlSeleccionado].valor);
      tiempoUltimaInteraccionMenu = millis();
    }
  } else if (comando.startsWith("#CC:")) {
    int sep = comando.indexOf(":", 4);
    if (sep > 0) {
      int id = comando.substring(4, sep).toInt();
      int nuevoCC = comando.substring(sep + 1).toInt();
      if (id >= 0 && id < numControles) {
        controles[id].cc = constrain(nuevoCC, 0, 127);
      }
    }
  } else if (comando.startsWith("#CHANNEL:")) {
    int sep = comando.indexOf(":", 9);
    if (sep > 0) {
      int id = comando.substring(9, sep).toInt();
      int nuevoCanal = comando.substring(sep + 1).toInt();
      if (id >= 0 && id < numControles) {
        controles[id].canal = constrain(nuevoCanal, 0, 15);
      }
    }
  } else if (comando.startsWith("#GROUP:")) {
    int sep = comando.indexOf(":", 7);
    if (sep > 0) {
      int id = comando.substring(7, sep).toInt();
      if (id >= 0 && id < numControles) {
        controles[id].grupo = comando.substring(sep + 1);
      }
    }
  } else if (comando.startsWith("#NAME:")) {
    int sep = comando.indexOf(":", 6);
    if (sep > 0) {
      int id = comando.substring(6, sep).toInt();
      if (id >= 0 && id < numControles) {
        controles[id].nombre = comando.substring(sep + 1);
      }
    }
  }
}

void actualizarUART() {
  static String uartBuffer = "";

  while (midiSerial.available()) {
    char c = midiSerial.read();

    if ((uint8_t)c == 0xF8) {
      if (bpmSyncEnabled) {
        midiClockTicks++;
      }
      continue;
    }

    if (c == '\n' || c == '\r') {
      if (uartBuffer.length() > 0) {
        procesarComandoUART(uartBuffer);
        uartBuffer = "";
      }
    } else {
      uartBuffer += c;
    }
  }
}

void enviarTodosLosCCsConParametros() {
  for (int i = 0; i < numControles; i++) {
    midiSerial.print("#ID:"); midiSerial.println(i);
    midiSerial.print("#GROUP:"); midiSerial.print(i); midiSerial.print(":"); midiSerial.println(controles[i].grupo);
    midiSerial.print("#NAME:");  midiSerial.print(i); midiSerial.print(":"); midiSerial.println(controles[i].nombre);
    midiSerial.print("#CHANNEL:"); midiSerial.print(i); midiSerial.print(":"); midiSerial.println(controles[i].canal);
    midiSerial.print("#CC:"); midiSerial.print(i); midiSerial.print(":"); midiSerial.println(controles[i].cc);
    midiSerial.print("#VALUE:"); midiSerial.println(controles[i].valor);
    midiSerial.print("#MUX:"); midiSerial.print(i); midiSerial.print(":"); midiSerial.println(controles[i].mux);
  }
}

void guardarPresetActual(const char* ruta) {
  SD.remove(ruta);
  File archivo = SD.open(ruta, FILE_WRITE);
  if (!archivo) {
    Serial.println("No se pudo abrir el archivo para guardar.");
    return;
  }

  for (int i = 0; i < numControles; i++) {
    archivo.print(controles[i].origen); archivo.print(",");
    archivo.print(controles[i].grupo); archivo.print(",");
    archivo.print(controles[i].nombre); archivo.print(",");
    archivo.print(controles[i].canal); archivo.print(",");
    archivo.print(controles[i].cc); archivo.print(",");
    archivo.print(controles[i].valor); archivo.print(",");
    archivo.println(controles[i].mux);
  }

  archivo.close();
  Serial.println("Preset guardado correctamente.");
}

String obtenerNombreGrupo(uint8_t indice) {
  if (indice >= NUM_MENUS_SUPERIOR1) return "UNKNOWN";
  const char* nombres[] = {
    "KEYBOARD", "SURFACE", "SEQUENCER", "PRESETS SURF", 
    "PRESETS SEQ", "CFG KEY", "CFG SEQ", "CFG SURF"
  };
  return nombres[indice];
}

String obtenerNombreSubgrupo(uint8_t grupoIndex) {
  if (grupoIndex >= numControles) return "";
  return controles[grupoIndex].grupo;
}

String obtenerNombreControl(uint8_t grupoIndex, uint8_t idControl) {
  if (grupoIndex >= numControles) return "";
  return controles[idControl].nombre;
}
