/*
   When morse key is pressed, hold down spacebar
   When morse key is released, release spacebar

   Start countdown when key is pressed
   if x milliseconds have passed, send single 'E' character to submit letter
*/
#include <Keyboard.h>
#include <Mouse.h>
#include <Bounce2.h>
#include <Adafruit_DotStar.h>
#include <SPI.h>

#define KEY_SPACE 0x20

// DotStar definitions
#define NUMPIXELS 1 // Number of LEDs in strip
#define DATAPIN   7
#define CLOCKPIN   8

const byte morsePin = 0;                   // Pin connected to morse key
const byte modeSelectPin = 3;              // Pin connected to selection switch (mouse vs. keyboard)

boolean timerStarted = false;              // Flag to start the countdown
boolean keyboardMode = true;               // Flag to indicate if keyboard or mouse is being emulated, some programs use one or the other
const unsigned long submissionDelay = 150; // Delay to send "e" indicating the end of the character in Submorse, ~150 to ~300 seems to work well
unsigned long releasedMillis = millis();   // Reference time for the countdown
Bounce morseButton = Bounce(morsePin, 5);  // Debounce the morse key
Adafruit_DotStar strip = Adafruit_DotStar(NUMPIXELS, DATAPIN, CLOCKPIN, DOTSTAR_BGR); // define onboard DotStar pixel


void setup() {
  // put your setup code here, to run once:
  pinMode(morsePin, INPUT_PULLUP);
  pinMode(modeSelectPin, INPUT_PULLUP);
  pinMode(LED_BUILTIN, OUTPUT);
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'

  // check for mode
  if (digitalRead(modeSelectPin) == HIGH) {
    keyboardMode = true;
    strip.setPixelColor(0, 0x001000); // green
  } else {
    keyboardMode = false;
    strip.setPixelColor(0, 0x100010); // magenta-ish
  }
  if (keyboardMode) {
    Keyboard.begin();
  } else {
    Mouse.begin();
  }

  strip.show();

}

void loop() {
  // put your main code here, to run repeatedly:
  //delay(100);
  morseButton.update();

  if (morseButton.fell())
  {
    // Key is pressed, send spacebar
    digitalWrite(LED_BUILTIN, HIGH);
    if (keyboardMode) {

      Keyboard.press(KEY_SPACE);
    } else {
      Mouse.press(MOUSE_LEFT);
    }
    // Starts keying, Reset timer
    timerStarted = false;
  }
  if (morseButton.rose()) {
    // Key released, start countdown
    if (keyboardMode) {
      Keyboard.releaseAll();
    } else {
      Mouse.release(MOUSE_LEFT);
    }
    timerStarted = true;
    releasedMillis = millis();
  }

  if (timerStarted) {
    // check if timer is ready to drop the letter
    if (millis() - releasedMillis > submissionDelay) {
      digitalWrite(LED_BUILTIN, LOW);
      timerStarted = false;

      // send 'e' to submit the letter attempt
      if (keyboardMode) {
        Keyboard.press('e');
        delay(50);
        Keyboard.releaseAll();
      }
    }
  }
}
