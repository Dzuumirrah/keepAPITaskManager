#include "pinout_main.h"
#include "config.h"
#include "secrets.h"
#include <time.h>

// Class configuration for MQTT connection
WiFiClient netClient;
PubSubClient mqtt(netClient);

constexpr uint8_t BUTTON_UP     = 13; 
constexpr uint8_t BUTTON_DOWN   = 14; 
constexpr uint8_t BUTTON_YES    = 26; 
constexpr uint8_t BUTTON_NO     = 27;

#define INIT_WIFI 1
#define INIT_MQTT 2

uint8_t BUTTONS[]       = {BUTTON_UP, BUTTON_DOWN, BUTTON_YES, BUTTON_NO};
bool buttonPressed[4]   = {false, false, false, false}; // Array to track button states

// Flag for development modes
bool UsingConnection = true; // Variable to check connection status
bool UsingHelpingLines = false; // Variable to check if helping lines are used

bool connectionStatus[2] = {true, true}; // WiFi and MQTT connection status
String lastClockTime;
int TASKS_POINTER = 0;
const uint8_t MAX_TASK_DISPLAY = 6;
bool needDisplayUpdate = true;
int TASKS_X_INDENT = 16;
int CHILD_INDENT = 36;
int FIRST_TASK_Y_INDENT = 36;
bool SPLASH_SCREEN = true; // Flag to show splash screen on first boot
// TFT_eSPI tft = TFT_eSPI(); // Create an instance of TFT_eSPI
#define TITLE "TasksSyncESP32"


// WiFi configuration
const char* WIFI_SSID       = wifi_ssid;
const char* WIFI_PASSWORD   = wifi_password;
// MQTT configuration
const char* MQTT_SERVER             = mqtt_server; 
const uint16_t MQTT_PORT            = mqtt_port;   
const char* mqtt_topic_tasks        = topic_task;

void setup() {
    Serial.begin(115200);
    delay(1000);
    // initialize Buttons
    for (auto button : BUTTONS) {
        pinMode(button, INPUT_PULLUP); // Set buttons as input with pull-up resistor
    }
    Serial.println("Starting device . . .");
    tft.init();
    tft.setTextWrap(false);
    tft.setTextSize(1);
    SplashScreen();
    if (UsingConnection){
        setupWiFi(WIFI_SSID, WIFI_PASSWORD);
        SplashScreen(INIT_WIFI);
        mqttConnect(MQTT_SERVER, MQTT_PORT, mqtt_topic_tasks);
        SplashScreen(INIT_MQTT);
        setupRTC(); // Initialize the RTC
    }
    Serial.println("Setup complete!");
    SPLASH_SCREEN = false;

}

unsigned long lastConnectAttempt =0; 
void loop() {
    // Check if any button is pressed
    for (int i = 0; i < sizeof(BUTTONS) / sizeof(BUTTONS[0]); i++) {
        if ((digitalRead(BUTTONS[i]) == LOW) && !buttonPressed[i]) { // Button pressed
            // Call the corresponding action function
            ButtonActionPointers[i]();
            buttonPressed[i] = true; // Mark button as pressed
            needDisplayUpdate = true; // Set flag to update display
            Serial.printf("TASK_POINTER: %d\n", TASKS_POINTER);
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
            Serial.println("Successfully Restart ESP. \n\n");
        } else if (command == "connect") {
            UsingConnection = true;
            Serial.println("Ready to connect to WiFi and MQTT.");
        } else if (command == "disconnect") {
            UsingConnection = false;
            Serial.println("Disconnecting from WiFi and MQTT.");
        } else if (command == "lines") {
            UsingHelpingLines = !UsingHelpingLines; // Toggle helping lines

        } else {
            Serial.println("Order not recognized.");
        }
        return;
    }
    // Display the current time on the screen
    String currentTime = getCurrentTime();
    // Check if the device is connected to WiFi
    // Only update the display when there is button input (buttonPressed is true for any button)
    connectionStatus[0] = (WiFi.status() == WL_CONNECTED);
    connectionStatus[1] = mqtt.connected();
    static bool previousConnectionStatus[2] = {false, false}; // Track previous connection status
    if ((connectionStatus[0] != previousConnectionStatus[0] || 
            connectionStatus[1] != previousConnectionStatus[1]) ||
            (lastClockTime != currentTime)) {
        drawStatusBar(connectionStatus, WiFi.SSID().c_str(), TITLE, currentTime);
        lastClockTime = currentTime; // Update last clock time
        previousConnectionStatus[0] = connectionStatus[0]; // Update previous WiFi status
        previousConnectionStatus[1] = connectionStatus[1]; // Update previous MQTT status
    }
    if (needDisplayUpdate) {
        tft.fillScreen(TFT_WHITE);
        if (UsingHelpingLines) {
            HelpingLines();
        }
        drawStatusBar(connectionStatus, WiFi.SSID().c_str(), TITLE, currentTime);
        // Highlight active task
        tft.fillRect(0, FIRST_TASK_Y_INDENT + (48 * TASKS_POINTER_DISPLAY_POSITION), tft.width(), 48, TFT_LIGHTGREY);
        drawTasks(allTasks, FIRST_TASK_Y_INDENT, TASKS_X_INDENT, CHILD_INDENT, TASKS_POINTER, MAX_TASK_DISPLAY);
        // drawTasks(roots, FIRST_TASK_Y_INDENT, TASKS_X_INDENT, CHILD_INDENT);
        needDisplayUpdate = false;
    }

    drawSyncCountdown(FIRST_TASK_Y_INDENT + (48 * TASKS_POINTER_DISPLAY_POSITION) + 35);
    // Reset last attempt time for WiFi and MQTT connection
    if (UsingConnection) {
        if (WiFi.status() != WL_CONNECTED && millis() - lastConnectAttempt > 5000) {
            Serial.println("WiFi disconnected. Reconnecting...");
            setupWiFi(WIFI_SSID, WIFI_PASSWORD);
            lastConnectAttempt = millis(); // Update last connect attempt time
        }
        if (!mqtt.connected() && millis() - lastConnectAttempt > 5000) {
            Serial.println("[MQTT] Disconnected.");
            mqttConnect(MQTT_SERVER, MQTT_PORT, mqtt_topic_tasks);
            lastConnectAttempt = millis(); // Update last connect attempt time
        }
        // Handle incoming messages and maintain the connection    
        mqtt.loop();
    } else {
        WiFi.disconnect(); 
        mqtt.disconnect();
    }

}

// Function to initialize the RTC
void setupRTC() {
    configTime(7 * 3600, 0, "pool.ntp.org", "time.nist.gov"); // Set timezone to UTC+7 (Jakarta)
    Serial.println("Syncronizing time with NTP servers...");
    struct tm timeinfo;
    if (!getLocalTime(&timeinfo)) {
        Serial.println("Failed to obtain time.");
        return;
    }
    Serial.println("Time synchronized successfully!");
}

// Function to get the current time in hh:mm format
String getCurrentTime() {
    struct tm timeinfo;
    if (!getLocalTime(&timeinfo)) {
        return "--:--"; // Return placeholder if time is not available
    }
    char timeString[6];
    strftime(timeString, sizeof(timeString), "%H:%M", &timeinfo);
    return String(timeString);
}
