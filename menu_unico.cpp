//menu_unico.cpp

#include "menu_unico.h"
#include "pantalla_unica.h"
#include "secuenciador.h"
#include "configuracion.h"
#include "controles.h"
#include "zona_menu.h"

EstadoMenuUnico estadoMenuUnico;

// ============================================================
// CORRECCIÓN: Declaraciones extern para variables compartidas
// ============================================================
extern bool modoMono;
extern bool bpmSyncEnabled;
extern bool secuenciaTecladoLinkeada;
extern bool tecladoActivo;
extern bool secuenciadorZonaActiva;
extern bool superficieActiva;
extern bool muteSequencerNotes;
extern uint16_t estadoTempo;
extern uint8_t porcentajeLegato;
extern uint8_t porcentajeSustain;
extern uint8_t indicePistaActiva;
extern Step secuencia[NUM_SEQUENCERS][MAX_STEPS_CONFIGURABLE];

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
