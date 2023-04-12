/*
   When morse key is pressed, hold down spacebar
   When morse key is released, release spacebar

   Start countdown when key is pressed
   if x milliseconds have passed, send single 'E' character to submit letter
*/
#include <Keyboard.h>
#include <Bounce2.h>

#define KEY_SPACE 0x20

const byte morsePin = 0;                   // Pin connected to morse key
boolean timerStarted = false;              // Flag to start the countdown
const unsigned long submissionDelay = 150; // Delay to send "e" indicating the end of the character in Submorse, ~150 to ~300 seems to work well
unsigned long releasedMillis = millis();   // Reference time for the countdown
Bounce morseButton = Bounce(morsePin, 5);  // Debounce the morse key

void setup() {
  // put your setup code here, to run once:
  pinMode(morsePin, INPUT_PULLUP);
  pinMode(LED_BUILTIN, OUTPUT);
  Keyboard.begin();
}

void loop() {
  // put your main code here, to run repeatedly:
  //delay(100);
  morseButton.update();

  if (morseButton.fell())
  {
    // Key is pressed, send spacebar
    digitalWrite(LED_BUILTIN, HIGH);
    Keyboard.press(KEY_SPACE);

    // Starts keying, Reset timer
    timerStarted = false;
  }
  if (morseButton.rose()) {
    // Key released, start countdown
    Keyboard.releaseAll();
    timerStarted = true;
    releasedMillis = millis();
  }
  
  if (timerStarted) {
    // check if timer is ready to drop the letter
    if (millis() - releasedMillis > submissionDelay) {
      digitalWrite(LED_BUILTIN, LOW);
      timerStarted = false;

      // send 'e' to submit the letter attempt
      Keyboard.press('e');
      delay(50);
      Keyboard.releaseAll();
    }
  }
}
