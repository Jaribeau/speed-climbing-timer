
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <Mouse.h>


//
// Pin assignments
//
int LASER_PIN = 10;
int LASER_SENSOR_PIN = 9;
int POT_PIN = A0;

// Variables
int counter = 0;
int last_position = 0;
bool already_incremented = false;

// Mouse
int prev_scroller_in = 0;
long mouse_delay_timer = 0;
long mouse_delay = 10;

LiquidCrystal_I2C lcd(0x27,20,4);  // set the LCD address to 0x27 for a 16 chars and 2 line display


void setup() {
  pinMode(LASER_SENSOR_PIN, INPUT);
  pinMode(LASER_PIN, OUTPUT);
  pinMode(POT_PIN, INPUT);

  // Setup LCD
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0,0);
  lcd.print("Hello!");
  lcd.setCursor(0,1);
  lcd.print("Midi Pedal v0.1");


  //Serial setup
  Serial.begin(115200);
  Serial.print("BtnAddr:");

  // Start laser
  digitalWrite(10, HIGH);

  // Setup Mouse
  Mouse.begin();
  last_position = analogRead(POT_PIN);
}

void loop() {  
  if(digitalRead(LASER_SENSOR_PIN) && !already_incremented){
    counter++;
    already_incremented = true;
  }
  else if(!digitalRead(LASER_SENSOR_PIN) && already_incremented){
    already_incremented = false; 
    Serial.println("Mouse off!");
  }

////  Mouse.move(0,0,1);
//  delay(100);
//  Mouse.move(0,0, (char)(last_position - analogRead(POT_PIN)/8));
//  last_position = analogRead(POT_PIN)/8;


    ///////////////////
  // MOUSE SCROLL WHEEL
  ///////////////////
  if(millis() - mouse_delay_timer > mouse_delay){
    mouse_delay_timer = millis();
    Mouse.move(0,0, (char)(prev_scroller_in - analogRead(POT_PIN)/8));
    prev_scroller_in = analogRead(POT_PIN)/8;
  }
}
