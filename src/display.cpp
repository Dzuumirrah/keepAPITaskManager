#include "config.h"

#include "symbols.h"

byte DISPLAY_ROTATION = 1;
void drawLoadingCircleAnimation(int cx, int cy, uint16_t color1, uint16_t color2);

TFT_eSPI tft = TFT_eSPI(); // Create an instance of TFT_eSPI
void SplashScreen(int INIT) {
  tft.setRotation(DISPLAY_ROTATION);
  tft.fillScreen(TFT_WHITE);
  tft.setCursor(100, 115+31);
  tft.setTextColor(TFT_BLACK);
  tft.setFreeFont(&FreeSansBold18pt7b);
  tft.println("TasksSyncESP32");
  unsigned long startAttemptTime = millis();
  const unsigned long requestTimeout = 10000; // 10 seconds timeout
  // WiFi initialization
  if (INIT == 1){
    tft.drawBitmap(233, 160, bitmap_wifi, 15, 12, TFT_BLACK);
    tft.setFreeFont(&FreeSerifItalic9pt7b);
    tft.setCursor(175, 176 + 9);
    tft.print("Configuring WiFi");

    while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < requestTimeout) {
      drawLoadingCircleAnimation(234, 261, TFT_BLACK, TFT_LIGHTGREY);
      delay(500); // Wait for WiFi to connect
    }
    tft.fillRect(175, 170, tft.width(), 22, TFT_WHITE);
    if (WiFi.status() == WL_CONNECTED) {
      tft.setCursor(175, 176 + 9);
      tft.println("WIFI connected!");
      tft.setCursor(175, 197 + 9);
      tft.setFreeFont(&FreeSans9pt7b);
      tft.printf(WiFi.SSID().c_str());
    } else {
      tft.setCursor(100, 176 + 9);
      tft.println("WiFi connection failed!");
    }

  } else if (INIT == 2) {
    tft.drawBitmap(233, 158, bitmap_mqtt, 15, 15, TFT_PURPLE);
    tft.setFreeFont(&FreeSerifItalic9pt7b);
    tft.setCursor(175, 176 + 9);
    tft.print("Configuring MQTT");
  }
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

void drawTasks(const std::vector<Task*>& list, int& FirstY, int TaskIndent, 
                int ChildIndent, int TASK_POINTER, const uint8_t MAX_TASKS_DISPLAYED) {
  const int boxSize = 18;
  const int containerHeight = 48;
  const int textIndent = 16; // Indent for text after checkbox
  const int radius = boxSize / 2;
  int text_y_indent = FirstY; // Y indent for text

  // Only display up to MAX_TASKS_DISPLAYED tasks, starting from TASK_POINTER
  int endIdx = std::min((int)list.size(), TASK_POINTER + MAX_TASKS_DISPLAYED);
  for (int i = TASK_POINTER; i < endIdx; ++i) {
    Task* t = list[i];
    int cx = TaskIndent + radius;
    int cy = text_y_indent + (containerHeight / 2);

    // 1) checkbox as circle
    if (t->completed) {
      tft.fillCircle(cx, cy, radius, TFT_BLACK);
    } else {
      tft.fillCircle(cx, cy, radius, TFT_WHITE);
    }
    tft.drawCircle(cx, cy, radius, TFT_BLACK);

    // 2) title
    tft.setCursor(TaskIndent + boxSize + textIndent, text_y_indent + 18);
    tft.setTextColor(TFT_BLACK);
    tft.setFreeFont(&FreeSans9pt7b);
    tft.print(t->title);
    
    // 3) due date
    if (t->due.length()) {
      tft.setCursor(TaskIndent + boxSize + textIndent, text_y_indent + 35);
      tft.setTextColor(TFT_BLACK);
      tft.setFreeFont(&FreeSerifItalic9pt7b);
      tft.print(t->due);
    }
    text_y_indent += containerHeight;
    
    // 4) children (show all children for visible parent)
    if (t->children.size()) {
      drawTasks(t->children, text_y_indent, TaskIndent + ChildIndent, ChildIndent, 0, MAX_TASKS_DISPLAYED);
    }
  }
}

// Flattens the task tree into a flat list with indentation levels
void flattenTasks(const std::vector<Task*>& roots, std::vector<std::pair<Task*, int>>& 
                    flatList, int indent = 0) {
  for (auto* t : roots) {
    flatList.push_back({t, indent});
    if (!t->children.empty()) {
      flattenTasks(t->children, flatList, indent + 1);
    }
  }
}

void drawTasksFlat(const std::vector<Task*>& roots, int& FirstY, int TaskIndent, 
  int ChildIndent, int TASK_POINTER, const uint8_t MAX_TASKS_DISPLAYED) {
  // Flatten the tree
  std::vector<std::pair<Task*, int>> flatList;
  flattenTasks(roots, flatList);

  const int boxSize = 18;
  const int containerHeight = 48;
  const int textIndent = 16; // Indent for text after checkbox
  const int radius = boxSize / 2;
  int text_y_indent = FirstY;

  int endIdx = std::min((int)flatList.size(), TASK_POINTER + MAX_TASKS_DISPLAYED);
  for (int i = TASK_POINTER; i < endIdx; ++i) {
    Task* t = flatList[i].first;
    int indentLevel = flatList[i].second;
    int effectiveIndent = TaskIndent + indentLevel * ChildIndent;
    int cx = effectiveIndent + radius;
    int cy = text_y_indent + (containerHeight / 2);

    // 1) checkbox as circle
    if (t->completed) {
      tft.fillCircle(cx, cy, radius, TFT_BLACK);
    } else {
      tft.fillCircle(cx, cy, radius, TFT_WHITE);
    }
    tft.drawCircle(cx, cy, radius, TFT_BLACK);

    // 2) title
    tft.setCursor(effectiveIndent + boxSize + textIndent, text_y_indent + 18);
    tft.setTextColor(TFT_BLACK);
    tft.setFreeFont(&FreeSans9pt7b);
    tft.print(t->title);
    
    // 3) due date
    if (t->due.length()) {
      tft.setCursor(effectiveIndent + boxSize + textIndent, text_y_indent + 35);
      tft.setTextColor(TFT_BLACK);
      tft.setFreeFont(&FreeSerifItalic9pt7b);
      // Convert RFC 3339 timestamp to desired format
      struct tm timeinfo;
      if (strptime(t->due.c_str(), "%Y-%m-%dT%H:%M:%S", &timeinfo)) {
        char buffer[32];
        strftime(buffer, sizeof(buffer), "%a, %d %b, %H:%M", &timeinfo);
        tft.print(buffer);
      } else {
        tft.print("Invalid date");
      }
    }
    text_y_indent += containerHeight;
  }
}

// Draws tasks as a flat list with a hover/selection pointer
void drawTasksFlatWithPointer(const std::vector<Task*>& roots,
                              int& FirstY,
                              int TaskIndent,
                              int ChildIndent,
                              int CURSOR_POINTER,
                              const uint8_t MAX_TASKS_DISPLAYED) {
  // Flatten the tree once
  std::vector<std::pair<Task*, int>> flatList;
  flattenTasks(roots, flatList);

  const int totalTasks = flatList.size();
  if (totalTasks == 0) return;

  // 1) Clamp pointer into [0, totalTasks-1]
  if (CURSOR_POINTER < 0) CURSOR_POINTER = 0;
  if (CURSOR_POINTER >= totalTasks) CURSOR_POINTER = totalTasks - 1;

  // 2) Determine window start so pointer is visible
  int startIdx = 0;
  if (totalTasks > MAX_TASKS_DISPLAYED) {
    // Only start scrolling once pointer reaches bottom of window
    if (CURSOR_POINTER >= MAX_TASKS_DISPLAYED) {
      startIdx = CURSOR_POINTER - (MAX_TASKS_DISPLAYED - 1);
    }
    // But never scroll past the end
    int maxStart = totalTasks - MAX_TASKS_DISPLAYED;
    if (startIdx > maxStart) startIdx = maxStart;
  }
  int endIdx = startIdx + ((MAX_TASKS_DISPLAYED < (totalTasks - startIdx)) ? MAX_TASKS_DISPLAYED : (totalTasks - startIdx));

  // 3) Draw the visible slice [startIdx, endIdx)
  const int boxSize        = 18;
  const int containerH     = 48;
  const int textIndent     = 16;
  const int radius         = boxSize / 2;
  int       y              = FirstY;

  for (int i = startIdx; i < endIdx; ++i) {
    Task* t          = flatList[i].first;
    int   indentLev  = flatList[i].second;
    int   xOffset    = TaskIndent + indentLev * ChildIndent;
    int   cx         = xOffset + radius;
    int   cy         = y + (containerH / 2);

    // Highlight the selected row
    if (i == CURSOR_POINTER) {
      tft.fillRect(0, y, tft.width(), containerH, TFT_LIGHTGREY);
    }

    // 1) Checkbox as circle
    tft.fillCircle(cx, cy, radius, t->completed ? TFT_BLACK : TFT_WHITE);
    tft.drawCircle(cx, cy, radius, TFT_BLACK);

    // 2) Title
    tft.setCursor(xOffset + boxSize + textIndent, y + 18);
    tft.setTextColor(TFT_BLACK);
    tft.setFreeFont(&FreeSans9pt7b);
    tft.print(t->title);

    // 3) Due date
    if (t->due.length()) {
      tft.setCursor(xOffset + boxSize + textIndent, y + 35);
      tft.setTextColor(TFT_BLACK);
      tft.setFreeFont(&FreeSerifItalic9pt7b);
      tft.print(t->due);
    }

    y += containerH;
  }
}

void drawStatusBar(bool (&ConnectionStatus)[2], String Wifi_SSID, String Title, String ClockTime) {
  // If MQTT and WiFi are connected
  if (ConnectionStatus[0] && ConnectionStatus[1]) {
    tft.fillRect(0, 0, tft.width(), 10+8+8, TFT_BLACK);
  } 
    // If only WiFi is not connected
    else if (!ConnectionStatus[0] && ConnectionStatus[1]) {
    tft.fillRect(0, 0, tft.width(), 10+8+8, TFT_RED);
    Wifi_SSID = "Reconnect WiFi..."; // Change title to "Connecting..." if WiFi is not connected
  } 
  // If only MQTT is not connected
    else if (ConnectionStatus[0] && !ConnectionStatus[1]) {
    tft.fillRect(0, 0, tft.width(), 10+8+8, TFT_PURPLE);
    Title = "Reconnect MQTT..."; // Change title to "Connecting..." if MQTT is not connected 
  } 
  // If both are not connected
    else {
    // Fade effect between connection states
    for (int fade = 0; fade <= 255; fade += 15) {
      uint16_t color = tft.color565(fade, 0, 0); // Fade from black to red
      tft.fillRect(0, 0, tft.width(), 10+8+8, color);
      delay(10);
    }
    Wifi_SSID = "Reconnect WiFi..."; // Change title to "Connecting..." if WiFi is not connected
    Title = "Reconnect MQTT..."; // Change title to "Connecting..." if MQTT is not connected
  }
  // Draw status bar background
  // Draw Wifi icons and SSID
  tft.drawBitmap(18, 8, bitmap_wifi, 15, 12, TFT_WHITE);
  tft.setTextColor(TFT_WHITE);
  tft.setCursor(43, 18);
  tft.setFreeFont(&FreeSerifItalic9pt7b);
  tft.print(Wifi_SSID);

  // Draw Title
  tft.setCursor(tft.width() / 2 - Title.length() * 4, 18); // Center the title
  tft.setTextColor(TFT_WHITE);
  tft.setFreeFont(&FreeSansBold9pt7b);
  tft.print(Title);

  // Draw Clock
  tft.setCursor(tft.width() - ClockTime.length() * 6 - 25, 18); // Right align the clock
  tft.setFreeFont(&FreeSerif9pt7b);
  tft.setTextColor(TFT_WHITE);
  tft.print(ClockTime);

}

void drawLoadingCircleAnimation(int cx, int cy, uint16_t color1, uint16_t color2) {
  // Draw loading circle animation
      static int angle = 0;
      int centerX = cx; // Adjust as needed for your display
      int centerY = cy;
      int radius = 13;
      int dotRadius = 2;
      // Erase previous circle (draw white over it)
      tft.fillCircle(centerX, centerY, radius + dotRadius + 2, TFT_WHITE);
      // Draw 12 dots around the circle
      for (int j = 0; j < 12; ++j) {
        float theta = (angle + j * 30) * 3.14159 / 180.0;
        int x = centerX + cos(theta) * radius;
        int y = centerY + sin(theta) * radius;
        uint16_t color = (j == 0) ? color1 : color2;
        tft.fillCircle(x, y, dotRadius, color);
      }
      angle = (angle + 30) % 360;
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
