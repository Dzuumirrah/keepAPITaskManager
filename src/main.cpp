#include "pinout_main.h"
#include "config.h"
#include "secrets.h"


void setup() {
    Serial.begin(115200);
    delay(1000);
    Serial.println("Starting device . . ..");
    tft.init();
    
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
    SplashScreen();
    HelpingLines();
    delay(2000);
    // testDisplay();
}
