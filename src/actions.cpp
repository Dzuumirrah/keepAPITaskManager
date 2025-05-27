#include "config.h"
#include "pinout_main.h"

uint8_t PAGE_POINTERS = 0;      // Variable to keep track of the current page
int TASKS_POINTER_DISPLAY_POSITION = 0;

bool syncCountDown = false; // Flag to indicate if countdown needs to be synchronized

void (*ButtonActionPointers [4])() = {
    buttonUpAction,   // Button Up
    buttonDownAction, // Button Down
    buttonYesAction,  // Button Yes
    buttonNoAction    // Button No
};
// Function declarations for button actions
void buttonUpAction() {
    Serial.println("Button Up Pressed");
    if ((TASKS_POINTER > 0)) {
        TASKS_POINTER--; 
    }
    if (TASKS_POINTER_DISPLAY_POSITION > 0) {
        TASKS_POINTER_DISPLAY_POSITION--;
    }
};

void buttonDownAction() {
    Serial.println("Button Down Pressed");
    if ((TASKS_POINTER < allTasks.size() - 1)) {
        TASKS_POINTER++; 
    }
    if (TASKS_POINTER_DISPLAY_POSITION < MAX_TASK_DISPLAY - 1) {
        TASKS_POINTER_DISPLAY_POSITION++;
    }
};

bool buttonYesPressedAgain = false; // Flag to track if button Yes was pressed again
void buttonYesAction() {
    Serial.println("Button Yes Pressed");

    // If countdown is already running, treat this as a cancel action
    if (syncCountDown) {
        syncCountDown = false; // Cancel the countdown
        Serial.println("Sync countdown cancelled.");
        // Optionally, clear or update the UI to reflect cancellation
        drawSyncCountdown(FIRST_TASK_Y_INDENT + (48 * TASKS_POINTER_DISPLAY_POSITION) + 35);
        return;
    }
    // Start the countdown
    syncCountDown = true;
    drawSyncCountdown(FIRST_TASK_Y_INDENT + (48 * TASKS_POINTER_DISPLAY_POSITION) + 35);
    yield();
    // 
    // PAGE_POINTERS++;
    // Serial.print("Current Page Pointer: ");
    // Serial.println(PAGE_POINTERS);
    // if (PAGE_POINTERS > 4) {
    //     PAGE_POINTERS = 0;
    // }
}

void buttonNoAction() {
    Serial.println("Button No Pressed");
    // PAGE_POINTERS--;
    // Serial.print("Current Page Pointer: ");
    // Serial.println(PAGE_POINTERS);
    // if (PAGE_POINTERS < 0) {
    //     PAGE_POINTERS = 4;
    // }
};

