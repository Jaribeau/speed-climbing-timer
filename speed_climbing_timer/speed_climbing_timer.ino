////////////////////////////////////////////////
// Layout
////////////////////////////////////////////////
// Top: 
//  12v, 5v, GND
//  1x Laser
//  1x light-sensor
//  LEDs (TBD if digital IO or laser triggered)
//  Large LCD?
// Foot:
//  1x Laser
//  1x light-sensor
//  1x LEDs

// TODO
// - Add sound
// - Add LEDs
// - (?) Add highscores

#include <Wire.h>
#include <hd44780.h>                       // main hd44780 header
#include <hd44780ioClass/hd44780_I2Cexp.h> // i2c expander i/o class header

// LCD
hd44780_I2Cexp lcd; // declare lcd object: auto locate & auto config expander chip
const int LCD_COLS = 16;
const int LCD_ROWS = 2;

// Pin assignments
int TOP_LASER_PIN = 8;
int TOP_SENSOR_PIN = 1;
int FOOT_LASER_PIN = 9;
int FOOT_SENSOR_PIN = 7;
int START_BTN_PIN = 10;

//Constants
const int READY     = 1;
const int COUNTDOWN = 2;
const int CLIMBING  = 3;
long beepInterval = 1000;
long DEBOUNCE_TIME = 500; // milliseconds

// State Variables
int currentMode = 0;
int counter = 0;
volatile bool footSwitchState = false;
volatile bool topSwitchState = false;
volatile bool stateChangedFlag = true;
long timer = 0;
double lastTime = 0.0;
long countdownTimer = 0;
long footswitchDebounceTimer = 0;
long printTimer = 0;
int printFreq = 100;
int countdownStep = 0;

void changeMode(int mode_){
  currentMode = mode_;
  switch (currentMode)
  {
    case READY:
      // "Beep"
      lcd.clear();
      lcd.print("Last: ");
      lcd.print(lastTime);
      lcd.setCursor(0,1);
      lcd.print("Step on PAD...");
      lcd.blink();
      break;

    case COUNTDOWN:
      lcd.clear();
      countdownTimer = millis();
      //  -> Footswitch ON
      //  -> Double beep to signal countdown coming
      //  -> 5 second delay
      //  -> If feet release: LED RED and BUZZ
      break;

    case CLIMBING:
      timer = millis();
      lcd.clear();
      lcd.print(timer);
      lcd.setCursor(0,1);
      lcd.print("CLIMBING");
      lcd.noBlink();
      break;
  }
}

// Register new switch states
void footSwitchPressedISR() { footSwitchState = true, stateChangedFlag = true; }
void footSwitchReleasedISR(){ footSwitchState = false, stateChangedFlag = true; }
void topSwitchPressedISR()  { topSwitchState = true, stateChangedFlag = true; }
void topSwitchReleasedISR() { topSwitchState = false, stateChangedFlag = true; }


void setup() {
  // Setup LCD
  int status;
	status = lcd.begin(LCD_COLS, LCD_ROWS);
	if(status) // non zero status means it was unsuccesful
	{
		status = -status; // convert negative status value to positive number
		// begin() failed so blink error code using the onboard LED if possible
		hd44780::fatalError(status); // does not return
	}
	// initalization was successful, the backlight should be on now

	// Print a message to the LCD
  lcd.lineWrap();
  lcd.print("Climber Timer");
  // Serial.begin(115200);
  // Serial.print("Climber Timer v0.1");

  //Setup input pins
  pinMode(TOP_SENSOR_PIN, INPUT_PULLUP);
  pinMode(FOOT_SENSOR_PIN, INPUT_PULLUP);
  pinMode(START_BTN_PIN, INPUT_PULLUP);

  // Start lasers
  digitalWrite(FOOT_LASER_PIN, HIGH);
  digitalWrite(TOP_LASER_PIN, HIGH);

  // Attach interrupt service routines (ISRs)
  attachInterrupt(digitalPinToInterrupt(FOOT_SENSOR_PIN), footSwitchPressedISR, FALLING);
  attachInterrupt(digitalPinToInterrupt(FOOT_SENSOR_PIN), footSwitchReleasedISR, RISING);
  attachInterrupt(digitalPinToInterrupt(TOP_SENSOR_PIN), topSwitchPressedISR, FALLING);
  attachInterrupt(digitalPinToInterrupt(TOP_SENSOR_PIN), topSwitchReleasedISR, RISING);
  
  delay(3000);
  changeMode(READY);
}

void loop() {  
  // lcd.print(!digitalRead(FOOT_SENSOR_PIN));
  switch (currentMode)
  {
    case READY:
      // Pulse led on footswitch
      // watch for footswitch press
      if(!digitalRead(FOOT_SENSOR_PIN)){
        changeMode(COUNTDOWN);
        footswitchDebounceTimer = millis();
      }
      break;

    case COUNTDOWN:
      if (millis() - countdownTimer < 5000){
        if(countdownStep != 1){
          lcd.clear();
          lcd.print("Get ready...");
        }
        countdownStep = 1;
      }
      else if (millis() - countdownTimer < 5000 + beepInterval){
        if(countdownStep != 2){
          lcd.clear();
          lcd.print("Beep 1");
        }
        countdownStep = 2;
      }
      else if (millis() - countdownTimer < 5000 + beepInterval*2){
        if(countdownStep != 3){
          lcd.clear();
          lcd.print("Beep 2");
        }
        countdownStep = 3;
      }
      else if (millis() - countdownTimer >= 5000 + beepInterval*2){
        lcd.clear();
        lcd.print("BEEEEP 3. GO!");
        changeMode(CLIMBING);
      }

      //  -> If feet release: LED RED and BUZZ
      if(digitalRead(FOOT_SENSOR_PIN) && (millis() - footswitchDebounceTimer) > DEBOUNCE_TIME){
        lcd.clear();
        lcd.print("FOOT FAULT");
        countdownStep = 0;
        delay(3000);
        changeMode(READY);
      }
      break;

    case CLIMBING:
      if(millis() - printTimer > printFreq){
        printTimer = millis();
        lcd.setCursor(0,0);
        lcd.print((double)(millis() - timer) / 1000.0);
      }
      
      if(!digitalRead(TOP_SENSOR_PIN)){
        lastTime = (double)(millis() - timer) / 1000.0;
        lcd.setCursor(0,0);
        lcd.print(lastTime);
        lcd.setCursor(0,1);
        lcd.print("FINISHED!");
        delay(5000);
        changeMode(READY);
      }
      break;
  }
}
