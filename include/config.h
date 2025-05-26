#ifndef CONFIG_H
#define CONFIG_H

#include "task_types.h"
#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <TFT_eSPI.h>   // see .pio\libdeps\freenove_esp32_wrover\TFT_eSPI\User_Setups\Setup21_ILI9488.h for setup
#include <SPI.h>

// -------------------------------------------------------------------
//                CONNECTION CONFIGURATION
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
extern const uint8_t MAX_TASK_DISPLAY;
extern int TASKS_POINTER;

// -------------------------------------------------------------------
//                  DISPLAY CONFIGURATION
// -------------------------------------------------------------------
// Display rotation setting
// Set as 1,2,3, or 4
extern byte DISPLAY_ROTATION;
extern bool SPLASH_SCREEN; // Flag to show splash screen on first boot
extern bool needDisplayUpdate;
// Pointer to the cursor position on the TFT display
// show splash screen on TFT display on first boot
// parameters:
// - INIT: Flag for init process 
//   0 just Title, 1 for WiFi, 2 for MQTT   
void SplashScreen(int INIT = 0);
// Show helping lines on TFT display for developing convenience
void HelpingLines();
// Testing ILI9488 display
void testDisplay();
// Draw tasks on the TFT display
// parameters:
// - list: vector of Task pointers to draw
// - FirstY: reference to the first Y position to start drawing tasks
// - TaskIndent: horizontal indent for the task container
// - ChildIndent: horizontal indent for child tasks
void drawTasks(const std::vector<Task*>& list, int& FirstY, int TaskIndent, 
                int ChildIndent, int TASK_POINTER, const uint8_t MAX_TASKS_DISPLAYED);
// Draw status bar on the TFT display
// parameters:
// - ConnectionStatus: array of two boolean pointers for WiFi and MQTT connection status
// - Wifi_SSID: WiFi SSID string. Change to "connecting..." if WIFI not connected
// - Title: title string to display. Change to "Connecting..." if MQTT not connected
// - Clock: clock string to display
void drawStatusBar(bool (&ConnectionStatus)[2], String Wifi_SSID, String Title, String ClockTime);

// --------------------------------------------------------
//                    RTC CONFIGURATION
// --------------------------------------------------------
// Function to setup the RTC (Real Time Clock) using NTP
void setupRTC();
// Function to get the current time in hh:mm format
String getCurrentTime();
#endif