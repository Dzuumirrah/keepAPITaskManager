#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <TFT_eSPI.h>   // see .pio\libdeps\freenove_esp32_wrover\TFT_eSPI\User_Setups\Setup21_ILI9488.h for setup
#include <SPI.h>


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


// Testing ILI9488 display
void testDisplay();
#endif