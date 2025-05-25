#ifndef CONFIG_H
#define CONFIG_H

#include "task_types.h"
#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <TFT_eSPI.h>   // see .pio\libdeps\freenove_esp32_wrover\TFT_eSPI\User_Setups\Setup21_ILI9488.h for setup
#include <SPI.h>

// -------------------------------------------------------------------
// CONNECTION CONFIGURATION
// -------------------------------------------------------------------

// Class configuration for MQTT connection
extern PubSubClient mqtt;
extern TFT_eSPI tft;

extern byte MCmacAdrr[6]; // Array to store the MAC address as bytes

// WiFi Setup. Can be used to generate the MAC address of the ESP32 device
// and store it in the MCmacAdrr array as a global variable.
void setupWiFi(const char* ssid, const char* password);
// MQTT setup.
void mqttConnect(const char* mqtt_server, const uint16_t mqtt_port, const char* mqtt_topic_tasks);
// Callback for MQTT communication.
void mqttCallback(char* topic, byte* payload, unsigned int length);

// --------------------------------------------------------
//                TASK PARSING AND STORAGE
// --------------------------------------------------------
// the “roots” of your task forest:
extern String lastPayload;
extern std::vector<Task*> roots;
void parseJson(const String& payload);
void buildTree();

// --------------------------------------------------------
//                    BUTTONS AND PAGES
// --------------------------------------------------------
// Function declarations and pointers for button actions
void buttonUpAction();
void buttonDownAction();
void buttonYesAction();
void buttonNoAction();
extern void (*ButtonActionPointers [4])();
// Pointers to current active page
extern uint8_t PAGE_POINTERS; 
// and task cursor
extern uint16_t TASKS_POINTERS;

// -------------------------------------------------------------------
// ACTION CONFIGURATION
// -------------------------------------------------------------------
extern uint8_t TASK_MAX_COUNT;



// -------------------------------------------------------------------
// DISPLAY CONFIGURATION
// -------------------------------------------------------------------
// Display rotation setting
// Set as 1,2,3, or 4
extern byte DISPLAY_ROTATION;
// Pointer to the cursor position on the TFT display
extern uint16_t CURSOR_POINTER;
// show splash screen on TFT display on first boot
void SplashScreen();
// Show helping lines on TFT display for developing convenience
void HelpingLines();
// Testing ILI9488 display
void testDisplay();
#endif