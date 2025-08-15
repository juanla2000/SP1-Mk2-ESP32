//menu_unico.cpp

#include "menu_unico.h"
#include "pantalla_unica.h"
#include "secuenciador.h"
#include "configuracion.h"
#include "controles.h"
#include "zona_menu.h"

EstadoMenuUnico estadoMenuUnico;

// ================================
// IMPLEMENTACIÓN DE FUNCIONES DE MENÚ
// ================================

void inicializarMenuUnico() {
  estadoMenuUnico.zonaActiva = ZONA_SUPERIOR;
  estadoMenuUnico.subzonaActiva = 0;
  estadoMenuUnico.indiceSuperior1 = MENU_SEQUENCER;
  estadoMenuUnico.submenuSuperior = SUBMENU_TRIGGER;
  estadoMenuUnico.indiceSuperior2 = 0;
  estadoMenuUnico.indiceCentral1 = 0;
  estadoMenuUnico.indiceCentral2 = 0;
  estadoMenuUnico.indiceInferior1 = 0;
  estadoMenuUnico.confirmando = false;
  estadoMenuUnico.parpadeo = false;
  estadoMenuUnico.faseNavegacion = 0;
  estadoMenuUnico.ultimaInteraccion = millis();
  estadoMenuUnico.colorFondo = 0x0000;
  
  corregirSubmenuInvalido();
  actualizarTextosMenuUnico();
}

void avanzarZonaMenuUnico() {
  switch (estadoMenuUnico.zonaActiva) {
    case ZONA_SUPERIOR:
      estadoMenuUnico.zonaActiva = ZONA_CENTRAL;
      estadoMenuUnico.subzonaActiva = 0;
      break;
    case ZONA_CENTRAL:
      estadoMenuUnico.zonaActiva = ZONA_INFERIOR;
      estadoMenuUnico.subzonaActiva = 0;
      break;
    case ZONA_INFERIOR:
      estadoMenuUnico.zonaActiva = ZONA_SUPERIOR;
      estadoMenuUnico.subzonaActiva = 0;
      break;
  }
  estadoMenuUnico.ultimaInteraccion = millis();
}

void clickCortoMenuUnico() {
  estadoMenuUnico.ultimaInteraccion = millis();
  
  switch (estadoMenuUnico.zonaActiva) {
    case ZONA_SUPERIOR:
      if (estadoMenuUnico.subzonaActiva == 0) {
        // Cambiar menú principal
        estadoMenuUnico.indiceSuperior1++;
        if (estadoMenuUnico.indiceSuperior1 >= NUM_MENUS_SUPERIOR1) {
          estadoMenuUnico.indiceSuperior1 = 0;
        }
        corregirSubmenuInvalido();
      } else {
        // Cambiar submenú
        estadoMenuUnico.submenuSuperior++;
        if (!submenuValidoParaMenu(estadoMenuUnico.indiceSuperior1, estadoMenuUnico.submenuSuperior)) {
          // Buscar el siguiente submenú válido
          for (uint8_t i = 0; i < NUM_SUBMENUS_SUPERIOR2; i++) {
            if (submenuValidoParaMenu(estadoMenuUnico.indiceSuperior1, i)) {
              estadoMenuUnico.submenuSuperior = i;
              break;
            }
          }
        }
      }
      break;
      
    case ZONA_CENTRAL:
      if (estadoMenuUnico.subzonaActiva == 0) {
        // Cambiar grupo/pista
        estadoMenuUnico.indiceCentral1++;
        if (estadoMenuUnico.indiceCentral1 >= 4) { // 4 pistas máximo
          estadoMenuUnico.indiceCentral1 = 0;
        }
        indicePistaActiva = estadoMenuUnico.indiceCentral1;
      } else {
        // Cambiar paso/control
        estadoMenuUnico.indiceCentral2++;
        if (estadoMenuUnico.indiceCentral2 >= 16) { // 16 pasos por defecto
          estadoMenuUnico.indiceCentral2 = 0;
        }
      }
      break;
      
    case ZONA_INFERIOR:
      // En zona inferior, avanzar a siguiente zona
      avanzarZonaMenuUnico();
      break;
  }
  
  actualizarTextosMenuUnico();
}

void confirmarAccionMenuUnico() {
  estadoMenuUnico.confirmando = true;
  estadoMenuUnico.ultimaInteraccion = millis();
  
  // Implementar acciones específicas según el menú activo
  switch (estadoMenuUnico.indiceSuperior1) {
    case MENU_PRESETS_SURFACE:
    case MENU_PRESETS_SEQUENCER:
      // Cargar preset seleccionado
      if (estadoMenuUnico.indiceInferior1 == 0) {
        // Acción "OK" - cargar preset
        // TODO: Implementar carga de preset
      }
      break;
      
    default:
      // Para otros menús, simplemente confirmar el cambio
      break;
  }
  
  estadoMenuUnico.confirmando = false;
}

void cancelarAccionMenuUnico() {
  estadoMenuUnico.confirmando = false;
  estadoMenuUnico.ultimaInteraccion = millis();
  // Restaurar valores anteriores si es necesario
}

void actualizarTimeoutMenu() {
  if (millis() - estadoMenuUnico.ultimaInteraccion > 5000) {
    // Timeout del menú - volver a pantalla anterior
    // Esta lógica se maneja en pantalla_navegacion.cpp
  }
}

void avanzarFaseMenu() {
  estadoMenuUnico.faseNavegacion++;
  if (estadoMenuUnico.faseNavegacion > 4) {
    estadoMenuUnico.faseNavegacion = 0;
  }
  estadoMenuUnico.ultimaInteraccion = millis();
}

void rotarMenu(int direccion) {
  estadoMenuUnico.ultimaInteraccion = millis();
  
  switch (estadoMenuUnico.zonaActiva) {
    case ZONA_SUPERIOR:
      if (estadoMenuUnico.subzonaActiva == 0) {
        // Rotar menú principal
        estadoMenuUnico.indiceSuperior1 += direccion;
        if (estadoMenuUnico.indiceSuperior1 >= NUM_MENUS_SUPERIOR1) {
          estadoMenuUnico.indiceSuperior1 = NUM_MENUS_SUPERIOR1 - 1;
        }
        if (estadoMenuUnico.indiceSuperior1 < 0) {
          estadoMenuUnico.indiceSuperior1 = 0;
        }
        corregirSubmenuInvalido();
      } else {
        // Rotar submenú
        do {
          estadoMenuUnico.submenuSuperior += direccion;
          if (estadoMenuUnico.submenuSuperior >= NUM_SUBMENUS_SUPERIOR2) {
            estadoMenuUnico.submenuSuperior = NUM_SUBMENUS_SUPERIOR2 - 1;
          }
          if (estadoMenuUnico.submenuSuperior < 0) {
            estadoMenuUnico.submenuSuperior = 0;
          }
        } while (!submenuValidoParaMenu(estadoMenuUnico.indiceSuperior1, estadoMenuUnico.submenuSuperior));
      }
      break;
      
    case ZONA_CENTRAL:
      if (estadoMenuUnico.subzonaActiva == 0) {
        // Rotar pista/grupo
        estadoMenuUnico.indiceCentral1 += direccion;
        if (estadoMenuUnico.indiceCentral1 >= 4) {
          estadoMenuUnico.indiceCentral1 = 3;
        }
        if (estadoMenuUnico.indiceCentral1 < 0) {
          estadoMenuUnico.indiceCentral1 = 0;
        }
        indicePistaActiva = estadoMenuUnico.indiceCentral1;
      } else {
        // Rotar paso/control
        estadoMenuUnico.indiceCentral2 += direccion;
        if (estadoMenuUnico.indiceCentral2 >= 16) {
          estadoMenuUnico.indiceCentral2 = 15;
        }
        if (estadoMenuUnico.indiceCentral2 < 0) {
          estadoMenuUnico.indiceCentral2 = 0;
        }
      }
      break;
      
    case ZONA_INFERIOR:
      // En zona inferior, cambiar valores
      cambiarValorInferior(direccion);
      break;
  }
  
  actualizarTextosMenuUnico();
}

void dibujarMenuConParpadeo() {
  // Esta funcionalidad está integrada en mostrarPantallaUnica()
  // El parpadeo se controla con estadoMenuUnico.parpadeo
}

// ================================
// GESTIÓN DE PRESETS
// ================================

std::vector<String> archivosPresetsSurface;
std::vector<String> archivosPresetsSequencer;

void cargarListaPresetsSurface() {
  archivosPresetsSurface.clear();
  
  // TODO: Implementar lectura de directorio SD
  // Por ahora, agregar presets de ejemplo
  archivosPresetsSurface.push_back("SERUM_SP1_SURFACE_PRESET01.csv");
  archivosPresetsSurface.push_back("SERUM_SP1_SURFACE_PRESET02.csv");
  archivosPresetsSurface.push_back("SERUM_SP1_SURFACE_PRESET03.csv");
}

void cargarListaPresetsSequencer() {
  archivosPresetsSequencer.clear();
  
  // TODO: Implementar lectura de directorio SD
  // Por ahora, agregar presets de ejemplo
  archivosPresetsSequencer.push_back("SERUM_SP1_SEQUENCER_PRESET01.csv");
  archivosPresetsSequencer.push_back("SERUM_SP1_SEQUENCER_PRESET02.csv");
  archivosPresetsSequencer.push_back("SERUM_SP1_SEQUENCER_PRESET03.csv");
}

void corregirSubmenuInvalido() {
  if (!submenuValidoParaMenu(estadoMenuUnico.indiceSuperior1, estadoMenuUnico.submenuSuperior)) {
    switch (estadoMenuUnico.indiceSuperior1) {
      case MENU_PRESETS_SURFACE:
      case MENU_PRESETS_SEQUENCER:
        estadoMenuUnico.submenuSuperior = SUBMENU_PRESETS;
        break;
      case MENU_CONFIG_KEYBOARD:
      case MENU_CONFIG_SEQUENCER:
      case MENU_CONFIG_SURFACE:
        estadoMenuUnico.submenuSuperior = SUBMENU_CONFIG_SURFACE_SEQUENCER;
        break;
      case MENU_SEQUENCER:
        estadoMenuUnico.submenuSuperior = SUBMENU_TRIGGER;
        break;
      default:
        estadoMenuUnico.submenuSuperior = SUBMENU_NONE;
        break;
    }
  }
}

void actualizarTextosMenuUnico() {
  estadoMenuUnico.nombreSuperior1 = obtenerNombreGrupo(estadoMenuUnico.indiceSuperior1);
  switch (estadoMenuUnico.submenuSuperior) {
    case SUBMENU_PRESETS:
      estadoMenuUnico.nombreSuperior2 = "PRESETS";
      break;
    case SUBMENU_CONFIG_SURFACE_SEQUENCER:
    case SUBMENU_CONFIG_SURFACE_MIDI:
    case SUBMENU_CONFIG_KEYBOARD_OUTPUT:
    case SUBMENU_CONFIG_KEYBOARD_LINK:
    case SUBMENU_CONFIG_SEQUENCER_OUTPUT:
      estadoMenuUnico.nombreSuperior2 = "CONFIG";
      break;
    case SUBMENU_TRIGGER:
      estadoMenuUnico.nombreSuperior2 = "STEP TRIGGER";
      break;
    case SUBMENU_NOTE:
      estadoMenuUnico.nombreSuperior2 = "STEP NOTE";
      break;
    case SUBMENU_VELOCITY:
      estadoMenuUnico.nombreSuperior2 = "STEP VELOCITY";
      break;
    case SUBMENU_LONG:
      estadoMenuUnico.nombreSuperior2 = "STEP LONG";
      break;
    case SUBMENU_TOTAL_STEPS:
      estadoMenuUnico.nombreSuperior2 = "TOTAL STEPS";
      break;
    case SUBMENU_MAX_STEPS:
      estadoMenuUnico.nombreSuperior2 = "MAX STEPS";
      break;
    case SUBMENU_MODE:
      estadoMenuUnico.nombreSuperior2 = "MODE";
      break;
    case SUBMENU_CURVE:
      estadoMenuUnico.nombreSuperior2 = "CURVE";
      break;
    case SUBMENU_LEGATO:
      estadoMenuUnico.nombreSuperior2 = "LEGATO";
      break;
    case SUBMENU_BPM:
      estadoMenuUnico.nombreSuperior2 = "BPM";
      break;
    case SUBMENU_BPM_SYNC:
      estadoMenuUnico.nombreSuperior2 = "BPM SYNC";
      break;
    default:
      estadoMenuUnico.nombreSuperior2 = "";
  }

  estadoMenuUnico.nombreCentral1 = obtenerNombreSubgrupo(estadoMenuUnico.indiceCentral1);
  estadoMenuUnico.nombreCentral2 = obtenerNombreControl(estadoMenuUnico.indiceCentral1, estadoMenuUnico.indiceCentral2);

  switch (estadoMenuUnico.indiceSuperior1) {
    case MENU_SEQUENCER:
      switch (estadoMenuUnico.submenuSuperior) {
        case SUBMENU_TRIGGER: {
          Step paso = obtenerPaso(estadoMenuUnico.indiceCentral2);
          estadoMenuUnico.valorInferiorSeleccionado = paso.active ? "ON" : "OFF";
          break;
        }
        case SUBMENU_NOTE: {
          Step paso = obtenerPaso(estadoMenuUnico.indiceCentral2);
          estadoMenuUnico.valorInferiorSeleccionado = notaToNombre(paso.note);
          break;
        }
        case SUBMENU_VELOCITY: {
          Step paso = obtenerPaso(estadoMenuUnico.indiceCentral2);
          estadoMenuUnico.valorInferiorSeleccionado = String(paso.velocity) + " %";
          break;
        }
        case SUBMENU_LONG: {
          Step paso = obtenerPaso(estadoMenuUnico.indiceCentral2);
          estadoMenuUnico.valorInferiorSeleccionado = String(paso.duration) + " TICKS";
          break;
        }
        case SUBMENU_TOTAL_STEPS:
          estadoMenuUnico.valorInferiorSeleccionado = String(obtenerTotalSteps());
          break;
        case SUBMENU_MAX_STEPS:
          estadoMenuUnico.valorInferiorSeleccionado = String(obtenerMaxSteps());
          break;
        case SUBMENU_MODE:
          estadoMenuUnico.valorInferiorSeleccionado = modoMono ? "MONO" : "POLY";
          break;
        case SUBMENU_LEGATO:
          estadoMenuUnico.valorInferiorSeleccionado = String(porcentajeLegato) + " %";
          break;
        case SUBMENU_CURVE:
          estadoMenuUnico.valorInferiorSeleccionado = String(porcentajeSustain) + " %";
          break;
        case SUBMENU_BPM:
          estadoMenuUnico.valorInferiorSeleccionado = String(estadoTempo);
          break;
        case SUBMENU_BPM_SYNC:
          estadoMenuUnico.valorInferiorSeleccionado = bpmSyncEnabled ? "ON" : "OFF";
          break;
        default:
          estadoMenuUnico.valorInferiorSeleccionado = "";
      }
      break;

    case MENU_PRESETS_SURFACE:
    case MENU_PRESETS_SEQUENCER:
      if (estadoMenuUnico.indiceInferior1 == 0) {
        estadoMenuUnico.valorInferiorSeleccionado = "OK";
      } else {
        estadoMenuUnico.valorInferiorSeleccionado = "CANCEL";
      }
      break;

    case MENU_CONFIG_KEYBOARD:
    case MENU_CONFIG_SEQUENCER:
    case MENU_CONFIG_SURFACE:
      switch (estadoMenuUnico.submenuSuperior) {
        case SUBMENU_CONFIG_KEYBOARD_OUTPUT:
          estadoMenuUnico.valorInferiorSeleccionado = tecladoActivo ? "ON" : "OFF";
          break;
        case SUBMENU_CONFIG_KEYBOARD_LINK:
          estadoMenuUnico.valorInferiorSeleccionado = secuenciaTecladoLinkeada ? "ON" : "OFF";
          break;
        case SUBMENU_CONFIG_SEQUENCER_OUTPUT:
          estadoMenuUnico.valorInferiorSeleccionado = !muteSequencerNotes ? "ON" : "OFF";
          break;
        case SUBMENU_CONFIG_SURFACE_SEQUENCER:
          estadoMenuUnico.valorInferiorSeleccionado = secuenciadorZonaActiva ? "ON" : "OFF";
          break;
        case SUBMENU_CONFIG_SURFACE_MIDI:
          estadoMenuUnico.valorInferiorSeleccionado = superficieActiva ? "ON" : "OFF";
          break;
        default:
          estadoMenuUnico.valorInferiorSeleccionado = "";
      }
      break;

    default:
      estadoMenuUnico.valorInferiorSeleccionado = "";
  }
}

void cambiarValorInferior(int delta) {
  switch (estadoMenuUnico.indiceSuperior1) {
    case MENU_SEQUENCER:
      switch (estadoMenuUnico.submenuSuperior) {
        case SUBMENU_TRIGGER: {
          Step& paso = secuencia[indicePistaActiva][estadoMenuUnico.indiceCentral2];
          paso.active = !paso.active;
          break;
        }
        case SUBMENU_NOTE: {
          Step& paso = secuencia[indicePistaActiva][estadoMenuUnico.indiceCentral2];
          int nuevaNota = paso.note + delta;
          if (nuevaNota >= 0 && nuevaNota <= 127) paso.note = nuevaNota;
          break;
        }
        case SUBMENU_VELOCITY: {
          Step& paso = secuencia[indicePistaActiva][estadoMenuUnico.indiceCentral2];
          int nuevaVel = paso.velocity + (delta * 8);
          paso.velocity = constrain(nuevaVel, 0, 127);
          break;
        }
        case SUBMENU_LONG: {
          Step& paso = secuencia[indicePistaActiva][estadoMenuUnico.indiceCentral2];
          int nuevaDuracion = paso.duration + delta;
          paso.duration = constrain(nuevaDuracion, 1, 32);
          break;
        }
        case SUBMENU_TOTAL_STEPS:
          establecerTotalSteps(constrain(obtenerTotalSteps() + delta, 1, obtenerMaxSteps()));
          break;
        case SUBMENU_MAX_STEPS: {
          uint8_t nuevoMax = obtenerMaxSteps() + (delta * 8);
          nuevoMax = constrain(nuevoMax, 8, MAX_STEPS_CONFIGURABLE);
          establecerMaxSteps(nuevoMax);
          break;
        }
        case SUBMENU_MODE:
          modoMono = !modoMono;
          break;
        case SUBMENU_LEGATO:
          porcentajeLegato = constrain(porcentajeLegato + delta * 10, 0, 100);
          break;
        case SUBMENU_CURVE:
          porcentajeSustain = constrain(porcentajeSustain + delta * 10, 0, 200);
          break;
        case SUBMENU_BPM:
          estadoTempo = constrain(estadoTempo + delta, 30, 300);
          break;
        case SUBMENU_BPM_SYNC:
          bpmSyncEnabled = !bpmSyncEnabled;
          break;
      }
      break;

    case MENU_CONFIG_KEYBOARD:
    case MENU_CONFIG_SEQUENCER:
    case MENU_CONFIG_SURFACE:
      switch (estadoMenuUnico.submenuSuperior) {
        case SUBMENU_CONFIG_KEYBOARD_OUTPUT:
          tecladoActivo = !tecladoActivo;
          break;
        case SUBMENU_CONFIG_KEYBOARD_LINK:
          secuenciaTecladoLinkeada = !secuenciaTecladoLinkeada;
          break;
        case SUBMENU_CONFIG_SEQUENCER_OUTPUT:
          muteSequencerNotes = !muteSequencerNotes;
          break;
        case SUBMENU_CONFIG_SURFACE_SEQUENCER:
          secuenciadorZonaActiva = !secuenciadorZonaActiva;
          break;
        case SUBMENU_CONFIG_SURFACE_MIDI:
          superficieActiva = !superficieActiva;
          break;
        default:
          break;
      }
      break;
  }
}
