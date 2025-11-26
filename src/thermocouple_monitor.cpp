#include "aoyue906.h"

extern thermocoupleMonitorData tc_monitor;
extern heaterControlMonitorData heater_control_monitor;
#ifdef TC_MAX31855
  extern Adafruit_MAX31855 thermocouple;
#endif
#ifdef TC_MAX6675
  extern MAX6675 thermocouple;
#endif

void thermocouple_monitor_initialize(void) {
  tc_monitor.thermocouple = &thermocouple;
  tc_monitor.state = THERMOCOUPLE_MONITOR_INIT;
  tc_monitor.wand_celsius = 0.0;
  tc_monitor.ambient_celsius = 0.0;
  tc_monitor.last_read_ms = 0;
  tc_monitor.read_every_ms = 250;
  tc_monitor.error_flag = false;
  tc_monitor.connect_flag = false;
  tc_monitor.error = THERMOCOUPLE_ERROR_NONE;
  tc_monitor.tip.name = ACTIVE_TIP.name;
  tc_monitor.tip.resistance = ACTIVE_TIP.resistance;
  tc_monitor.tip.tc_type = ACTIVE_TIP.tc_type;
  tc_monitor.tip.tc_grounded = ACTIVE_TIP.tc_grounded;
}

void thermocouple_monitor_tasks(void) {
  switch (tc_monitor.state) {
    case THERMOCOUPLE_MONITOR_INIT:
      tc_monitor.state = THERMOCOUPLE_MONITOR_WAIT;
      break;

    case THERMOCOUPLE_MONITOR_WAIT:
      tc_monitor.connect_flag = false;
      if (heater_control_monitor.now_ms - tc_monitor.last_read_ms >= tc_monitor.read_every_ms) {
        tc_monitor.state = THERMOCOUPLE_MONITOR_READ;
      }
      break;

    case THERMOCOUPLE_MONITOR_READ:
      read_thermocouple();
      tc_monitor.last_read_ms = millis();
      tc_monitor.state = THERMOCOUPLE_MONITOR_WAIT;
      //tc_monitor.read_flag = false;
      break;

    default:
      tc_monitor.state = THERMOCOUPLE_MONITOR_INIT;
      break;
  }
}


void read_thermocouple(){
  digitalWrite(IRON_RELAY, LOW);
  delay(heater_control_monitor.debounce_time_ms); // deadtime + allow time for signal to stabilize 
  tc_monitor.connect_flag = true;
  digitalWrite(THERMOCOUPLE_CONNECT, HIGH);
  tc_monitor.wand_celsius = tc_monitor.thermocouple->readCelsius();
  if (isnan(tc_monitor.wand_celsius) || tc_monitor.wand_celsius == 0.0) {
    tc_monitor.error_flag = true;
    #ifdef TC_MAX31855
      uint8_t fault = tc_monitor.thermocouple->readError();
      if (fault & MAX31855_FAULT_OPEN) {
        tc_monitor.error = THERMOCOUPLE_ERROR_DISCONNECTED;
      } else if (fault & MAX31855_FAULT_SHORT_GND) {
        tc_monitor.error = THERMOCOUPLE_ERROR_SHORTED_TO_GROUND;
      } else if (fault & MAX31855_FAULT_SHORT_VCC) {
        tc_monitor.error = THERMOCOUPLE_ERROR_SHORTED_TO_VCC;
      } else {
        tc_monitor.error = THERMOCOUPLE_ERROR_DISCONNECTED;
      }
    #endif
    #ifdef TC_MAX6675
      tc_monitor.error = THERMOCOUPLE_ERROR_DISCONNECTED;
    #endif
  } else {
    tc_monitor.error_flag = false;
    tc_monitor.error = THERMOCOUPLE_ERROR_NONE;
  }
  #ifdef TC_MAX31855
    tc_monitor.ambient_celsius = convert_temperature_reading(tc_monitor.tip, tc_monitor.thermocouple->readCelsius(), tc_monitor.thermocouple->readInternal());
  #endif
  #ifdef TC_MAX6675
    tc_monitor.ambient_celsius = convert_temperature_reading(tc_monitor.tip, tc_monitor.thermocouple->readCelsius(), 25.0);
  #endif
  tc_monitor.connect_flag = false;
  digitalWrite(THERMOCOUPLE_CONNECT, LOW);
  delay(10); // deadtime to protect thermocouple 
  digitalWrite(IRON_RELAY, heater_control_monitor.relay_on ? HIGH : LOW);
}


float voltage_from_temperature(float gain, float temperature, float ambient_temperature){
  return gain * (temperature - ambient_temperature);
}

float temperature_from_voltage(float gain, float voltage, float ambient_temperature) {
  return (voltage / gain) + ambient_temperature;
}

float convert_temperature1_to_temperature2(float temperature1, float gain1, float gain2, float ambient_temperature) {
  return (gain1/gain2) * (temperature1 - ambient_temperature) + ambient_temperature;
} 

float convert_temperatureK_to_temperatureJ(float temperatureK, float ambient_temperature){
  return (TC_GAIN_RATIO_K_TO_J) * (temperatureK - ambient_temperature) + ambient_temperature;
} 

float convert_temperatureK_to_temperatureN(float temperatureK, float ambient_temperature){
  return (TC_GAIN_RATIO_K_TO_N) * (temperatureK - ambient_temperature) + ambient_temperature;
} 

float convert_temperatureK_to_temperatureT(float temperatureK, float ambient_temperature){
  return (TC_GAIN_RATIO_K_TO_T) * (temperatureK - ambient_temperature) + ambient_temperature;
} 

float convert_temperature_reading(const tipProfile &tip, float temperature_reading, float ambient_temperature){
  switch(tip.tc_type) {
    case 'K':
      return temperature_reading;
    case 'J':
      return convert_temperatureK_to_temperatureJ(temperature_reading, ambient_temperature);
    case 'N':
      return convert_temperatureK_to_temperatureN(temperature_reading, ambient_temperature);
    case 'T':
      return convert_temperatureK_to_temperatureT(temperature_reading, ambient_temperature);
    default:
      return temperature_reading; // unknown type, return as-is
  }
}

