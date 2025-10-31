#include "oled_SD1306.h"
//#include "tip_profiles.h"

 void setup_OLED_display(Adafruit_SSD1306 &display, const int sda, const int scl){
  Wire.begin(sda, scl);
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
  display.clearDisplay();
}


void update_OLED_display(Adafruit_SSD1306 &display, const float Setpoint, const float Input, const float Output, const float max_output, const unsigned long msNow, const unsigned long windowStartTime, const unsigned long windowSize, const unsigned int lastSetpointChangeTime){
  if(abs((int) msNow  - (int)lastSetpointChangeTime) < 2000) {
    show_set_temperature(display, Setpoint);
    return;
  }
  if( msNow - windowStartTime <= (windowSize >> 1) ) {
    return;
  }
  show_actual_temperature(display, Input);
  return;
}



void update_debug_OLED_display(Adafruit_SSD1306 &display, const float Setpoint, const float Input, const float Output, const float max_output, const unsigned long msNow, const unsigned long windowStartTime, const unsigned long windowSize, const TipProfile &activeTip){
  if( msNow - windowStartTime >= (windowSize >> 1) ) {
    int input = (int) round(Input);
    int output = (int) round(Output * 100 / max_output);
    display.clearDisplay();
    display.setTextSize(2);      
    display.setTextColor(WHITE); 
    display.setCursor(0,0); 
    display.print("Set: "); 
    display.print((int) round(Setpoint)); 
    display.println(" C"); 
    if(input < 10) {
      display.print("Now:   ");
      display.print(input); 
    } else if(input < 100) {
      display.print("Now:  ");
      display.print(input); 
    } else {
      display.print("Now: ");
      display.print(input); 
    }
    display.println(" C");
    if(output < 10) {
      display.print("Pwr:   ");
      display.print(output); 
    } else if(output < 100) {
      display.print("Pwr:  ");
      display.print(output); 
    } else {
      display.print("Pwr: ");
      display.print(output); 
    }
    display.println("%"); 

    int16_t x1,y1;
    uint16_t w1,h1;
    display.getTextBounds(activeTip.name, 0,0, &x1, &y1, &w1, &h1);

    display.setTextSize(1.5);
    display.setCursor(SCREEN_WIDTH - w1, 68-12);
    display.print(activeTip.name);
    display.display();
  }
}



void show_temperature(Adafruit_SSD1306 &display, const bool is_actual, const float temperature){
  display.clearDisplay();
  display.setTextColor(WHITE); 
  display.setTextSize(1.9);
  display.setCursor(0,0); 
  if(is_actual)
    display.print("  Now "); 
  else
    display.print("  Set "); 
  // display.print((char)247); 
  // display.println("C:");

  display.setTextSize(5);      
  display.setCursor(10,15);
  int temperature_int = (int) round(temperature);
  if(temperature_int < 10) {
    display.print("  ");
  } else if(temperature_int < 100) {
    display.print(" ");
  } 
  display.print(temperature_int); 
  display.setTextSize(2);      
  display.print((char)247); 
  display.print("C");
  display.display();
}

void show_set_temperature(Adafruit_SSD1306 &display, const float Setpoint){
    show_temperature(display, false, Setpoint);
}

void show_actual_temperature(Adafruit_SSD1306 &display, const float Input){
    show_temperature(display, true, Input);
} 
  