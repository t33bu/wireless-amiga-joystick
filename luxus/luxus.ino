/*
 * luxus.ino
 *
 * Created on: 25.12.2017
 * Author: Teemu Leppänen (tjlepp@gmail.com)
 *
 * This work is licensed under Creative Commons Attribution-NonCommercial-ShareAlike (CC BY-NC-SA 4.0)
 * https://creativecommons.org/licenses/by-nc-sa/4.0/
 * 
 */

#include <SPI.h>
#include <Wire.h>
#include <TimerOne.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH1106.h>

// **************************************************
#define UPDATE_DELAY  10 // delay 10ms

#define JOY_AUTO      A2
#define JOY_DOWN      10
#define JOY_LEFT      11
#define JOY_RIGHT     12
#define JOY_UP        13  // Led pin: should be changed due to lower voltage
#define BUTTON_IN     4
#define BUTTON_OUT    2

#define OLED_CLK      9
#define OLED_MOSI     8
#define OLED_RESET    7
#define OLED_DC       6
#define OLED_CS       5
Adafruit_SH1106 display(OLED_MOSI, OLED_CLK, OLED_DC, OLED_RESET, OLED_CS);

#define STR_POS       88

boolean invert=false;
boolean flash=false;
volatile boolean autoshoot=false;

uint8_t joystick_state=0;
uint16_t autofire=0, old_autofire=0;
uint32_t autoperiod=0, shoot_count=0;
char shoot_str[16];

// **************************************************
void autoShoot() {

  autoshoot=true;
}

// **************************************************
void setup()   {                

  pinMode(JOY_DOWN, INPUT);          
  pinMode(JOY_UP, INPUT);          
  pinMode(JOY_LEFT, INPUT);          
  pinMode(JOY_RIGHT, INPUT);          

  pinMode(BUTTON_IN, INPUT);          
  digitalWrite(BUTTON_IN,LOW);
  pinMode(BUTTON_OUT, OUTPUT);          
  digitalWrite(BUTTON_OUT,HIGH);
  pinMode(JOY_AUTO, INPUT);          
  digitalWrite(JOY_AUTO,LOW);
  
  display.begin(SH1106_SWITCHCAPVCC);
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.clearDisplay();
  display.display();  

  Timer1.initialize(1000000L);         
  Timer1.stop();

}
 
// **************************************************
void loop() {

  // If display inverted, return to normal state
  if (invert==true) {
    display.invertDisplay(false);
    invert=false;
  }

  // Set normal state
  joystick_state=0;
  display.clearDisplay();
  display.setCursor(STR_POS,0);
  sprintf(shoot_str,"%06d",shoot_count);
  display.println(shoot_str);

  // Autofire
  autofire = analogRead(JOY_AUTO); 
  autofire /= 100;
  if (old_autofire != autofire) {

    Timer1.stop();
    if (autofire != 0) {

      autoperiod = (float)1/(float)autofire*1000000L; 
      Timer1.attachInterrupt(autoShoot,autoperiod);
      Timer1.restart();
      
      old_autofire = autofire;      
    }
  }
  
  // Shooting?
  if (autoshoot == true || digitalRead(BUTTON_IN) == 1) {
    digitalWrite(BUTTON_OUT,LOW);   
    autoshoot=false;
    flash=true;        
  } else {
    
    digitalWrite(BUTTON_OUT,HIGH);

    // Update shoot count when low state ends
    if (flash == true) { 
      flash=false;        
      shoot_count++;
    }    
  }

  // Movement?
  if (digitalRead(JOY_UP) == 0) {
    display.fillRect(0,8,128,16,WHITE);    
  } 
  else if (digitalRead(JOY_DOWN) == 0) {
    display.fillRect(0,40,128,16,WHITE);    
  } 
  else if (digitalRead(JOY_RIGHT) == 0) {
    display.fillRect(80,0,32,64,WHITE);    
  } 
  else if (digitalRead(JOY_LEFT) == 0) {
    display.fillRect(16,0,32,64,WHITE);    
  } 
  
  // Update display
  if (flash==true) {
    display.invertDisplay(true);
    invert=true;
  }  
  display.display();

  // Wait to show stuff on the screen
  delay(UPDATE_DELAY);     
}



