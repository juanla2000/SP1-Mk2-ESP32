# SP1 GENERAL README (SP1 Midi Control) / (SP1MK2 Midi Control)
Sp1 Tangible Mk2

# FIRMWARE_MIDI_SP1

Controlador MIDI modular diseñado para sintetizadores tipo Serum, con una interfaz de usuario inspirada en su estructura visual. Implementado sobre ESP32-S3 y controlado con un encoder rotativo. Esta versión integra un menú único centralizado, presets en tarjeta SD y un secuenciador de pasos.

---

# Estructura del proyecto

- `FIRMWARE_MIDI_SP1.ino`: archivo principal con lógica de `setup()` y `loop()`.
- `hardware_config.h`: define pines y configuraciones de hardware.
- `controles.h/.cpp`: gestiona controles físicos (knobs y switches) y preset inicial.
- `configuracion.h/.cpp`: configuración global del sistema cargada al inicio.
- `secuenciador.h/.cpp`: paso a paso de hasta 4 pistas, editable por CC.
- `menu_unico.h/.cpp`: navegación estructurada por encoder (LOAD, SAVE, DELETE).
- `pantalla_unica.h/.cpp`: interfaz gráfica OLED para vista de controles o secuenciador.
- `pantalla_inicio.h/.cpp`: animación de arranque.
- `zona_menu.h`: define las zonas del menú (superior, central, inferior).
- `/Presets Surface/`: presets de controles (CSV).
- `/Presets Seq/`: (pendiente) presets de secuencia.

---

# Funciones implementadas

# Configuración general
- `cargarConfig()`: activa MIDI Thru, rutas del secuenciador, etc.
- Pines definidos en `hardware_config.h`.
- Variables globales eliminadas de múltiples archivos para evitar duplicaciones.

# Controles
- 108 controles físicos definidos en `controles.cpp`.
- Se cargan desde un archivo `.csv` al arrancar (`SERUM_SP1_SURFACE_PRESET01.csv`).
- CC MIDI actualiza cada control por coincidencia.
- Envío de notas MIDI incluido.

# Secuenciador
- Matriz `Step secuencia[numSecuencias][totalSteps]`.
- Variables: nota, velocidad, duración, activación.
- Avance por tiempo `stepInterval`.
- Editar valores por CCs: 20–23.

# Gestión de presets
- Menú dividido en zonas:
  - **Superior:** LOAD / SAVE / DELETE
  - **Central:** selección nombre de preset
  - **Inferior:** confirmación OK / CANCEL
- Presets en `/Presets Surface/`.
- Carga desde SD lista.
- Guardado y borrado pendiente.

# Pantallas OLED
- `pantalla_unica.cpp`: vista gráfica dinámica según `pantallaActiva`.
- `pantalla_inicio.cpp`: animación al arrancar.
- Parpadeo gestionado con `tiempoUltimoParpadeo`.

---

# Pendiente por implementar

# Menú de Presets
- Finalizar SAVE de preset desde valores actuales de `controles[]`.
- DELETE: borrar archivo seleccionado de SD.

# Menú CONFIG (modular)
  - Navegación y edición de parámetros como:
  - Enlace teclado→secuenciador (`secuenciaTecladoLinkeada`)
  - Mutear notas del secuenciador
  - Guardado persistente si se desea (EEPROM / SD).

# Secuenciador
- [ ] Posible sincronización por MIDI Clock (futuro).
- [ ] Añadir edición directa en pantalla (OLED) por pasos.

# Visual
- [ ] Diferenciar visualmente zonas del menú.
- [ ] Mostrar nombres de presets cargados.

---

# Librerías necesarias

- `Adafruit_SSD1351`
- `Adafruit_GFX`
- `Adafruit_BusIO`
- `SD`
- `SPI`, `Wire`, `FS` (nativas)
- `EncoderButton`
- `Encoder`
- `Bounce2`
- `MIDI Library`

---

# Últimos cambios clave (Julio 2025)
- Separación de `hardware_config.h`, `configuracion.h`, `controles.h`
- Eliminación de variables duplicadas (`lastStepTime`, `tecladoSecuenciaEnabled`).
- Revisión completa de interdependencias entre archivos.
- Refactor completo del `menu_unico.cpp`.

---

# Arquitectura modular

```text
main.ino
 ├── controles.{h,cpp} ──> control físico y presets
 ├── secuenciador.{h,cpp} ──> lógica de pasos y edición
 ├── menu_unico.{h,cpp} ──> navegación tipo Serum
 ├── pantalla_unica.{h,cpp} ──> OLED principal
 ├── configuracion.{h,cpp} ──> flags globales
 └── pantalla_inicio.{h,cpp} ──> animación de arranque
```

---

Para cualquier ampliación futura, se recomienda mantener esta estructura modular.

