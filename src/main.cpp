#include "pinout_main.h"
#include "config.h"
#include "secrets.h"

// Class configuration for MQTT connection
WiFiClient netClient;
PubSubClient mqtt(netClient);

// TFT_eSPI tft = TFT_eSPI(); // Create an instance of TFT_eSPI


// WiFi configuration
const char* WIFI_SSID       = wifi_ssid;
const char* WIFI_PASSWORD   = wifi_password;
// MQTT configuration
const char* MQTT_SERVER             = mqtt_server; 
const uint16_t MQTT_PORT            = mqtt_port;   
const char* mqtt_topic_tasks        = topic_task; 
const char* mqtt_topic_completion   = topic_completion;


void setup() {
    Serial.begin(115200);
    delay(1000);
    Serial.println("Starting device . . ..");
    tft.init();   

    setupWiFi(WIFI_SSID, WIFI_PASSWORD);
    mqttConnect(MQTT_SERVER, MQTT_PORT, mqtt_topic_tasks);

    Serial.println("Setup complete!");
}

void loop() {
    // Restart command from Serial Monitor
    if (Serial.available() > 0) {
        String command = Serial.readStringUntil('\n');
        if (command == "restart" || command == "reset") {
            Serial.println("\n\nRestarting ESP32. . .");
            ESP.restart();
            Serial.println("Selesai Restart ESP. \n\n");
        } else {
            Serial.println("Perintah tidak dikenali. Silakan coba lagi.");
        }
        return;
    }
    
    // testDisplay();
    // delay(1000);
    // Check if the device is connected to WiFi
    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("WiFi disconnected. Reconnecting...");
        setupWiFi(WIFI_SSID, WIFI_PASSWORD);
    }

    // Check if the MQTT client is connected
    if (!mqtt.connected()) {
        Serial.println("[MQTT] Disconnected.");
        mqttConnect(MQTT_SERVER, MQTT_PORT, mqtt_topic_tasks);
    }

    // Handle incoming messages and maintain the connection
    mqtt.loop();
}
