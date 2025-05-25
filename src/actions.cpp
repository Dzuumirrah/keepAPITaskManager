#include "config.h"
#include "pinout_main.h"

uint16_t TASKS_POINTERS = 0;    // Variable to keep track of the current task
uint8_t PAGE_POINTERS = 0;      // Variable to keep track of the current page

void (*ButtonActionPointers [4])() = {
    buttonUpAction,   // Button Up
    buttonDownAction, // Button Down
    buttonYesAction,  // Button Yes
    buttonNoAction    // Button No
};
// Function declarations for button actions
void buttonUpAction() {
    Serial.println("Button Up Pressed");
};

void buttonDownAction() {
    Serial.println("Button Down Pressed");
};

void buttonYesAction() {
    Serial.println("Button Yes Pressed");
    PAGE_POINTERS++;
    Serial.print("Current Page Pointer: ");
    Serial.println(PAGE_POINTERS);
    if (PAGE_POINTERS > 4) {
        PAGE_POINTERS = 0;
    }
};

void buttonNoAction() {
    Serial.println("Button No Pressed");
    PAGE_POINTERS--;
    Serial.print("Current Page Pointer: ");
    Serial.println(PAGE_POINTERS);
    if (PAGE_POINTERS < 0) {
        PAGE_POINTERS = 4;
    }
};

