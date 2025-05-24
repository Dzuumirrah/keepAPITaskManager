// This file contains the pinout definitions for the main board.
// Please connect the pinout definitions to the correct pins on your board.

#ifndef PINOUT_MAIN_H
#define PINOUT_MAIN_H

#include <Wire.h>
#include <Arduino.h>

// Pins for Buttons
constexpr int BUTTON_YES    = 0; 
constexpr int BUTTON_NO     = 1; 
constexpr int BUTTON_UP     = 2; 
constexpr int BUTTON_DOWN   = 3; 

// // See Setup204_ESP32_TouchDown.h for the TFT pinout.
// The declaration of the TFT pins is done in the TFT_eSPI library.
// This comment is to remind you to check the TFT pinout in the library.
// #define TFT_CS           15
// #define TFT_RST          4
// #define TFT_DC           2
// #define TFT_MOSI         23
// #define TFT_SCLK         18
// #define TFT_BL           32
// #define TFT_MISO         19  // use 1k resistor if using this pin
// #define TOUCH_CLK        18  // parallel with TFT_SCLK
// #define TOUCH_CS         21  
// #define TOUCH_DIN        23  // parallel with TFT_MOSI
// #define TOUCH_DO         19  // parallel with TFT_MISO
// #define TOUCH_IRQ        22 
// #define TFT_BACKLIGHT_ON HIGH


#define TOUCH_CS 21
#endif