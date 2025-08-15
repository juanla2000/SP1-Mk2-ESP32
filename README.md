
# SP1 Mk2 MIDI Controller Firmware - Versión 0.6.1

## Descripción General
Firmware avanzado para controlador MIDI SP1 Mk2 que combina funcionalidades de superficie de control MIDI, teclado MIDI y secuenciador paso a paso. Diseñado para ESP32-S3 con interfaz OLED y navegación por encoder.

## Características Principales

### 🎛️ Sistema de Menú Unificado
- Interfaz estructurada en 3 zonas:
  - **Superior**: Menús principales/submenús
  - **Central**: Parámetros de grupo/controles
  - **Inferior**: Valores/acciones
- Navegación por encoder (giro + pulsación)
- Parpadeo visual para selección activa
- Timeout automático para volver a pantalla principal

### ⏱️ Secuenciador Paso a Paso
- **4 pistas** independientes
- Hasta **64 pasos configurables** por pista
- Parámetros editables por paso:
  - Activación (On/Off)
  - Nota musical (0-127)
  - Velocidad (0-127)
  - Duración (ticks)
- Configuración global:
  - Tempo (30-300 BPM)
  - Sincronización MIDI externa (MIDI Clock)
  - Modo Mono/Poli
  - Ajustes de Legato/Sustain

### 🎚️ Gestión de Controles MIDI
- **108 controles físicos** organizados
- Configuración por control:
  - Nombre personalizado
  - Canal MIDI (0-15)
  - Número CC (0-127)
  - Valor actual (0-127)
- Identificación mediante **9 multiplexores**

### 💾 Sistema de Presets
- Almacenamiento en tarjeta SD (CSV)
- Tipos:
  - Presets de Superficie (controladores)
  - Presets de Secuenciador
- Carga/guardado de configuración completa

### 🔌 Comunicación MIDI
- Dual:
  - **USB MIDI** (TinyUSB)
  - **UART MIDI** (31250 baudios)
- MIDI Thru configurable
- Sincronización con MIDI Clock externo

### 🖥️ Sistema de Pantallas Inteligente
- Modos:
  - **Home**: Estado del sistema
  - **Standby**: Eventos MIDI en tiempo real
  - **Menú**: Navegación completa
- Actualización por zonas
- Animación de inicio personalizada

## Estructura del Código

### Módulos Principales:
| Módulo | Función |
|--------|---------|
| `controles` | Gestión de controles físicos y presets |
| `secuenciador` | Motor del secuenciador paso a paso |
| `menu_unico` | Lógica completa del sistema de menús |
| `pantalla_navegacion` | Máquina de estados de pantallas |
| `pantalla_unica` | Renderizado de interfaz |
| `configuracion` | Parámetros globales del sistema |
| `zona_menu` | Definición de zonas interactivas |

### Flujo Principal:
```mermaid
graph TD
    A[Inicio] --> B[Pantalla Bienvenida]
    B --> C[Inicializar Hardware]
    C --> D[Cargar Configuración]
    D --> E[Bucle Principal]
    E --> F[Gestionar MIDI USB]
    E --> G[Gestionar MIDI UART]
    E --> H[Actualizar Secuenciador]
    E --> I[Gestionar Encoder]
    E --> J[Actualizar Pantalla]
    J --> K{Menú Visible?}
    K -->|Sí| L[Actualizar Menú]
    K -->|No| M[Actualizar Pantalla Actual]