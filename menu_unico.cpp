// menu_unico.cpp

#include "menu_unico.h"
#include "pantalla_unica.h"
#include "secuenciador.h"
#include "configuracion.h"
#include "controles.h"
#include "zona_menu.h"
#include <vector>
#include <SD.h>
#include <Adafruit_SSD1351.h>

extern Adafruit_SSD1351 display;
EstadoMenuUnico estadoMenuUnico;


// ============================================================
// INICIALIZACIÓN DEL MENÚ
// ============================================================
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
    estadoMenuUnico.parpadeo = true;
    estadoMenuUnico.faseNavegacion = 0;
    estadoMenuUnico.colorFondo = 0x0000;
    estadoMenuUnico.ultimaInteraccion = millis();
    
    corregirSubmenuInvalido();
    actualizarTextosMenuUnico();
    
    cargarListaPresetsSurface();
    cargarListaPresetsSequencer();
}

// ============================================================
// CLIC CORTO (NAVEGACIÓN SECUENCIAL)
// ============================================================
void clickCortoMenuUnico() {
    estadoMenuUnico.ultimaInteraccion = millis();
    
    // Avanzar secuencialmente por las líneas
    if (estadoMenuUnico.zonaActiva == ZONA_SUPERIOR) {
        if (estadoMenuUnico.subzonaActiva == 0) {
            estadoMenuUnico.subzonaActiva = 1;  // Segunda línea zona superior
        } else {
            estadoMenuUnico.zonaActiva = ZONA_CENTRAL;
            estadoMenuUnico.subzonaActiva = 0;  // Primera línea zona central
        }
    } 
    else if (estadoMenuUnico.zonaActiva == ZONA_CENTRAL) {
        if (estadoMenuUnico.subzonaActiva == 0) {
            estadoMenuUnico.subzonaActiva = 1;  // Segunda línea zona central
        } else {
            estadoMenuUnico.zonaActiva = ZONA_INFERIOR;  // Última línea
        }
    }
    else if (estadoMenuUnico.zonaActiva == ZONA_INFERIOR) {
        // Salir del menú (manejo externo)
        estadoMenuUnico.faseNavegacion = 0;
        return;
    }
    
    estadoMenuUnico.parpadeo = true;
    actualizarTextosMenuUnico();
}

// ============================================================
// ROTAR MENÚ (MODIFICAR VALORES)
// ============================================================
void rotarMenu(int direccion) {
    estadoMenuUnico.ultimaInteraccion = millis();
    
    if (estadoMenuUnico.zonaActiva == ZONA_SUPERIOR) {
        if (estadoMenuUnico.subzonaActiva == 0) {
            estadoMenuUnico.indiceSuperior1 += direccion;
            if (estadoMenuUnico.indiceSuperior1 >= NUM_MENUS_SUPERIOR1) estadoMenuUnico.indiceSuperior1 = 0;
            if (estadoMenuUnico.indiceSuperior1 < 0) estadoMenuUnico.indiceSuperior1 = NUM_MENUS_SUPERIOR1 - 1;
            corregirSubmenuInvalido();
        } else {
            do {
                estadoMenuUnico.submenuSuperior += direccion;
                if (estadoMenuUnico.submenuSuperior >= NUM_SUBMENUS_SUPERIOR2) estadoMenuUnico.submenuSuperior = 0;
                if (estadoMenuUnico.submenuSuperior < 0) estadoMenuUnico.submenuSuperior = NUM_SUBMENUS_SUPERIOR2 - 1;
            } while (!submenuValidoParaMenu(estadoMenuUnico.indiceSuperior1, estadoMenuUnico.submenuSuperior));
        }
    } 
    else if (estadoMenuUnico.zonaActiva == ZONA_CENTRAL) {
        if (estadoMenuUnico.subzonaActiva == 0) {
            estadoMenuUnico.indiceCentral1 += direccion;
            if (estadoMenuUnico.indiceCentral1 >= 4) estadoMenuUnico.indiceCentral1 = 0;
            if (estadoMenuUnico.indiceCentral1 < 0) estadoMenuUnico.indiceCentral1 = 3;
            indicePistaActiva = estadoMenuUnico.indiceCentral1;
        } else {
            estadoMenuUnico.indiceCentral2 += direccion;
            if (estadoMenuUnico.indiceCentral2 >= 16) estadoMenuUnico.indiceCentral2 = 0;
            if (estadoMenuUnico.indiceCentral2 < 0) estadoMenuUnico.indiceCentral2 = 15;
        }
    }
    else if (estadoMenuUnico.zonaActiva == ZONA_INFERIOR) {
        cambiarValorInferior(direccion);
    }
    
    actualizarTextosMenuUnico();
}

// ============================================================
// ACTUALIZAR TEXTOS DEL MENÚ
// ============================================================
void actualizarTextosMenuUnico() {
    const char* nombresGrupos[] = {
        "MIDI KEYBOARD", "MIDI SURFACE", "SEQUENCER", "PRESETS SURFACE", 
        "PRESETS SEQUENCER", "CONFIG KEYBOARD", "CONFIG SEQUENCER", "CONFIG SURFACE"
    };
    
    // Actualizar nombres según el menú seleccionado
    if (estadoMenuUnico.indiceSuperior1 < NUM_MENUS_SUPERIOR1) {
        estadoMenuUnico.nombreSuperior1 = nombresGrupos[estadoMenuUnico.indiceSuperior1];
    } else {
        estadoMenuUnico.nombreSuperior1 = "UNKNOWN";
    }
    
    // Actualizar submenús
    switch (estadoMenuUnico.submenuSuperior) {
        case SUBMENU_PRESETS: estadoMenuUnico.nombreSuperior2 = "PRESETS"; break;
        case SUBMENU_CONFIG_SURFACE_SEQUENCER: estadoMenuUnico.nombreSuperior2 = "SEQUENCER SURFACE"; break;
        case SUBMENU_CONFIG_SURFACE_MIDI: estadoMenuUnico.nombreSuperior2 = "MIDI SURFACE"; break;
        case SUBMENU_CONFIG_KEYBOARD_OUTPUT: estadoMenuUnico.nombreSuperior2 = "KEYBOARD OUTPUT"; break;
        case SUBMENU_CONFIG_KEYBOARD_LINK: estadoMenuUnico.nombreSuperior2 = "KEYBOARD LINK"; break;
        case SUBMENU_CONFIG_SEQUENCER_OUTPUT: estadoMenuUnico.nombreSuperior2 = "SEQUENCER OUTPUT"; break;
        case SUBMENU_TRIGGER: estadoMenuUnico.nombreSuperior2 = "STEP TRIGGER"; break;
        case SUBMENU_NOTE: estadoMenuUnico.nombreSuperior2 = "STEP NOTE"; break;
        case SUBMENU_VELOCITY: estadoMenuUnico.nombreSuperior2 = "STEP VELOCITY"; break;
        case SUBMENU_LONG: estadoMenuUnico.nombreSuperior2 = "STEP LONG"; break;
        case SUBMENU_TOTAL_STEPS: estadoMenuUnico.nombreSuperior2 = "TOTAL STEPS"; break;
        case SUBMENU_MAX_STEPS: estadoMenuUnico.nombreSuperior2 = "MAX STEPS"; break;
        case SUBMENU_MODE: estadoMenuUnico.nombreSuperior2 = "MODE"; break;
        case SUBMENU_LEGATO: estadoMenuUnico.nombreSuperior2 = "LEGATO"; break;
        case SUBMENU_BPM: estadoMenuUnico.nombreSuperior2 = "BPM"; break;
        case SUBMENU_BPM_SYNC: estadoMenuUnico.nombreSuperior2 = "BPM SYNC"; break;
        default: estadoMenuUnico.nombreSuperior2 = "";
    }
    
    // Manejo de nombres centrales
    estadoMenuUnico.nombreCentral1 = obtenerNombreSubgrupo(estadoMenuUnico.indiceCentral1);
    
    // Manejo especial para menús de presets
    if (estadoMenuUnico.indiceSuperior1 == MENU_PRESETS_SURFACE || 
        estadoMenuUnico.indiceSuperior1 == MENU_PRESETS_SEQUENCER) {
        
        std::vector<String>& lista = (estadoMenuUnico.indiceSuperior1 == MENU_PRESETS_SURFACE) ? 
                                    archivosPresetsSurface : archivosPresetsSequencer;
        
        if (estadoMenuUnico.indiceCentral2 < lista.size()) {
            estadoMenuUnico.nombreCentral2 = lista[estadoMenuUnico.indiceCentral2];
        } else {
            estadoMenuUnico.nombreCentral2 = "NO PRESETS";
        }
        
        estadoMenuUnico.valorInferiorSeleccionado = 
            (estadoMenuUnico.submenuSuperior == SUBMENU_PRESETS) ? "LOAD" : "SAVE";
    } 
    else {
        // Manejo estándar para otros menús
        estadoMenuUnico.nombreCentral2 = obtenerNombreControl(estadoMenuUnico.indiceCentral1, estadoMenuUnico.indiceCentral2);
        
        // Actualizar valor inferior según menú actual
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
}

// ============================================================
// GESTIÓN DE PRESETS
// ============================================================
std::vector<String> archivosPresetsSurface;
std::vector<String> archivosPresetsSequencer;

void cargarListaPresetsSurface() {
    archivosPresetsSurface.clear();
    archivosPresetsSurface.push_back("SERUM_SP1_SURFACE_PRESET01.csv");
    archivosPresetsSurface.push_back("SERUM_SP1_SURFACE_PRESET02.csv");
    archivosPresetsSurface.push_back("SERUM_SP1_SURFACE_PRESET03.csv");
}

void cargarListaPresetsSequencer() {
    archivosPresetsSequencer.clear();
    archivosPresetsSequencer.push_back("SERUM_SP1_SEQUENCER_PRESET01.csv");
    archivosPresetsSequencer.push_back("SERUM_SP1_SEQUENCER_PRESET02.csv");
    archivosPresetsSequencer.push_back("SERUM_SP1_SEQUENCER_PRESET03.csv");
}

// ============================================================
// CORRECCIÓN DE SUBMENÚS
// ============================================================
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
        }
    }
}

// ============================================================
// CAMBIO DE VALORES
// ============================================================
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

// ============================================================
// GESTIÓN DE TIMEOUT
// ============================================================
void actualizarTimeoutMenu() {
    if (millis() - estadoMenuUnico.ultimaInteraccion > 5000) {
        estadoMenuUnico.faseNavegacion = 0;
        estadoMenuUnico.zonaActiva = ZONA_SUPERIOR;
        estadoMenuUnico.subzonaActiva = 0;
    }
}

// ============================================================
// AVANCE DE FASE (OBSOLETO - MANTENIDO POR COMPATIBILIDAD)
// ============================================================
void avanzarFaseMenu() {
    estadoMenuUnico.faseNavegacion++;
    if (estadoMenuUnico.faseNavegacion > 4) {
        estadoMenuUnico.faseNavegacion = 0;
    }
    estadoMenuUnico.ultimaInteraccion = millis();
}

// ============================================================
// DIBUJAR MENÚ CON PARPADEO (OBSOLETO - MANTENIDO POR COMPATIBILIDAD)
// ============================================================
void dibujarMenuConParpadeo() {
    // Función vacía - ahora se maneja en pantalla_unica.cpp
}