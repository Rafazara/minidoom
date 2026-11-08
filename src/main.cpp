#include <Arduino.h>
#include "config.h"

#ifdef MODE_OLED_DIAG
#include "doom_graphics.h"
#include "doom_validation_test.h"
#include "oled_ui.h"

static bool g_display_ready = false;
static uint8_t g_pattern = 0;
static uint8_t g_animation_frames = 0;
static uint32_t g_last_update = 0;

void setup() {
  Serial.begin(115200);
  delay(1000);

  Wire.begin(OLED_SDA, OLED_SCL);
  Wire.setClock(100000);
  g_display_ready = OLEDUI::initialize(OLED_ADDRESS_PRIMARY);
  if (g_display_ready) {
    DoomGraphics::initialize();
    g_last_update = millis() - 3000;
  } else {
    LOG_ERROR("OLED diagnostic initialization failed");
  }
}

void loop() {
  if (!g_display_ready) {
    delay(1000);
    return;
  }

  bool moving = g_pattern == DoomValidationTest::TEST_PATTERN_MOVING_SHAPE;
  uint32_t interval = moving ? 100 : 3000;
  uint32_t now = millis();
  if (now - g_last_update < interval) return;
  g_last_update = now;

  DoomValidationTest::runTest((DoomValidationTest::TestPattern)g_pattern);
  DoomGraphics::blitBufferToOLED();

  if (moving && ++g_animation_frames < 30) return;

  g_animation_frames = 0;
  g_pattern = (g_pattern + 1) % DoomValidationTest::TEST_PATTERN_COUNT;
}
#else
#include "doom/doom_integration.h"

void setup() {
  Serial.begin(115200);
  delay(1000);
  DoomIntegration::setup();
}

void loop() {
  DoomIntegration::loop();
}
#endif
