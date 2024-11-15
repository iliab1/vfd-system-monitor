#include <Arduino.h>

// Display dimensions
const byte DISPLAY_ROWS = 2;
const byte DISPLAY_COLUMNS = 20 + 1; // 1 extra for null-terminator

char buffer[DISPLAY_ROWS][DISPLAY_COLUMNS]; // Unified buffer for all display rows

// Function to set the cursor position on the VFD display
void setCursor(byte column, byte row) { // 1-indexed, not 0-indexed
    Serial1.write(0x1F); // Command prefix
    Serial1.write(0x24); // SetCursor command
    Serial1.write(column); // Column position
    Serial1.write(row);    // Row position
}

// Function to update the display content from buffers
void updateDisplay() {
    for (byte row = 0; row < DISPLAY_ROWS; row++) {
        setCursor(1, row + 1); // Set the cursor to the appropriate row
        Serial1.print(buffer[row]); // Write the row to the display
    }
}

// Function to initialize and clear the display
void initializeDisplay() {
    Serial1.write(0x1B); // Command prefix
    Serial1.write(0x40); // ClearScreen command
}

// Function to map input data into the display buffer dynamically
void updateBuffer(const char* input) {
    for (byte row = 0; row < DISPLAY_ROWS; row++) {
        memset(buffer[row], ' ', DISPLAY_COLUMNS - 1); // Fill row with spaces
        buffer[row][DISPLAY_COLUMNS - 1] = '\0'; // Ensure null-termination
    }

    size_t inputLength = strlen(input);
    size_t charsPerRow = DISPLAY_COLUMNS - 1;

    for (byte row = 0; row < DISPLAY_ROWS; row++) {
        size_t startIdx = row * charsPerRow;
        if (startIdx < inputLength) {
            size_t copyLength = min(charsPerRow, inputLength - startIdx);
            memcpy(buffer[row], input + startIdx, copyLength);
        }
    }
}

void setup() {
    Serial.begin(9600); // Initialize serial for host computer
    Serial1.begin(9600, SERIAL_8N1, 20, 21); // Initialize Serial1 for VFD screen (TX=GPIO20, RX=GPIO21)

    for (byte row = 0; row < DISPLAY_ROWS; row++) {
        memset(buffer[row], '\0', DISPLAY_COLUMNS); // Clear all rows in the buffer
    }
    delay(1000); // Delay to allow the VFD screen to initialize
    initializeDisplay(); // Clear the VFD screen
}

void loop() {
    if (Serial.available()) {
        static char inputBuffer[DISPLAY_ROWS * (DISPLAY_COLUMNS)]; // Buffer to store the incoming string
        size_t bytesRead = Serial.readBytesUntil('\n', inputBuffer, sizeof(inputBuffer) - 1); // Read the incoming string
        inputBuffer[bytesRead] = '\0'; // Null-terminate the string

        updateBuffer(inputBuffer); // Update the display buffer with new content
        updateDisplay();           // Update the display with the new buffer content
    }
}
