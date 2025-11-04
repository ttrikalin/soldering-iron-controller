#include "aoyue906.h"

extern potentiometerMonitorData pot_monitor;

void potentiometer_monitor_initialize(void) {
  pot_monitor.state = POTENTIOMETER_MONITOR_INIT;
  pot_monitor.value = 0;
  pot_monitor.current_celsius = 0.0;
  pot_monitor.previous_celsius = 0.0;
  pot_monitor.last_change_time_ms = 0;
  pot_monitor.changed_flag = false;
  pot_monitor.read_every_ms = 500;
}


void potentiometer_monitor_tasks(void) {
  switch (pot_monitor.state) {
    case POTENTIOMETER_MONITOR_INIT:
      pot_monitor.state = POTENTIOMETER_MONITOR_WAIT;
      break;

    case POTENTIOMETER_MONITOR_WAIT:
      if (millis() - pot_monitor.last_change_time_ms >= pot_monitor.read_every_ms) {
        pot_monitor.state = POTENTIOMETER_MONITOR_READ;
      }
      break;

    case POTENTIOMETER_MONITOR_READ:
      read_potentiometer();
      pot_monitor.state = POTENTIOMETER_MONITOR_WAIT;
      break;

    default:
      pot_monitor.state = POTENTIOMETER_MONITOR_INIT;
      break;
  }
}




void read_potentiometer() {
  pot_monitor.value = analogRead(TEMPERATURE_SET_PIN); 
  for(int i = 0; i<7; i++){
    pot_monitor.value += analogRead(TEMPERATURE_SET_PIN);
  }
  pot_monitor.value = (pot_monitor.value >> 3); 

  pot_monitor.current_celsius = MIN_TEMP_CELSIUS + (MAX_TEMP_CELSIUS - MIN_TEMP_CELSIUS) *  ((float)pot_monitor.value) / 4095;
  if (abs(pot_monitor.current_celsius - pot_monitor.previous_celsius) >= 5) {
    pot_monitor.changed_flag = true;
    pot_monitor.previous_celsius = pot_monitor.current_celsius;
    pot_monitor.last_change_time_ms = millis();
  } else {
    pot_monitor.changed_flag = false;
  }

  #ifdef ENABLE_SERIAL
    Serial.print("Pot value is "); 
    Serial.println(pot_monitor.value); 

    Serial.print("Temperature set: "); 
    Serial.print(pot_monitor.current_celsius); 
    Serial.println(" oC"); 
  #endif
}
