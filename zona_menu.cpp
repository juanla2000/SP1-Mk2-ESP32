//zona_menu.cpp

#include "zona_menu.h"
#include "menu_unico.h"
#include "controles.h" 

bool submenuValidoParaMenu(uint8_t menu, uint8_t submenu) {
    switch (menu) {
        case MENU_PRESETS_SURFACE:
        case MENU_PRESETS_SEQUENCER:
            return submenu == SUBMENU_PRESETS;
            
        case MENU_SEQUENCER:
            return (submenu >= SUBMENU_TRIGGER && submenu <= SUBMENU_LEGATO) ||
                   submenu == SUBMENU_BPM || submenu == SUBMENU_BPM_SYNC;
                   
        case MENU_CONFIG_KEYBOARD:
        case MENU_CONFIG_SEQUENCER:
        case MENU_CONFIG_SURFACE:
            return submenu >= SUBMENU_CONFIG_SURFACE_SEQUENCER && 
                   submenu <= SUBMENU_CONFIG_SEQUENCER_OUTPUT;
                   
        default:
            return false;
    }
}