#include "pinout_main.h"
#include "config.h"
#include "secrets.h"

// Class configuration for MQTT connection
WiFiClient netClient;
PubSubClient mqtt(netClient);

constexpr uint8_t BUTTON_UP     = 13; 
constexpr uint8_t BUTTON_DOWN   = 14; 
constexpr uint8_t BUTTON_YES    = 26; 
constexpr uint8_t BUTTON_NO     = 27;

uint8_t BUTTONS[] = {BUTTON_UP, BUTTON_DOWN, BUTTON_YES, BUTTON_NO};
bool buttonPressed[4] = {false, false, false, false}; // Array to track button states

bool UsingConnection = false; // Variable to check connection status

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
    // initialize Buttons
    for (auto button : BUTTONS) {
        pinMode(button, INPUT_PULLUP); // Set buttons as input with pull-up resistor
    }
    Serial.println("Starting device . . .");
    tft.init();   

    if (UsingConnection){
        setupWiFi(WIFI_SSID, WIFI_PASSWORD);
        mqttConnect(MQTT_SERVER, MQTT_PORT, mqtt_topic_tasks);
    }
    Serial.println("Setup complete!");
}

void loop() {
    // Check if any button is pressed
    for (int i = 0; i < sizeof(BUTTONS) / sizeof(BUTTONS[0]); i++) {
        if ((digitalRead(BUTTONS[i]) == LOW) && !buttonPressed[i]) { // Button pressed
            // Call the corresponding action function
            ButtonActionPointers[i]();
            buttonPressed[i] = true; // Mark button as pressed
            delay(100); // Debounce delay
        } else if (digitalRead(BUTTONS[i]) == HIGH) {
            buttonPressed[i] = false; // Mark button as not pressed
        }
    }
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
    SplashScreen();
    HelpingLines();
    delay(2000);
    // testDisplay();
    // delay(1000);
    // Check if the device is connected to WiFi
    if (UsingConnection) {
        if (WiFi.status() != WL_CONNECTED) {
            Serial.println("WiFi disconnected. Reconnecting...");
            setupWiFi(WIFI_SSID, WIFI_PASSWORD);
            // Check if the MQTT client is connected
        }
        if (!mqtt.connected()) {
            Serial.println("[MQTT] Disconnected.");
            mqttConnect(MQTT_SERVER, MQTT_PORT, mqtt_topic_tasks);
        }
        // Handle incoming messages and maintain the connection    
        mqtt.loop();
    }
    
}
