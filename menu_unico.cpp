//menu_unico.cpp

#include "menu_unico.h"
#include "pantalla_unica.h"
#include "controles.h"
#include "secuenciador.h"
#include "configuracion.h"
#include <SD.h>

EstadoMenuUnico estadoMenuUnico;

// ===========================================
// AUXILIARES PARA LIMITAR SUBMENUS VÁLIDOS
// ===========================================

bool submenuValidoParaMenu(uint8_t menu, uint8_t submenu) {
  switch (menu) {
    case MENU_MIDI_KEYBOARD:
      return submenu == SUBMENU_NOTE;
    case MENU_MIDI_SURFACE:
      return submenu == SUBMENU_CC;
    case MENU_SEQUENCER:
      return submenu >= SUBMENU_TRIGGER && submenu <= SUBMENU_LONG;
    case MENU_PRESETS_SURFACE:
    case MENU_PRESETS_SEQUENCER:
      return submenu == SUBMENU_PRESETS;
    case MENU_CONFIG_KEYBOARD:
      return submenu == SUBMENU_ROUTE;
    case MENU_CONFIG_SEQUENCER:
      return submenu == SUBMENU_ACTIVE;
    case MENU_CONFIG_SURFACE:
      return submenu == SUBMENU_SEQUENCER;
    default:
      return false;
  }
}

void corregirSubmenuInvalido() {
  if (!submenuValidoParaMenu(estadoMenuUnico.indiceSuperior1, estadoMenuUnico.indiceSuperior2)) {
    // Resetear al primer válido si no es correcto
    switch (estadoMenuUnico.indiceSuperior1) {
      case MENU_MIDI_KEYBOARD:     estadoMenuUnico.indiceSuperior2 = SUBMENU_NOTE; break;
      case MENU_MIDI_SURFACE:      estadoMenuUnico.indiceSuperior2 = SUBMENU_CC; break;
      case MENU_SEQUENCER:         estadoMenuUnico.indiceSuperior2 = SUBMENU_TRIGGER; break;
      case MENU_PRESETS_SURFACE:
      case MENU_PRESETS_SEQUENCER: estadoMenuUnico.indiceSuperior2 = SUBMENU_PRESETS; break;
      case MENU_CONFIG_KEYBOARD:   estadoMenuUnico.indiceSuperior2 = SUBMENU_ROUTE; break;
      case MENU_CONFIG_SEQUENCER:  estadoMenuUnico.indiceSuperior2 = SUBMENU_ACTIVE; break;
      case MENU_CONFIG_SURFACE:    estadoMenuUnico.indiceSuperior2 = SUBMENU_SEQUENCER; break;
    }
  }
}

void actualizarTextosMenuUnico() {
  corregirSubmenuInvalido();

  // Superior 1
  switch (estadoMenuUnico.indiceSuperior1) {
    case MENU_MIDI_KEYBOARD:       estadoMenuUnico.nombreSuperior1 = "MIDI - KEYBOARD"; break;
    case MENU_MIDI_SURFACE:        estadoMenuUnico.nombreSuperior1 = "MIDI - SURFACE"; break;
    case MENU_SEQUENCER:           estadoMenuUnico.nombreSuperior1 = "SEQUENCER"; break;
    case MENU_PRESETS_SURFACE:     estadoMenuUnico.nombreSuperior1 = "PRESETS - SURFACE"; break;
    case MENU_PRESETS_SEQUENCER:   estadoMenuUnico.nombreSuperior1 = "PRESETS - SEQUENCER"; break;
    case MENU_CONFIG_KEYBOARD:     estadoMenuUnico.nombreSuperior1 = "CONFIG - KEYBOARD"; break;
    case MENU_CONFIG_SEQUENCER:    estadoMenuUnico.nombreSuperior1 = "CONFIG - SEQUENCER"; break;
    case MENU_CONFIG_SURFACE:      estadoMenuUnico.nombreSuperior1 = "CONFIG - SURFACE"; break;
    default:                       estadoMenuUnico.nombreSuperior1 = "--"; break;
  }

  // Superior 2
  switch (estadoMenuUnico.indiceSuperior2) {
    case SUBMENU_NOTE:         estadoMenuUnico.nombreSuperior2 = "NOTE"; break;
    case SUBMENU_CC:           estadoMenuUnico.nombreSuperior2 = "CC"; break;
    case SUBMENU_TRIGGER:      estadoMenuUnico.nombreSuperior2 = "TRIGGER"; break;
    case SUBMENU_PRESETS:      estadoMenuUnico.nombreSuperior2 = "PRESETS"; break;
    case SUBMENU_ROUTE:        estadoMenuUnico.nombreSuperior2 = "ROUTE"; break;
    case SUBMENU_ACTIVE:       estadoMenuUnico.nombreSuperior2 = "ACTIVE"; break;
    case SUBMENU_SEQUENCER:    estadoMenuUnico.nombreSuperior2 = "SEQUENCER"; break;
    case SUBMENU_ZONE:         estadoMenuUnico.nombreSuperior2 = "ZONE"; break;
    default:                   estadoMenuUnico.nombreSuperior2 = "--"; break;
  }

  // Central 1
  switch (estadoMenuUnico.indiceSuperior1) {
    case MENU_SEQUENCER:
      estadoMenuUnico.nombreCentral1 = "STEP GROUP";
      break;
    case MENU_PRESETS_SURFACE:
    case MENU_PRESETS_SEQUENCER:
      estadoMenuUnico.nombreCentral1 = "LOAD/SAVE/DEL";
      break;
    case MENU_CONFIG_KEYBOARD:
    case MENU_CONFIG_SEQUENCER:
      estadoMenuUnico.nombreCentral1 = "NOTE";
      break;
    case MENU_CONFIG_SURFACE:
      estadoMenuUnico.nombreCentral1 = "ZONE";
      break;
    case MENU_MIDI_SURFACE:
      estadoMenuUnico.nombreCentral1 = "CC INFO";
      break;
    default:
      estadoMenuUnico.nombreCentral1 = "-";
      break;
  }

  // Central 2
  switch (estadoMenuUnico.indiceSuperior1) {
    case MENU_SEQUENCER:
      estadoMenuUnico.nombreCentral2 = "STEP";
      break;
    case MENU_PRESETS_SURFACE:
    case MENU_PRESETS_SEQUENCER:
      estadoMenuUnico.nombreCentral2 = "NAME";
      break;
    case MENU_CONFIG_KEYBOARD:
      estadoMenuUnico.nombreCentral2 = "SEQUENCER";
      break;
    case MENU_CONFIG_SEQUENCER:
      estadoMenuUnico.nombreCentral2 = "OUTPUT";
      break;
    case MENU_CONFIG_SURFACE:
      estadoMenuUnico.nombreCentral2 = "ACTIVE";
      break;
    case MENU_MIDI_SURFACE:
      estadoMenuUnico.nombreCentral2 = "GROUP/NAME";
      break;
    default:
      estadoMenuUnico.nombreCentral2 = "-";
      break;
  }

  // Inferior
  switch (estadoMenuUnico.indiceSuperior1) {
    case MENU_SEQUENCER:
      switch (estadoMenuUnico.indiceSuperior2) {
        case SUBMENU_TRIGGER:      estadoMenuUnico.valorInferiorSeleccionado = secuenciadorActivo[estadoMenuUnico.indiceCentral2] ? "<ON>" : "<OFF>"; break;
        case SUBMENU_NOTE:         estadoMenuUnico.valorInferiorSeleccionado = notaToNombre(secuenciadorNotas[estadoMenuUnico.indiceCentral2]); break;
        case SUBMENU_ACTIVE:       estadoMenuUnico.valorInferiorSeleccionado = secuenciadorActivo[estadoMenuUnico.indiceCentral2] ? "<ON>" : "<OFF>"; break;
        case SUBMENU_ZONE:         estadoMenuUnico.valorInferiorSeleccionado = secuenciadorZona[estadoMenuUnico.indiceCentral2] ? "<ON>" : "<OFF>"; break;
        case SUBMENU_SEQUENCER:    estadoMenuUnico.valorInferiorSeleccionado = secuenciadorGlobalActivo ? "<ON>" : "<OFF>"; break;
        case SUBMENU_PRESETS:      estadoMenuUnico.valorInferiorSeleccionado = "<OK/CANCEL>"; break;
        case SUBMENU_ROUTE:        estadoMenuUnico.valorInferiorSeleccionado = secuenciadorRutaSolo ? "<SOLO>" : secuenciadorRutaActiva ? "<ON>" : "<OFF>"; break;
        default:                   estadoMenuUnico.valorInferiorSeleccionado = "-"; break;
      }
      break;
    case MENU_PRESETS_SURFACE:
    case MENU_PRESETS_SEQUENCER:
      estadoMenuUnico.valorInferiorSeleccionado = "<OK/CANCEL>";
      break;
    case MENU_CONFIG_KEYBOARD:
      estadoMenuUnico.valorInferiorSeleccionado = tecladoSecuenciaEnabled ? "<ON>" : "<OFF>";
      break;
    case MENU_CONFIG_SEQUENCER:
    case MENU_CONFIG_SURFACE:
      estadoMenuUnico.valorInferiorSeleccionado = secuenciadorGlobalActivo ? "<ON>" : "<OFF>";
      break;
    case MENU_MIDI_KEYBOARD:
    case MENU_MIDI_SURFACE:
      estadoMenuUnico.valorInferiorSeleccionado = "<VALOR>";
      break;
    default:
      estadoMenuUnico.valorInferiorSeleccionado = "-";
      break;
  }
}

void inicializarMenuUnico() {
  estadoMenuUnico.zonaActiva = ZONA_SUPERIOR;
  estadoMenuUnico.subzonaActiva = 0;
  estadoMenuUnico.indiceSuperior1 = MENU_SEQUENCER;
  estadoMenuUnico.indiceSuperior2 = SUBMENU_TRIGGER;
  estadoMenuUnico.indiceCentral1 = 0;
  estadoMenuUnico.indiceCentral2 = 0;
  estadoMenuUnico.confirmando = false;
  estadoMenuUnico.parpadeo = false;
  estadoMenuUnico.colorFondo = 0x0000;
  actualizarTextosMenuUnico();
}

void avanzarZonaMenuUnico() {
  if (estadoMenuUnico.zonaActiva == ZONA_SUPERIOR && estadoMenuUnico.subzonaActiva == 0) {
    estadoMenuUnico.subzonaActiva = 1;
  } else if (estadoMenuUnico.zonaActiva == ZONA_SUPERIOR && estadoMenuUnico.subzonaActiva == 1) {
    estadoMenuUnico.zonaActiva = ZONA_CENTRAL;
    estadoMenuUnico.subzonaActiva = 0;
  } else if (estadoMenuUnico.zonaActiva == ZONA_CENTRAL && estadoMenuUnico.subzonaActiva == 0) {
    estadoMenuUnico.subzonaActiva = 1;
  } else if (estadoMenuUnico.zonaActiva == ZONA_CENTRAL && estadoMenuUnico.subzonaActiva == 1) {
    estadoMenuUnico.zonaActiva = ZONA_INFERIOR;
    estadoMenuUnico.subzonaActiva = 0;
  } else {
    estadoMenuUnico.zonaActiva = ZONA_SUPERIOR;
    estadoMenuUnico.subzonaActiva = 0;
  }
  actualizarTextosMenuUnico();
}

void clickCortoMenuUnico() {
  avanzarZonaMenuUnico();
}

void confirmarAccionMenuUnico() {
  estadoMenuUnico.confirmando = true;
}

void cancelarAccionMenuUnico() {
  estadoMenuUnico.confirmando = false;
}
