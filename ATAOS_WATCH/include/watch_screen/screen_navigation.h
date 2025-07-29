#pragma once

// Button Pins
#define BUTTON_PIN_LEFT    33
#define BUTTON_PIN_HOME    32
#define BUTTON_PIN_RIGHT   25

#define BUTTON_DEBOUNCE_DELAY        50     // debounce delay
#define BUTTON_DOUBLE_CLICK_TIME     500    // double-click detection
#define BUTTON_HOLD_TIME             2000   // hold detection
#define BUTTON_COOLDOWN_TIME         1500   // cooldown period after double-click or holding

// Button States
enum ButtonState {
  BUTTON_NONE,
  BUTTON_LEFT_PRESSED,
  BUTTON_HOME_PRESSED,
  BUTTON_RIGHT_PRESSED,
  BUTTON_LEFT_DOUBLE_CLICK,
  BUTTON_HOME_DOUBLE_CLICK,
  BUTTON_RIGHT_DOUBLE_CLICK,
  BUTTON_LEFT_HOLDING,
  BUTTON_HOME_HOLDING,
  BUTTON_RIGHT_HOLDING,
  BUTTON_LEFT_RELEASED,
  BUTTON_HOME_RELEASED,
  BUTTON_RIGHT_RELEASED
};

