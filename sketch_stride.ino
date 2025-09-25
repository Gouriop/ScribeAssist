#include "USB.h"
#include "USBHIDMouse.h"

USBHIDMouse mouse;

// === Pins ===
#define VRX_PIN 14
#define VRY_PIN 13
#define JOY_SW  12

#define ENC_CLK 
#define ENC_DT  10
#define ENC_SW   5

#define LEFT_CLICK_PIN   9
#define RIGHT_CLICK_PIN 46

// === Joystick Calibration ===
#define ORIGIN_X 2808
#define ORIGIN_Y 2845
#define DEADZONE 200
#define MOVE_SPEED 2

// === State Tracking ===
bool lastJoyPressed = false;
bool lastEncPressed = false;
bool lastLeftClick = false;
bool lastRightClick = false;
int lastClk = HIGH;

void setup() {
  pinMode(JOY_SW, INPUT_PULLUP);
  pinMode(ENC_CLK, INPUT_PULLUP);
  pinMode(ENC_DT, INPUT_PULLUP);
  pinMode(ENC_SW, INPUT_PULLUP);
  pinMode(LEFT_CLICK_PIN, INPUT_PULLUP);
  pinMode(RIGHT_CLICK_PIN, INPUT_PULLUP);

  lastClk = digitalRead(ENC_CLK);

  USB.begin();
  mouse.begin();
  delay(500);
}

void loop() {
  // === Joystick Movement ===
  int xRaw = analogRead(VRX_PIN);
  int yRaw = analogRead(VRY_PIN);
  bool joyPressed = (digitalRead(JOY_SW) == LOW);

  int dx = 0, dy = 0;
  if (xRaw > ORIGIN_X + DEADZONE) dx = MOVE_SPEED;
  else if (xRaw < ORIGIN_X - DEADZONE) dx = -MOVE_SPEED;
  if (yRaw < ORIGIN_Y - DEADZONE) dy = -MOVE_SPEED;
  else if (yRaw > ORIGIN_Y + DEADZONE) dy = MOVE_SPEED;
  if (dx || dy) mouse.move(dx, dy);

  if (joyPressed && !lastJoyPressed)
    mouse.click(MOUSE_BUTTON_RIGHT);
  lastJoyPressed = joyPressed;

  // === Encoder Scroll ===
  int currentClk = digitalRead(ENC_CLK);
  if (currentClk != lastClk && currentClk == LOW) {
    if (digitalRead(ENC_DT) != currentClk)
      mouse.move(0, 0, 1);  // Scroll up
    else
      mouse.move(0, 0, -1); // Scroll down
    delay(5);
  }
  lastClk = currentClk;

  // === Encoder Button: Left Click ===
  bool encPressed = digitalRead(ENC_SW) == LOW;
  if (encPressed && !lastEncPressed) {
    mouse.click(MOUSE_BUTTON_LEFT);
    delay(300);
  }
  lastEncPressed = encPressed;

  // === External Left Click ===
  bool leftClick = digitalRead(LEFT_CLICK_PIN) == LOW;
  if (leftClick && !lastLeftClick)
    mouse.click(MOUSE_BUTTON_LEFT);
  lastLeftClick = leftClick;

  // === External Right Click ===
  bool rightClick = digitalRead(RIGHT_CLICK_PIN) == LOW;
  if (rightClick && !lastRightClick)
    mouse.click(MOUSE_BUTTON_RIGHT);
  lastRightClick = rightClick;

  delay(2);
}
