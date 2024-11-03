

#include <Bounce2.h>
#include <Arduino.h>
// Define pin connections & variables
const int dataPin = 14;   // Connect to the data input of your shift register
const int latchPin = 15;  // Connect to the latch pin
const int clockPin = 16;  // Connect to the clock pin
#define BUTTON_PIN 11     // Connect to  push-button switch
Bounce2::Button button = Bounce2::Button();
int pressNumber = 0;
int firstDigit, secondDigit, currentRandonNumber;
int roundNumber = 1;

void setup() {
  pinMode(dataPin, OUTPUT);
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  Serial.println("Round #1...");
  button.attach(BUTTON_PIN, INPUT);  // got from https://github.com/thomasfredericks/Bounce2?tab=readme-ov-file, the bounce2 library example
  button.interval(5);                // DEBOUNCE INTERVAL IN MILLISECONDS
  button.setPressedState(HIGH);      // INDICATE THAT THE HIGH STATE CORRESPONDS TO PHYSICALLY PRESSING THE BUTTON
}

//data for shift register
byte digits[10] = {
  B11111100,  // 0
  B01100000,  // 1
  B11011010,  // 2
  B11110010,  // 3
  B01100110,  // 4
  B10110110,  // 5
  B10111110,  // 6
  B11100000,  // 7
  B11111110,  // 8
  B11110110   // 9
};


void loop() {

  button.update();
  currentRandonNumber = random(10);
  displayDigit(currentRandonNumber);
  delay(500);
  if (button.pressed()) {
    pressNumber++;
    //first round
    if (pressNumber == 1) {
      firstDigit = currentRandonNumber;
      Serial.print("PBS press #");
      Serial.print(pressNumber);
      Serial.print(" ==> ");
      Serial.println(firstDigit);

      //second round
    } else if (pressNumber >= 2) {
      secondDigit = currentRandonNumber;
      Serial.print("PBS press #");
      Serial.print(pressNumber);
      Serial.print(" ==> ");
      Serial.println(secondDigit);
      checkMatch();
      roundNumber++;
      Serial.print("Round #");
      Serial.print(roundNumber);
      Serial.println("...");
      pressNumber = 0;  // Reset for next round
    }
    displayDigit(currentRandonNumber);  // Freeze the digit on the display
    delay(2000);                        // Freeze for 2 seconds
  }
}

//check if the round 1 and round 2 number match
void checkMatch() {
  if (firstDigit == secondDigit) {
    Serial.println("=> **MATCH**");
    blinkDigit(firstDigit);
  } else {
    Serial.println("=> NO Match... period.");
    showDecimalPoint();
  }
  Serial.println(" ");
}

//show decimal point only -  Turn off other segments
void showDecimalPoint() {
  //   DP is the last segment (pin 7 for Shift register)
  digitalWrite(latchPin, LOW);
  shiftOut(dataPin, clockPin, MSBFIRST, B10000000 ^ B11111111);  // Light up the decimal point
  digitalWrite(latchPin, HIGH);
  delay(2000);
  clearDisplay();
}

//displayt the number on 7 segment display
void displayDigit(int digit) {
  digitalWrite(latchPin, LOW);
  shiftOut(dataPin, clockPin, LSBFIRST, (digits[digit]) ^ B11111111);
  digitalWrite(latchPin, HIGH);
}

//blink the current number for 2 seconds (4*(250+250) millisseconds)
void blinkDigit(int digit) {
  for (int i = 0; i < 4; i++) {
    displayDigit(digit);
    delay(250);
    clearDisplay();
    delay(250);
  }
}

//clear display- turn off all segments
void clearDisplay() {
  digitalWrite(latchPin, LOW);
  shiftOut(dataPin, clockPin, MSBFIRST, B00000000 ^ B11111111);  // Turn off all segments
  digitalWrite(latchPin, HIGH);
}
