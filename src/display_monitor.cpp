#include "aoyue906.h"

extern displayMonitorData display_monitor;
extern thermocoupleMonitorData tc_monitor;
extern potentiometerMonitorData pot_monitor;
extern heaterControlMonitorData heater_control_monitor;
extern Adafruit_SSD1306 display;

void display_monitor_initialize(void){
  //display_monitor.display = display; 
  display_monitor.state = DISPLAY_MONITOR_INIT;
  display_monitor.heater_off_color_scheme = false;
  display_monitor.text_color = WHITE;
  display_monitor.bar_color = WHITE;
  display_monitor.error_counter = 0;

  Wire.begin(SDA_PIN, SCL_PIN);
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
  display.clearDisplay();
}


void display_monitor_tasks(void){
  switch(display_monitor.state){
    case DISPLAY_MONITOR_INIT:
      display_splash_screen_message();
      display_monitor.state = DISPLAY_MONITOR_WAIT;
      break;
    case DISPLAY_MONITOR_WAIT:
      if(tc_monitor.error_flag) {
        switch(display_monitor.error_counter) {
          case 0:
            display_monitor.state = DISPLAY_MONITOR_NO_WAND_MESSAGE;
            break; 
          case 1:
            display_monitor.state = DISPLAY_MONITOR_POWER_OFF_MESSAGE;
            break;  
          case 2:
            display_monitor.state = DISPLAY_MONITOR_NO_WAND_ERROR_INFO;
            break;  
          case 3:
            display_monitor.state = DISPLAY_MONITOR_THERMOCOUPLE_ERROR_INFO;
            break; 
          default:
            display_monitor.state = DISPLAY_MONITOR_OFF_MESSAGE;
            break; 
        }
      } else if(pot_monitor.changed_flag) {
        display_monitor.state = DISPLAY_MONITOR_POTENTIOMETER_TEMPERATURE;
      } else {
        display_monitor.state = DISPLAY_MONITOR_THERMOCOUPLE_TEMPERATURE;
      }
      break;
    case DISPLAY_MONITOR_POTENTIOMETER_TEMPERATURE:
      display_potentiometer_temperature();
      break;
    case DISPLAY_MONITOR_THERMOCOUPLE_TEMPERATURE:
      display_thermocouple_temperature();
      break;
    case DISPLAY_MONITOR_NO_WAND_MESSAGE:
      display_monitor.error_counter++;
      display_no_wand_message();
      break;
    case DISPLAY_MONITOR_POWER_OFF_MESSAGE:
      display_monitor.error_counter++;
      display_power_off_message();
      break;
    case DISPLAY_MONITOR_NO_WAND_ERROR_INFO:
      display_monitor.error_counter++;
      display_no_wand_error_info();
    case DISPLAY_MONITOR_THERMOCOUPLE_ERROR_INFO:
      display_monitor.error_counter = 0;
      display_thermocouple_error_info();
      break;
    case DISPLAY_MONITOR_OFF_MESSAGE:
      display_off_message();
      break;
    default:
      display_monitor.state = DISPLAY_MONITOR_INIT;
      break;
  }
}




void display_splash_screen_message(void){
  //use_colors(splash_background_color, splash_text_color, WHITE);
  display.clearDisplay();
  display.setTextColor(display_monitor.text_color);
  display.setTextSize(2);
  display.setCursor(10, 20);
  display.println(" AOYUE"); 
  display.println("  906");
  display.invertDisplay(display_monitor.heater_off_color_scheme);
  display.display();
}


void display_thermocouple_error_info(void){
  display.clearDisplay();
  display.setTextSize(2);      
  display.setTextColor(display_monitor.text_color); 
  display.setCursor(0,0);     
  display.println("TC Error?");
  display.setCursor(0,16);     
  display.setTextSize(1);
  display.println("\nCheck for");
  display.println("1 Unconnected wand");
  display.println("2 Thermocouple fault");
  #ifdef TC_MAX31855
    uint8_t fault = thermocouple.readError();
    if (fault & MAX31855_FAULT_OPEN) {
      display.println("- Open circuit");
    } else if (fault & MAX31855_FAULT_SHORT_GND) {
      display.println("- Short to GND");
    } else if (fault & MAX31855_FAULT_SHORT_VCC) {
      display.println("- Short to VCC");
    } else {
      display.println("- Unknown MAX31855 error");
    }
    
  #endif
  #ifdef TC_MAX6675
    display.println("- Thermocouple open?");  
    display.println("- MAX6675 error?");
  #endif
  display.invertDisplay(display_monitor.heater_off_color_scheme);
  display.display();
}


void display_off_message(void){
  display.clearDisplay();
  display.setTextSize(3);      
  display.setTextColor(display_monitor.text_color); 
  display.setCursor(0,8);   
  display.println(" Power");
  display.println("  OFF");
  display.invertDisplay(display_monitor.heater_off_color_scheme);
  display.display();
}

void display_no_wand_message(void){
  display.clearDisplay();
  display.setTextSize(3);      
  display.setTextColor(display_monitor.text_color); 
  display.setCursor(0,20);     
  display.println("No Wand");
  display.invertDisplay(display_monitor.heater_off_color_scheme);
  display.display();
}

void display_power_off_message(void) {
  display.clearDisplay();
  display.setTextSize(3);      
  display.setTextColor(display_monitor.text_color); 
  display.setCursor(0,8);   
  display.println(" Power");
  display.println("  OFF");
  display.invertDisplay(display_monitor.heater_off_color_scheme);
  display.display();
}


void display_no_wand_error_info(void){
  display.clearDisplay();
  display.setTextSize(2);      
  display.setTextColor(display_monitor.text_color); 
  display.setCursor(0,0);     
  display.println("No Wand?");
  display.setCursor(0,16);     
  display.setTextSize(1);
  display.println("\nCheck for");
  display.println("1 Unplugged wand");
  display.println("2 Faulty connection");
  display.println("3 Broken thermocouple");
  display.invertDisplay(display_monitor.heater_off_color_scheme);
  display.display();
}


void show_temperature(bool show_tc_temp){
  int temperature;
  display.clearDisplay();
  display.setTextColor(display_monitor.text_color); 
  display.setTextSize(1);
  display.setCursor(0,0); 
  if(show_tc_temp) {
    display.print("  Now ");
    temperature = (int) round(tc_monitor.wand_celsius); 
  } else {
    display.print("  Set "); 
    temperature = (int) round(pot_monitor.current_celsius);  
  }
  display.setTextSize(5);      
  display.setCursor(10,15);
  if(temperature < 10) {
    display.print("  ");
  } else if(temperature < 100) {
    display.print(" ");
  } 
  display.print(temperature); 
  display.setTextSize(2);      
  display.print((char)247); 
  display.print("C");
  
}

void show_power_bar(){
  int bar_width = (int) round((heater_control_monitor.pid_output_ms / heater_control_monitor.pid_max_output_ms) * (SCREEN_WIDTH - 10));
  display.drawRect(2, (int) SCREEN_HEIGHT - 10, SCREEN_WIDTH - 5, 8, display_monitor.bar_color);
  display.fillRect(5, (int) SCREEN_HEIGHT - 8, bar_width, 4, display_monitor.bar_color);
}

void display_potentiometer_temperature(){
    show_temperature(false);
    show_power_bar();
    display.invertDisplay(display_monitor.heater_off_color_scheme);
    display.display();
}

void display_thermocouple_temperature(){
    show_temperature(true);
    show_power_bar();
    display.invertDisplay(display_monitor.heater_off_color_scheme);
    display.display();
} 

