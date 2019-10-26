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

////////////////////////////////////////////////
// Modes
////////////////////////////////////////////////
// IDLE
//  -> Show last score
//  -> Press start

// READY
//  -> Timer set to zero
//  -> Footswitch LED slow blinking (fading in/out?)
//  -> "Step on PAD to start countdown"

// COUNTDOWN
//  -> Footswitch ON
//  -> Double beep to signal countdown coming
//  -> 5 second delay
//  -> Beep beep BEEP
//  -> If feet release: LED RED and BUZZ

// CLIMBING
//  -> Start timer (display on screen)
//  -> Stop time when lazer hit (leave on screen)
//  -> Back to IDLE

/*
TODO:
- Plan out mode switching
- Plan out LED control in the main loop
 */

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
const int IDLE      = 0;
const int READY     = 1;
const int COUNTDOWN = 2;
const int CLIMBING  = 3;

// State Variables
int currentMode = 0;
int counter = 0;
volatile bool footSwitchState = false;
volatile bool topSwitchState = false;
volatile bool stateChangedFlag = true;
long timer = 0;


void changeMode(int mode_){
  switch (currentMode)
  {
    // IDLE case maybe not necessary?
    case IDLE:
      // Show last score
      lcd.clear();
      lcd.print("Time: ");
      lcd.print(timer);
      lcd.setCursor(0,1);
      lcd.print("Press START.");
      lcd.noBlink();
      break;

    case READY:
      // "Beep"
      lcd.clear();
      lcd.print("00000");
      lcd.setCursor(0,1);
      lcd.print("Step on PAD to start countdown...");
      lcd.blink();
      break;

    case COUNTDOWN:
      //  -> Footswitch ON
      //  -> Double beep to signal countdown coming
      //  -> 5 second delay
      //  -> Beep beep BEEP
      //  -> If feet release: LED RED and BUZZ
      break;

    case CLIMBING:
      timer = millis();
      lcd.clear();
      lcd.print(timer);
      lcd.setCursor(0,1);
      lcd.print("CLIMBING");
      //  -> Start timer (display on screen)
      //  -> Stop time when lazer hit (leave on screen)
      //  -> Back to IDLE
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
  changeMode(IDLE);
}

void loop() {  
  switch (currentMode)
  {
    // IDLE case maybe not necessary?
    case IDLE:
      // Watch for start button press
      if(!digitalRead(START_BTN_PIN)){
        changeMode(COUNTDOWN);
      }
      break;

    case READY:
      // Pulse led on footswitch
      // watch for footswitch press
      if(!digitalRead(FOOT_SENSOR_PIN)){
        changeMode(COUNTDOWN);
      }
      break;

    case COUNTDOWN:
      //  -> Footswitch ON

      //  -> Double beep to signal countdown coming
      //  -> 5 second delay
      //  -> Beep beep BEEP

      //  -> If feet release: LED RED and BUZZ
      break;

    case CLIMBING:
      /* code */
      break;
  }
}
