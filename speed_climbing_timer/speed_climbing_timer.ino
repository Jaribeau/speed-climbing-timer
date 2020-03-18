////////////////////////////////////////////////
// Speed Climbing Timer
// Author: Jared Baribeau
////////////////////////////////////////////////

// Potential improvements
// - Experiment with using an external speaker instead of a tiny amp + speaker to get more volume
// - Add LEDs to top switch to indicate when it's been pressed
// - Calculate timer precision (and change over to using interrupts if extreme precision is desired)
// - Add large LCD screen
// - Add ability to connect a second timer, for side-by-side use

#include <Wire.h>
#include <hd44780.h>                       // main hd44780 header
#include <hd44780ioClass/hd44780_I2Cexp.h> // i2c expander i/o class header

// LCD
hd44780_I2Cexp lcd; // declare lcd object: auto locate & auto config expander chip
const int LCD_COLS = 16;
const int LCD_ROWS = 2;

// Pin assignments
int TOP_SENSOR_PIN = 14;
int FOOT_SENSOR_PIN = 10;
int SPEAKER_PIN = 2;

//Constants
const int READY     = 1;
const int COUNTDOWN = 2;
const int CLIMBING  = 3;
long beepInterval = 1000;
long DEBOUNCE_TIME = 500; // milliseconds

// State Variables
int currentMode = 0;
int counter = 0;
volatile bool topSwitchStateChanged = false;
volatile long topSwitchPressedTime = 0;
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
      countdownStep = 0;
      break;

    case CLIMBING:
      timer = millis();
      topSwitchPressedTime = 0;
      lcd.clear();
      lcd.print(timer);
      lcd.setCursor(0,1);
      lcd.print("CLIMBING");
      lcd.noBlink();
      break;
  }
}

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

  //Setup input pins
  pinMode(TOP_SENSOR_PIN, INPUT_PULLUP);
  pinMode(FOOT_SENSOR_PIN, INPUT_PULLUP);

  delay(2000);
  changeMode(READY);
}

void loop() {
  switch (currentMode)
  {
    case READY:
      // Pulse led on footswitch
      // watch for footswitch press
      if(digitalRead(FOOT_SENSOR_PIN)){
        changeMode(COUNTDOWN);
        footswitchDebounceTimer = millis();
      }
      break;

    case COUNTDOWN:
      if (millis() - countdownTimer < 5000){
        if(countdownStep != 1){
          tone(SPEAKER_PIN, 400, 100);
          lcd.clear();
          lcd.print("Get ready...");
        }
        countdownStep = 1;
      }
      else if (millis() - countdownTimer < 5000 + beepInterval){
        if(countdownStep != 2){
          tone(SPEAKER_PIN, 200, 500);
          lcd.clear();
          lcd.print("Beep 1");
        }
        countdownStep = 2;
      }
      else if (millis() - countdownTimer < 5000 + beepInterval*2){
        if(countdownStep != 3){
          tone(SPEAKER_PIN, 200, 500);
          lcd.clear();
          lcd.print("Beep 2");
        }
        countdownStep = 3;
      }
      else if (millis() - countdownTimer >= 5000 + beepInterval*2){
        tone(SPEAKER_PIN, 300, 500);
        lcd.clear();
        lcd.print("BEEEEP 3. GO!");
        changeMode(CLIMBING);
      }

      //  -> If feet release: LED RED and BUZZ
      if(!digitalRead(FOOT_SENSOR_PIN) && (millis() - footswitchDebounceTimer) > DEBOUNCE_TIME){
        if (millis() - countdownTimer > 5000 ){
          lcd.clear();
          lcd.print("FOOT FAULT");

          // Buzzer
          tone(SPEAKER_PIN, 100, 100);
          delay(200);
          tone(SPEAKER_PIN, 100, 100);
          delay(200);
          tone(SPEAKER_PIN, 100, 100);

          delay(2000);
          changeMode(READY);
        }
        else{
          tone(SPEAKER_PIN, 200, 50);
          changeMode(READY);
        }
      }
      break;

    case CLIMBING:
      // Show timer counting up
      if(millis() - printTimer > printFreq){
        printTimer = millis();
        lcd.setCursor(0,0);
        lcd.print((double)(millis() - timer) / 1000.0);
      }

      // Press footswitch to reset after a few seconds (in case climber doesn't finish)
      if(digitalRead(FOOT_SENSOR_PIN) && (millis() - timer) >= 5000 ){
        changeMode(READY);
        tone(SPEAKER_PIN, 200, 50);
      }
      
      // Record time that top switch is pressed
      if(topSwitchPressedTime == 0 && digitalRead(TOP_SENSOR_PIN))
        topSwitchPressedTime = millis();

      // Celebrate successful climb, etc
      if(topSwitchPressedTime != 0){
        lastTime = (double)(topSwitchPressedTime - timer) / 1000.0;
        lcd.setCursor(0,0);
        lcd.print(lastTime);
        lcd.setCursor(0,1);
        lcd.print("FINISHED!");

        noTone(SPEAKER_PIN);
        tone(SPEAKER_PIN, 600, 50);
        delay(100);
        tone(SPEAKER_PIN, 800, 50);
        delay(100);
        tone(SPEAKER_PIN, 800, 50);
        delay(100);
        tone(SPEAKER_PIN, 800, 50);
        delay(100);

        delay(3000);
        changeMode(READY);
      }
      break;
  }
}
