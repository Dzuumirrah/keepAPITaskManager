#include "config.h"
// #include "pinout_main.h"

byte DISPLAY_ROTATION = 1;

TFT_eSPI tft = TFT_eSPI(); // Create an instance of TFT_eSPI
void SplashScreen() {
  tft.setRotation(DISPLAY_ROTATION);
  tft.fillScreen(TFT_WHITE);
  tft.setCursor(100, 115+31);
  tft.setTextColor(TFT_BLACK);
  tft.setFreeFont(&FreeSansBold18pt7b);
  tft.println("TasksSyncESP32");
}

void HelpingLines() {
  // Draw horizontal lines
  for (int i = 0; i < tft.height(); i += 20) {
    tft.drawLine(0, i, tft.width(), i, TFT_DARKGREY);
    if (i % 100 == 0) {
      tft.drawLine(0, i, tft.width(), i, TFT_RED); // Highlight every 100 pixels
    }
  }
  // Draw vertical lines
  for (int i = 0; i < tft.width(); i += 20) {
    tft.drawLine(i, 0, i, tft.height(), TFT_DARKGREY);
    if (i % 100 == 0) {
      tft.drawLine(i, 0, i, tft.height(), TFT_RED); // Highlight every 100 pixels
    }
  }
}

int i = 0;
void testDisplay()  {

  Serial.print("set rotation to");
  Serial.println(i);
  Serial.println();
  tft.setRotation(i);
  delay(2000);
  tft.fillScreen(TFT_BLACK);
  tft.drawRect(0, 0, tft.width(), tft.height(), TFT_GREEN);

  // Set "cursor" at top left corner of display (0,0) and select font 4
  tft.setCursor(0, 4, 4);

  // Set the font colour to be white with a black background
  tft.setTextColor(TFT_WHITE);

  // We can now plot text on screen using the "print" class
  tft.println(" Initialised default\n");
  tft.println(" White text");
  
  tft.setTextColor(TFT_RED);
  tft.println(" Red text");
  
  tft.setTextColor(TFT_GREEN);
  tft.println(" Green text");
  
  tft.setTextColor(TFT_BLUE);
  tft.println(" Blue text");

  delay(2000);

  tft.invertDisplay( false ); // Where i is true or false
 
  tft.fillScreen(TFT_BLACK);
  tft.drawRect(0, 0, tft.width(), tft.height(), TFT_GREEN);

  tft.setCursor(0, 4, 4);

  tft.setTextColor(TFT_WHITE);
  tft.println(" Invert OFF\n");

  tft.println(" White text");
  
  tft.setTextColor(TFT_RED);
  tft.println(" Red text");
  
  tft.setTextColor(TFT_GREEN);
  tft.println(" Green text");
  
  tft.setTextColor(TFT_BLUE);
  tft.println(" Blue text");

  delay(2000);


  // Binary inversion of colours
  tft.invertDisplay( true ); // Where i is true or false
 
  tft.fillScreen(TFT_BLACK);
  tft.drawRect(0, 0, tft.width(), tft.height(), TFT_GREEN);

  tft.setCursor(0, 4, 4);

  tft.setTextColor(TFT_WHITE);
  tft.println(" Invert ON\n");

  tft.println(" White text");
  
  tft.setTextColor(TFT_RED);
  tft.println(" Red text");
  
  tft.setTextColor(TFT_GREEN);
  tft.println(" Green text");
  
  tft.setTextColor(TFT_BLUE);
  tft.println(" Blue text");

  delay(2000);
  i++;
  if (i > 3) {
    i = 0;
  }
}
