
# SP1 Mk2 v0.6— Firmware Profesional para Controlador MIDI Modular (ESP32-S3)

Firmware completo para el módulo SP1 Mk2, desarrollado sobre ESP32-S3, con navegación OLED por zonas, gestión avanzada de presets, secuenciador por pasos integrado, sincronización MIDI externa por USB y DIN, y comunicación UART con el módulo SP1 Mk1 que gestiona 108 controles físicos multiplexados.

---

## 🧠 Arquitectura general

```
                [ HOST USB ]
                     ▲
                 MIDI USB
                     │
        ┌────────────┴────────────┐
        │       SP1 Mk2 (ESP32-S3)│
        └────────────┬────────────┘
                     │ UART (31250 bps)
                     ▼
        ┌────────────┴────────────┐
        │       SP1 Mk1 (Arduino) │
        └─────────────────────────┘
```

---

## 📁 Estructura del proyecto

- `FIRMWARE_SP1_Mk2_ESP32S3.ino`: núcleo del sistema, ciclo `setup()` + `loop()`.
- `hardware_config.h`: mapeo de pines físicos.
- `controles.*`: lectura física + comunicación UART + envío MIDI.
- `configuracion.*`: flags de estado global, persistencia, opciones CONFIG.
- `secuenciador.*`: motor de reproducción, presets, BPM sync.
- `menu_unico.*`: menú dinámico por zonas.
- `pantalla_unica.*`: render OLED por zonas.
- `pantalla_navegacion.*`: navegación visual y eventos.
- `pantalla_inicio.*`: animación de arranque.
- `/Presets Surface/`: configuración de superficie en `.csv`.
- `/Presets Seq/`: configuración secuenciador en `.csv`.

---

## ✅ Funciones implementadas

### 🔧 Configuración global

- `cargarConfig()`, `guardarConfig()`: guarda y carga `bpmSyncEnabled`, `muteSequencerNotes`, `secuenciaTecladoLinkeada`.
- EEPROM virtual por bytes 0–2.
- Pines definidos en `hardware_config.h`.

### 🕹️ Control físico

- 108 controles multiplexados recibidos vía UART Mk1.
- Campos por control: `mux`, `cc`, `canal`, `valor`.
- Recepción de comandos `#MUX`, `#CC`, `#VAL`.
- Mapeo dinámico desde CSV: `SERUM_SP1_SURFACE_PRESET01.csv`.

### 🧠 Secuenciador

- 4 secuencias paralelas.
- Matriz `Step secuencia[][]`, campos: `active`, `note`, `velocity`, `inicio`, `duration`.
- Control MIDI externo por CC 20–23.
- Funciones:
  - `avanzarPaso()`
  - `apagarNotaSecuenciador()`
  - `obtenerPaso()`, `editarPaso()`
- Flags: `modoMono`, `porcentajeSustain`, `porcentajeLegato`, `secuenciadorGlobalActivo`, `muteSequencerNotes`.

### 🕒 Sincronización externa (Clock MIDI)

- `bpmSyncEnabled`: activa entrada `0xF8` desde USB o UART.
- `midiClockTicks`: incrementa al recibir Clock.
- `ticksPorStep`: define frecuencia de avance (por defecto = 6).
- `actualizarClockUSB()`: captura `0xF8` desde `usb_midi.read()`.

### 💾 Gestión de presets

- `guardarPresetSecuenciador()`, `cargarPresetSecuenciador()`.
- Formato `CSV`, estructuras `step`, `track`, `config`.
- Superficie y secuencias separadas (`/Presets Surface/`, `/Presets Seq/`).

### 🖥️ Pantallas OLED por zonas

- 5 líneas por pantalla: 2 sup., 2 cent., 1 inf.
- Navegación por encoder rotatorio.
- `pantallaActual`, `pantallaAnterior`, `tiempoUltimaActividad`.
- `mostrarPantallaUnica()`, `mostrarMenuPresets()`.

### 🧭 Menú único estructurado

- Navegación por zonas: `ZONA_SUPERIOR`, `ZONA_CENTRAL`, `ZONA_INFERIOR`.
- Clic corto/largo para cambiar entre opciones y confirmar acciones.
- Menús activos: PRESETS, SEQUENCER, CONFIG.

---

## 📌 Variables globales clave

| Variable                   | Descripción |
|----------------------------|-------------|
| `estadoTempo`             | BPM manual |
| `bpmSyncEnabled`          | Activación de Clock externo |
| `midiClockTicks`          | Cuenta mensajes 0xF8 |
| `ticksPorStep`            | Número de ticks por paso |
| `muteSequencerNotes`      | Silencio temporal |
| `modoMono`                | Mono vs. polifonía |
| `secuenciaTecladoLinkeada`| Sincronía con teclado |
| `porcentajeSustain`       | Duración de nota |
| `porcentajeLegato`        | Superposición entre notas |
| `secuenciadorGlobalActivo`| Pausa general del secuenciador |

---

## 🔌 Pines de hardware

| Nombre lógico      | GPIO | Función                  |
|--------------------|------|--------------------------|
| MIDI_UART_TX       | 14   | Salida MIDI DIN          |
| MIDI_UART_RX       | 13   | Entrada MIDI DIN         |
| ENCODER_A_PIN      | 34   | Encoder menú A           |
| ENCODER_B_PIN      | 35   | Encoder menú B           |
| ENCODER_BUTTON_PIN | 36   | Botón encoder            |
| OLED_CS            | 5    | Chip select OLED         |
| OLED_DC            | 16   | OLED D/C                 |
| OLED_RST           | 17   | OLED Reset               |
| SD_CS              | 10   | Chip select SD card      |

---

## 🎚️ Menú SEQUENCER – BMP y SYNC

| ID  | GRUPO         | ZONA SUP | ZONA CENT         | ZONA INF       | FUNCIONALIDAD                          |
|-----|---------------|----------|--------------------|----------------|----------------------------------------|
| 21  | MENU SEQUENCER| SEQUENCER BMP | SELECT / VALUE   | `[BMP VALOR]` | Ajusta tempo manual                   |
| 22  | MENU SEQUENCER| SEQUENCER SYNC| SELECT / BMP SYNC| `<ON / OFF>`  | Activa Clock MIDI externo              |

---

## 🔁 Función de sincronía avanzada

- `usb_midi.read()` y UART detectan `0xF8`.
- Si `bpmSyncEnabled`, se ignora `millis()` y se avanza por `ticksPorStep`.
- Compatible con DAWs: Ableton, FL Studio, Bitwig, MPC, etc.

---

## 📦 Librerías necesarias

- `Adafruit_GFX`, `Adafruit_SSD1351`, `Adafruit_BusIO`
- `Encoder`, `EncoderButton`, `Bounce2`
- `SD`, `SPI`, `Wire`, `FS`
- `MIDI Library`, `Adafruit_TinyUSB`

---

## 🛠️ Futuras ampliaciones

- Menú de rejilla (rejilla de paso: 1/4, 1/8, 1/16)
- Visualización de compás y grid en pantalla
- Soporte para `0xFA` (START), `0xFC` (STOP), `0xFB` (CONTINUE)
- Delete directo de presets
- Expansión multi-secuenciador y modulación cruzada

---

© 2025 Burdalo Design — Tatxanka
