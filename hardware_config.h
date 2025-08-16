// hardware_config.h

#pragma once

#include <Arduino.h>

// ----------------------------
// Pines de pantalla OLED
// ----------------------------
#define OLED_CS     5
#define OLED_DC     15
#define OLED_RST    16

// ----------------------------
// Pines SPI
// ----------------------------
#define PIN_SPI_MOSI 46    // MOSI compartido para OLED y SD
#define PIN_SPI_MISO 14    // MISO para tarjeta SD
#define PIN_SPI_SCK  12    // SCK compartido para OLED y SD

// Actualizar definición de pines SPI
#define SPI_MOSI PIN_SPI_MOSI
#define SPI_MISO PIN_SPI_MISO
#define SPI_SCK  PIN_SPI_SCK

// ----------------------------
// Tamaño de pantalla
// ----------------------------
#define SCREEN_WIDTH  128
#define SCREEN_HEIGHT 128

// ----------------------------
// Pin para la tarjeta SD
// ----------------------------
#define SD_CS       8

// ----------------------------
// UART MIDI
// ----------------------------
#define MIDI_UART_TX  43
#define MIDI_UART_RX  44
#define MIDI_BAUDRATE 31250

// ----------------------------
// Direcciones EEPROM
// ----------------------------
#define EEPROM_MIDI_THRU_ADDR     0
#define EEPROM_SEQ_LINK_ADDR      1
#define EEPROM_KEYBOARD_ADDR      2

// ----------------------------
// Rotary Encoder (uso exclusivo) - CONFIGURACIÓN ACTUALIZADA
// ----------------------------
#define ENCODER_CLK_PIN     9    // Pin CLK del encoder (anteriormente ENCODER_A_PIN)
#define ENCODER_DT_PIN      10   // Pin DT del encoder (anteriormente ENCODER_B_PIN)
#define ENCODER_BUTTON_PIN  13   // Pin del botón del encoder

// ----------------------------
// Variables globales compartidas
// ----------------------------
extern uint8_t pantallaActiva;
extern bool estadoParpadeo;
extern unsigned long tiempoUltimoParpadeo;
extern bool midiThruEnabled;
extern bool tecladoSecuenciaEnabled;
extern unsigned long lastStepTime;