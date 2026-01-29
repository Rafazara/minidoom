#include "input.h"
#include "config.h"
#include <Arduino.h>
#include <string.h>

namespace Input {

// ============================================================================
// GPIO PIN MAPPING (Configure these for your hardware)
// ============================================================================
// ESP32 GPIO pins available: 0-19, 21-23, 25-27, 32-39
#define GPIO_BUTTON_UP     32
#define GPIO_BUTTON_DOWN   33
#define GPIO_BUTTON_LEFT   25
#define GPIO_BUTTON_RIGHT  26
#define GPIO_BUTTON_FIRE   27

// ============================================================================
// DEBOUNCE CONFIGURATION
// ============================================================================
static const uint16_t DEBOUNCE_MS = 20;  // 20ms debounce threshold

// ============================================================================
// STATE MANAGEMENT
// ============================================================================
static bool g_initialized = false;
static uint8_t g_inputState = 0;  // Bitmask of current input state
static uint32_t g_lastDebounceTime[5] = {0, 0, 0, 0, 0};  // Last update time for each button
static uint8_t g_debouncedState[5] = {0, 0, 0, 0, 0};    // Debounced state for each button

// Array indices for button tracking
static const uint8_t BTN_UP = 0;
static const uint8_t BTN_DOWN = 1;
static const uint8_t BTN_LEFT = 2;
static const uint8_t BTN_RIGHT = 3;
static const uint8_t BTN_FIRE = 4;

// GPIO pins for all buttons
static const uint8_t BTN_PINS[5] = {
    GPIO_BUTTON_UP,
    GPIO_BUTTON_DOWN,
    GPIO_BUTTON_LEFT,
    GPIO_BUTTON_RIGHT,
    GPIO_BUTTON_FIRE
};

// Bitmask for each button
static const uint8_t BTN_MASKS[5] = {
    INPUT_BTN_UP,
    INPUT_BTN_DOWN,
    INPUT_BTN_LEFT,
    INPUT_BTN_RIGHT,
    INPUT_BTN_FIRE
};

// Button names for debug output
static const char* BTN_NAMES[5] = {
    "UP", "DOWN", "LEFT", "RIGHT", "FIRE"
};

/**
 * Initialize input system
 * Sets GPIO pins to INPUT mode with internal pull-ups
 */
void initialize() {
    if (g_initialized) {
        Serial.println(F("[INPUT] Already initialized, skipping"));
        return;
    }

    Serial.println(F("[INPUT] Initializing input system..."));

    // Configure all button pins
    pinMode(GPIO_BUTTON_UP, INPUT_PULLUP);
    pinMode(GPIO_BUTTON_DOWN, INPUT_PULLUP);
    pinMode(GPIO_BUTTON_LEFT, INPUT_PULLUP);
    pinMode(GPIO_BUTTON_RIGHT, INPUT_PULLUP);
    pinMode(GPIO_BUTTON_FIRE, INPUT_PULLUP);

    // Clear state
    g_inputState = 0;
    memset(g_lastDebounceTime, 0, sizeof(g_lastDebounceTime));
    memset(g_debouncedState, 0, sizeof(g_debouncedState));

    Serial.println(F("[INPUT] GPIO pins configured (UP=32, DOWN=33, LEFT=25, RIGHT=26, FIRE=27)"));
    Serial.println(F("[INPUT] Debounce threshold: 20ms"));
    Serial.println(F("[INPUT] Input system ready"));

    g_initialized = true;
}

/**
 * Apply debounce to a single button
 * @param buttonIdx: Index of button (0-4)
 * @param currentState: Current GPIO state (HIGH=unpressed due to pull-up)
 * @return: true if state changed and debounce threshold met
 */
static bool debounceButton(uint8_t buttonIdx, bool currentState) {
    uint32_t now = millis();
    bool stateChanged = (g_debouncedState[buttonIdx] != (currentState ? 0 : 1));

    // Only update if debounce time elapsed since last change
    if (stateChanged && (now - g_lastDebounceTime[buttonIdx] >= DEBOUNCE_MS)) {
        g_debouncedState[buttonIdx] = currentState ? 0 : 1;  // Invert (pull-up logic)
        g_lastDebounceTime[buttonIdx] = now;
        return true;
    }

    return false;
}

/**
 * Read GPIO states and apply debounce
 * Updates g_inputState bitmask with debounced values
 * Call every frame before game tick
 */
void readInput() {
    if (!g_initialized) {
        return;
    }

    // Clear input state
    g_inputState = 0;

    // Read and debounce each button
    for (uint8_t i = 0; i < 5; i++) {
        bool gpioState = digitalRead(BTN_PINS[i]);
        
        // Apply debounce logic
        debounceButton(i, gpioState);
        
        // Update bitmask if button is pressed (debounced)
        if (g_debouncedState[i]) {
            g_inputState |= BTN_MASKS[i];
        }
    }

    // Log new input events (buttons pressed)
    static uint8_t lastState = 0;
    uint8_t newPresses = g_inputState & ~lastState;  // XOR for transitions
    
    if (newPresses) {
        if (newPresses & INPUT_BTN_UP) {
            Serial.println(F("[INPUT] UP"));
        }
        if (newPresses & INPUT_BTN_DOWN) {
            Serial.println(F("[INPUT] DOWN"));
        }
        if (newPresses & INPUT_BTN_LEFT) {
            Serial.println(F("[INPUT] LEFT"));
        }
        if (newPresses & INPUT_BTN_RIGHT) {
            Serial.println(F("[INPUT] RIGHT"));
        }
        if (newPresses & INPUT_BTN_FIRE) {
            Serial.println(F("[INPUT] FIRE"));
        }
    }

    lastState = g_inputState;
}

/**
 * Get combined input state (bitmask)
 */
uint8_t getInput() {
    return g_inputState;
}

/**
 * Individual input queries
 */
bool isInputUp() {
    return (g_inputState & INPUT_BTN_UP) != 0;
}

bool isInputDown() {
    return (g_inputState & INPUT_BTN_DOWN) != 0;
}

bool isInputLeft() {
    return (g_inputState & INPUT_BTN_LEFT) != 0;
}

bool isInputRight() {
    return (g_inputState & INPUT_BTN_RIGHT) != 0;
}

bool isInputFire() {
    return (g_inputState & INPUT_BTN_FIRE) != 0;
}

/**
 * Check if any movement input is active
 */
bool isMoving() {
    return (g_inputState & (INPUT_BTN_UP | INPUT_BTN_DOWN | INPUT_BTN_LEFT | INPUT_BTN_RIGHT)) != 0;
}

/**
 * Get input debug string
 * Format: "[U] [ ] [L] [R] [ ]"
 */
const char* getInputDebugString() {
    static char debugStr[32];
    snprintf(debugStr, sizeof(debugStr),
        "[%s] [%s] [%s] [%s] [%s]",
        isInputUp() ? "U" : " ",
        isInputDown() ? "D" : " ",
        isInputLeft() ? "L" : " ",
        isInputRight() ? "R" : " ",
        isInputFire() ? "F" : " "
    );
    return debugStr;
}

/**
 * Check initialization status
 */
bool isInitialized() {
    return g_initialized;
}

}  // namespace Input
