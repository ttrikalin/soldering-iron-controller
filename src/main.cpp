#include <Arduino.h>
#include "aoyue906.h"

#ifdef TC_MAX31855
  SPIClass vspi(VSPI);
  Adafruit_MAX31855 thermocouple(THERMOCOUPLE_CHIP_SELECT, &vspi);
#endif
#ifdef TC_MAX6675
  MAX6675 thermocouple(THERMOCOUPLE_CLOCK, THERMOCOUPLE_CHIP_SELECT, THERMOCOUPLE_DATA);
#endif
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

potentiometerMonitorData pot_monitor; 
thermocoupleMonitorData tc_monitor;
displayMonitorData display_monitor;
heaterControlMonitorData heater_control_monitor;
extern QuickPID myPID; 

void setup(void){
  MCU_initialize();
  potentiometer_monitor_initialize();
  thermocouple_monitor_initialize();
  display_monitor_initialize();
  heater_control_initialize();
}

void loop(void){
  potentiometer_monitor_tasks();
  thermocouple_monitor_tasks();
  display_monitor_tasks();
  heater_control_tasks();

  // MCU Tasks
  digitalWrite(IRON_RELAY, heater_control_monitor.relay_on ? HIGH : LOW);
  digitalWrite(ERROR_LED, tc_monitor.error_flag ? HIGH : LOW);
  #ifdef ENABLE_SERIAL
    Serial.print("TC Temp: ");
    Serial.print(tc_monitor.wand_celsius);
    Serial.print(" C, Pot Temp: ");
    Serial.print(pot_monitor.current_celsius);
    Serial.print(" C, PID Output ms: ");
    Serial.print(heater_control_monitor.pid_output_ms);
    Serial.print(" ms, Relay On: ");
    Serial.print(heater_control_monitor.relay_on ? "Yes" : "No");
    Serial.print(", TC connected: ");
    Serial.println(tc_monitor.connect_flag ? "Yes" : "No");
  #endif
}



