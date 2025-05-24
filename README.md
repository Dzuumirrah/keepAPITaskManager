## Hardware inti yang digunakan
1. ESP32 microcontroller
2. Push Button (x4)
3. Rechargable battery 3.3V (x2)
4. Battery socket
5. OLED or TFT display (minimum 3.5", used in this file TFT ILI9488)

## Software dan library yang digunakan
### Server and IDE
1. gkeepapi
2. MQTT broker (Mosquitto)
3. PlatformIO IDE in VSCode
### Microcontroller
1. TFT_eSPI

## Fitur 
1. Sinkronisasi notes berupa tugas di Google Keep
2. Check dan uncheck tugas yang telah selesai
3. Alarm dan reminder setiap pagi (menggunakan Text-To-Speech untuk membaca tugas pada hari itu)
