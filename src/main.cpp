#include <Arduino.h>
#include <Encoder.h>
#include <LiquidCrystal_I2C.h>

#define encoder_pin_a 2
#define encoder_pin_b 3
#define a_sec 1000000

// LCD Address is 0x27
LiquidCrystal_I2C lcd(0x27, 16, 2);

Encoder encoder(encoder_pin_a, encoder_pin_b);
unsigned long encoder_timer = 0;

float encoder_new = 0;
float encoder_old = 0;
float frequency = 0;
unsigned int on_time = 100;
boolean is_on = false;
unsigned long timer;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(5, OUTPUT);
  // Turn off pin 5
  PORTD = PORTD & B11011111;
  encoder_new = encoder.read();
  encoder_old = encoder.read();
  timer = micros();
  encoder_timer = millis();

  lcd.init();
  lcd.clear();         
  lcd.backlight();
  lcd.setCursor(0,0);   //Set cursor to character 2 on line 0
  lcd.print("Frequency:");
  lcd.print(frequency);
  
  lcd.setCursor(0,1);   //Move cursor to character 2 on line 1
  lcd.print("RPM:");
  lcd.print(frequency*60);
}

void loop() {
  // Get encoder data
  float encoder_new = float(encoder.read())/8;
  if (encoder_new != encoder_old && millis() - encoder_timer > 100) {
    float difference = encoder_new - encoder_old;
    
    frequency = frequency + pow(difference, 3);
    encoder_old = encoder_new;

  if(frequency < 0 || frequency > 2000 || encoder_new < 0){
    frequency = 0; 
    encoder_new = 0; 
    encoder_old = 0;
  }
    lcd.setCursor(0,0);   //Set cursor to character 2 on line 0
    lcd.print("Frequency:");
    lcd.print(frequency);
  
    lcd.setCursor(0,1);   //Move cursor to character 2 on line 1
    lcd.print("RPM:");
    lcd.print(frequency*60);

    encoder_timer = millis();
  }

  if(frequency != 0){
    if(is_on == true){
      // Wait a set amount of time and then turn the LED off
      if(micros()-timer >= on_time){
        PORTD = PORTD & B11011111;
      }
      // wait the rest of the off period then reset the clock
      if((micros()-timer) >= a_sec/frequency - on_time){
        is_on = false;
        timer = micros();
      }
    }
    else{
      // Turn on the LED
      PORTD = PORTD | B00100000;
      is_on = true;
    }
  }
}