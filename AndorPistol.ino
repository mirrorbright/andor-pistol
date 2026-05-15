#include <Servo.h>
#include <SoftwareSerial.h>
#include "Adafruit_Soundboard.h"

// Pins for Serial communication
#define SFX_TX 7
#define SFX_RX 8
#define SFX_RST 9
#define SFX_ACT 12

const uint8_t SERVO_PIN  = 10;
const uint8_t BUTTON_PIN = 3;
const uint8_t SFX_BLASTER = 11;
const uint8_t SFX_LIZARD  = 12;

Servo myServo;
bool servoAt180 = false;
bool lastButtonState = HIGH;

// Timing variables
unsigned long lastButtonPressTime = 0;
bool isTimerRunning = false;
unsigned long lastActivityTime = 0; // When did the user STOP interacting?
bool isServoTimerRunning = false;
const unsigned long SERVO_MOVE_DELAY = 2000; 

// --- Sound Variables ---
unsigned long pressStartTime = 0;   // When did the current press start?
bool isPressed = false;
bool longPressTriggered = false;    // Did we already play the honk during this hold?
const unsigned long HOLD_TIME = 2000; // Time to trigger long press sound
int soundToPlay = SFX_BLASTER; //Which sound to play, start with Blaster
uint8_t randNumber;
char fileName[20] ;

// Connect via SoftwareSerial
SoftwareSerial ss = SoftwareSerial(SFX_TX, SFX_RX);
Adafruit_Soundboard sfx = Adafruit_Soundboard(&ss, NULL, SFX_RST);



void setup() {
  // 1. Start Serial Monitor (Your Computer)
  //Serial.begin(115200); 

  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(SFX_ACT, INPUT_PULLUP);

  myServo.attach(SERVO_PIN, 550, 2550);
  myServo.write(0);
 // myServo.detach();
  
  randomSeed(analogRead(A0)); 
  
  //Serial.println("--- Soundboard Debugger Starting ---");

  ss.begin(9600);
  
//Serial.println("Resetting Soundboard...");
  if (!sfx.reset()) {
    //Serial.println("ERROR: Soundboard NOT found. Check RX/TX and Power!");
    while (1); 
  }
  //Serial.println("Soundboard Found and Ready!");

}

void loop() {
  bool currentButtonState = digitalRead(BUTTON_PIN);
  unsigned long currentTime = millis();

  // ==========================================================
  // 1. BUTTON PRESS START (High -> Low)
  // ==========================================================
  if (lastButtonState == HIGH && currentButtonState == LOW) {
    //Serial.println("Button Pressed!");
    isPressed = true;
    pressStartTime = currentTime;
    longPressTriggered = false; // Reset this flag for the new press
    
    // Reset the Servo timer because user is interacting again
    isServoTimerRunning = false; 
        
    delay(20); // Debounce
  }

  // ==========================================================
  // 2. WHILE HOLDING (Checking for Long Press)
  // ==========================================================
  if (currentButtonState == LOW) {
    // Check if we have held it long enough ti switch time banks
    if ((currentTime - pressStartTime >= HOLD_TIME) && !longPressTriggered) {
      switchSound(soundToPlay);

      longPressTriggered = true; // Ensure we don't spam the honk command
    }
    
    // Keep resetting the servo timer while holding
    lastActivityTime = currentTime; 
  }

  // ==========================================================
  // 3. BUTTON RELEASE (Low -> High)
  // ==========================================================
  if (lastButtonState == LOW && currentButtonState == HIGH) {
    isPressed = false;
    
    // If we released the button AND we never triggered the long press...
    // That means it was a SHORT press. Play sound.
    if (!longPressTriggered) {
      triggerSound(soundToPlay);
    }

    // Start the Servo countdown now that the button is released
    lastActivityTime = currentTime;
    isServoTimerRunning = true;
    
    delay(20); // Debounce
  }

  // ==========================================================
  // 4. SERVO MOVEMENT (The "Cooldown" Logic)
  // ==========================================================
  if (isServoTimerRunning) {
    if (currentTime - lastActivityTime >= SERVO_MOVE_DELAY) {
      isServoTimerRunning = false; // Stop checking
      
      toggleServo();
    }
  }

  lastButtonState = currentButtonState;
}



// Helper function to trigger the Adafruit FX board
void triggerSound(int pin) {
  myServo.detach();  // prevent jitter during serial comms
  
  safeStop();
  
  if (soundToPlay == SFX_BLASTER) {
    randNumber = random(1, 7);
    snprintf(fileName, sizeof(fileName), "BLASTER%dWAV", (int)randNumber);
    //Serial.println("Playing...");
    //Serial.println(fileName);
    sfx.playTrack((char*)fileName);
  } else {
    sfx.playTrack("LIZZZARDWAV");
    //Serial.println("Playing LIZZZARD...");
  }

  myServo.attach(SERVO_PIN, 550, 2550);
  myServo.write(servoAt180 ? 180 : 0);  // reinforce where it already is
  delay(20);
}

void safeStop() {
  if (digitalRead(SFX_ACT) == LOW) {  // LOW means audio IS playing
    sfx.stop();
    delay(20);
  }
}

void switchSound(int pin) {
  if (pin == SFX_BLASTER) {
    soundToPlay = SFX_LIZARD;
  } else {
    soundToPlay = SFX_BLASTER;
  } 
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
