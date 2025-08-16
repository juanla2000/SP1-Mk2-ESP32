# FIRMWARE MIDI SP1 Mk2 - Controlador MIDI Avanzado

![SP1 Mk2 MIDI Controller](https://example.com/sp1_mk2.jpg) <!-- Reemplazar con URL real de imagen -->

## Descripción del Proyecto
Firmware completo para el controlador MIDI SP1 Mk2 basado en ESP32-S3. Este sistema integra:
- Secuenciador polifónico de 4 pistas
- Superficie de control MIDI con 108 parámetros mapeables
- Interfaz gráfica OLED de 128x128 con sistema de menús
- Sincronización MIDI Clock externa
- Gestión de presets y configuración

## Características Clave

### 🎛️ Sistema de Control
- **108 controles físicos** (knobs, switches) mapeables a parámetros MIDI
- **Encoder rotativo** para navegación con pulsador integrado
- **Zonas de control** independientes (teclado, superficie, secuenciador)
- **MIDI Thru** y reenvió de mensajes

### ⏱️ Secuenciador Avanzado
- 4 pistas independientes con hasta 64 pasos configurables
- Parámetros por paso: nota, velocidad, duración, activación
- Modos mono/poly con ajuste de legato y sustain
- Sincronización interna (BPM) o externa (MIDI Clock)

### 🖥️ Interfaz de Usuario
- Pantalla OLED a color de 128x128 píxeles
- Sistema de menús jerárquico con 3 zonas interactivas:
  1. Zona Superior: Selección de modo principal
  2. Zona Central: Parámetros y ajustes
  3. Zona Inferior: Valores editables
- Animaciones de inicio y feedback visual

### 💾 Almacenamiento
- Gestión SD Card para presets
- Formatos compatibles:
  - `SERUM_SP1_SURFACE_PRESET##.csv`
  - `SERUM_SP1_SEQUENCER_PRESET##.csv`
- Sistema de carga/guardado rápido

## Hardware Requerido

### Componentes Principales
| Componente               | Especificaciones                     |
|--------------------------|--------------------------------------|
| Microcontrolador         | ESP32-S3 (240 MHz, 512KB RAM)        |
| Pantalla                 | OLED SSD1351 128x128 (SPI)           |
| Almacenamiento           | Micro SD Card (hasta 32GB)           |
| Encoder                  | Rotativo con pulsador (EC11)         |
| Entradas MIDI            | UART a 31250 baudios                 |
| USB                      | Tipo C con soporte MIDI USB          |

### Diagrama de Pines
| Función              | Pin ESP32 |
|----------------------|-----------|
| OLED CS              | 5         |
| OLED DC              | 15        |
| OLED RESET           | 16        |
| MOSI (SPI compartido)| 46        |
| MISO (SD Card)       | 14        |
| SCK (SPI compartido) | 12        |
| SD CS                | 8         |
| Encoder CLK          | 9         |
| Encoder DT           | 10        |
| Encoder SW           | 13        |
| MIDI TX              | 43        |
| MIDI RX              | 44        |

## Instalación y Compilación

### Requisitos Previos
1. **Arduino IDE** (v2.3.2 o superior)
2. **Plataforma ESP32 Arduino**:
   ```bash
   https://github.com/espressif/arduino-esp32/releases/tag/3.3.0
Librerías Requeridas:

arduino
Adafruit_SSD1351@^1.3.3
Adafruit_GFX_Library@^1.12.1
EncoderButton@^1.0.6
Adafruit_TinyUSB_Library@^3.7.1
MIDI_Library@^5.0.2
Pasos de Compilación
Clonar repositorio:

bash
git clone https://github.com/tuusuario/sp1-mk2-firmware.git
Abrir FIRMWARE_SP1_Mk2_ESP32S3.ino en Arduino IDE

Seleccionar placa: Adafruit Feather ESP32-S3 No PSRAM

Configurar opciones:

text
Upload Speed: 921600
USB Mode: CDC
Partition Scheme: TinyUF2
Flash Mode: QIO
Flash Size: 8MB
Compilar y cargar

Estructura del Proyecto
Directorios Principales
text
├── firmware/
│   ├── FIRMWARE_SP1_Mk2_ESP32S3.ino   # Programa principal
│   ├── configuracion.h/.cpp            # Parámetros globales
│   ├── controles.h/.cpp                # Gestión de controles MIDI
│   ├── secuenciador.h/.cpp             # Motor del secuenciador
│   ├── menu_unico.h/.cpp               # Sistema de menús
│   ├── pantalla_navegacion.h/.cpp      # Gestor de pantallas
│   ├── pantalla_unica.h/.cpp           # Interfaz gráfica
│   └── hardware_config.h               # Configuración de pines
│
├── presets/
│   ├── SERUM_SP1_SURFACE_PRESET01.csv  # Ejemplo preset superficie
│   └── SERUM_SP1_SEQUENCER_PRESET01.csv # Ejemplo preset secuenciador
│
└── docs/
    └── schematics/                     # Diagramas electrónicos
Flujo Principal
Diagram
Code
graph TD
    A[Inicio] --> B[Inicialización Hardware]
    B --> C[Pantalla de Bienvenida]
    C --> D[Loop Principal]
    D --> E[Lectura Controles]
    D --> F[Actualización Secuenciador]
    D --> G[Procesamiento MIDI]
    D --> H[Actualización Pantalla]
    E --> I[Procesar Encoder]
    E --> J[Procesar Botones]
    F --> K[Avanzar Pasos]
    G --> L[Leer MIDI USB]
    H --> M[Renderizar UI]
Uso Básico
Navegación Principal
Pulsación corta encoder:

Fuera de menú: Entrar al sistema de menús

En menú: Navegar entre zonas

Giro encoder:

Fuera de menú: Ajustar parámetro activo

En menú: Seleccionar opciones

Pulsación larga (3s): Salir de menú

Modos de Operación
Modo	Descripción
Modo Teclado	Control MIDI tradicional
Modo Superficie	Control de parámetros DAW/plugins
Modo Secuenciador	Programación de patrones rítmicos
Modo Presets	Gestión de configuraciones guardadas
Contribución
Se aceptan contribuciones bajo estas pautas:

Reportar issues en el sistema de seguimiento

Hacer fork del repositorio

Crear ramas descriptivas (feature/, fix/)

Enviar pull requests con documentación actualizada

Licencia
text
Copyright 2023 Tatxanka

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
Versión Firmware: 0.6.1
Última Actualización: 15 de Noviembre, 2023
Desarrollador Principal: Tatxanka
Contacto: info@midisp1.com

text

## Instrucciones para Uso
1. Copia todo el contenido entre los ```markdown
2. Pégalo en tu archivo README.md
3. Personaliza las secciones de contacto y URLs
4. Actualiza la imagen principal con una real de tu proyecto
5. Ajusta las fechas y versión según corresponda

Este README proporciona una documentación completa y profesional para tu proyecto, incluyendo todos los aspectos técnicos y de usuario necesarios para entender y utilizar el firmware del SP1 Mk2.