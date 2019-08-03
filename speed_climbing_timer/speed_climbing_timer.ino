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
#include <LiquidCrystal_I2C.h>


// Pin assignments
int TOP_LASER_PIN = 10;
int TOP_SENSOR_PIN = 9;
int FOOT_LASER_PIN = 9;
int FOOT_SENSOR_PIN = 9;

//Constants
const int IDLE      = 0;
const int READY     = 1;
const int COUNTDOWN = 2;
const int CLIMBING  = 3;

// State Variables
int current_mode = 0;
int counter = 0;
volatile bool footSwitchState = false;
volatile bool topSwitchState = false;

LiquidCrystal_I2C lcd(0x27,20,4);  // set the LCD address to 0x27 for a 16 chars and 2 line display



void changeMode(int mode){

}

// Register new switch states
void footSwitchPressedISR() { footSwitchState = true;   }
void footSwitchReleasedISR(){ footSwitchState = false;  }
void topSwitchPressedISR()  { topSwitchState = true;    }
void topSwitchReleasedISR() { topSwitchState = false;   }


void setup() {
  pinMode(LASER_SENSOR_PIN, INPUT);
  pinMode(LASER_PIN, OUTPUT);

  // Setup LCD
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0,0);
  lcd.print("Hello!");
  lcd.setCursor(0,1);
  lcd.print("Climber Timer v0.1");

  //Serial setup
  Serial.begin(115200);
  Serial.print("Climber Timer v0.1");

  // Start lasers
  digitalWrite(FOOT_LASER_PIN, HIGH);
  digitalWrite(TOP_LASER_PIN, HIGH);

  // Setup input pins
  pinMode(FOOT_SENSOR_PIN, INPUT_PULLUP);
  pinMode(TOP_SENSOR_PIN, INPUT_PULLUP);

  // Attach interrupt service routines (ISRs)
  attachInterrupt(digitalPinToInterrupt(FOOT_SENSOR_PIN), footSwitchPressedISR, FALLING);
  attachInterrupt(digitalPinToInterrupt(FOOT_SENSOR_PIN), footSwitchReleasedISR, RISING);
  attachInterrupt(digitalPinToInterrupt(TOP_SENSOR_PIN), topSwitchPressedISR, FALLING);
  attachInterrupt(digitalPinToInterrupt(TOP_SENSOR_PIN), topSwitchReleasedISR, RISING);
}

void loop() {  
}
