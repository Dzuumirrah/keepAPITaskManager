// This file will be used for writing connection functions
// like WiFi, MQTT, etc.

#include "config.h"

byte MCmacAdrr[6];

void setupWiFi(const char* ssid, const char* password) {
    WiFi.setHostname("Dzuu-ESP32");
    WiFi.begin(ssid, password);
    Serial.print("Connecting to WiFi");
    while (WiFi.status() != WL_CONNECTED) {
        
        Serial.print(".");
        delay(500);
    }
    Serial.println("Successfully connected to WiFi!");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
    Serial.print("MAC address: ");

    String macAdrr = WiFi.macAddress();

    // Place this array inside an extern variable to be used in other files
    // extern byte MCmacAdrr[6];
    sscanf(macAdrr.c_str(), "%hhx:%hhx:%hhx:%hhx:%hhx:%hhx", 
       &MCmacAdrr[0], &MCmacAdrr[1], &MCmacAdrr[2], 
       &MCmacAdrr[3], &MCmacAdrr[4], &MCmacAdrr[5]);

    Serial.println(macAdrr);
    Serial.println("\n\n\n\n");
}

void mqttConnect(const char* mqtt_server, const uint16_t mqtt_port,const char* topic) {
    mqtt.setBufferSize(512); // Adjust size as needed
    mqtt.setServer(mqtt_server, mqtt_port);
    mqtt.setCallback(mqttCallback);
    Serial.println("Connecting to MQTT server...");
    while (!mqtt.connected()) {
        if (mqtt.connect("Dzuumirrah ESP32")) {
            Serial.println(".");
            mqtt.subscribe(topic); // Subscribe to a topic
        } else {
            Serial.print("Failed, rc=");
            Serial.print(mqtt.state());
            Serial.println(" try again in 2 seconds");
            delay(2000);
        }
    }
    Serial.println("Connected to MQTT server!");
}

void mqttCallback(char* topic, byte* payload, unsigned int length) {
    // Store message taken from MQTT inside "msg"
    String msg;
    for (int i = 0; i < length; i++){
        msg += (char)payload[i];
    }
    
    Serial.print("[MQTT] Message arrived on [");
    Serial.print(topic);
    Serial.println("]:\n");
    Serial.println(msg);
}




