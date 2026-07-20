#include <Servo.h>

const uint8_t SERVO_PIN  = 10;
const uint8_t BUTTON_PIN = 3;

Servo myServo;
bool servoAt180 = false;
bool lastButtonState = HIGH;

const unsigned long SERVO_MOVE_DELAY = 0;



void setup() {
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  myServo.attach(SERVO_PIN, 550, 2550);
  myServo.write(0);
 // myServo.detach();
}

void loop() {
  bool currentButtonState = digitalRead(BUTTON_PIN);

  // ==========================================================
  // 1. BUTTON PRESS START (High -> Low)
  // ==========================================================
  if (lastButtonState == HIGH && currentButtonState == LOW) {
    delay(20); // Debounce

    delay(SERVO_MOVE_DELAY); // Wait before spinning
    toggleServo();
  }

  // ==========================================================
  // 2. BUTTON RELEASE (Low -> High)
  // ==========================================================
  if (lastButtonState == LOW && currentButtonState == HIGH) {
    delay(20); // Debounce
  }

  lastButtonState = currentButtonState;
}

void toggleServo() {
  //myServo.attach(SERVO_PIN, 550, 2550);

  servoAt180 = !servoAt180;
  if (servoAt180) {
    myServo.write(180);
  } else {
    myServo.write(0);
  }
 // myServo.detach();
}
