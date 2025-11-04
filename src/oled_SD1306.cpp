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
    show_power_bar(display, Output, max_output);
    display.display();
    return;
  }
  if( msNow - windowStartTime <= (windowSize>>1) ) {
    return;
  }
  if (Input <-195) { 
    show_off_status(display);
    display.display();
    return;
  }
  show_actual_temperature(display, Input);
  show_power_bar(display, Output, max_output);
  display.display();
  return;
}

void show_off_status(Adafruit_SSD1306 &display){
  display.clearDisplay();
  display.setTextSize(5);      
  display.setCursor(10,15);
  display.println("OFF");
  return;
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
  //display.display();
}

void show_set_temperature(Adafruit_SSD1306 &display, const float Setpoint){
    show_temperature(display, false, Setpoint);
}

void show_actual_temperature(Adafruit_SSD1306 &display, const float Input){
    show_temperature(display, true, Input);
} 

void show_power_bar(Adafruit_SSD1306 &display, const float Input, const float max_input){
  int bar_width = (int) round((Input / max_input) * (SCREEN_WIDTH - 10));

  display.drawRect(2, (int) SCREEN_HEIGHT - 10, SCREEN_WIDTH - 5, 8, WHITE);
  display.fillRect(5, (int) SCREEN_HEIGHT - 8, bar_width, 4, WHITE);
  //display.display();
}