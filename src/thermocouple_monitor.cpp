#include "aoyue906.h"

void thermocouple_monitor_initialize(void) {
  thermocouple_monitor.state = THERMOCOUPLE_MONITOR_INIT;
  thermocouple_monitor.wand_celsius = 0.0;
  thermocouple_monitor.ambient_celsius = 0.0;
  thermocouple_monitor.last_read_ms = 0;
  thermocouple_monitor.read_every_ms = 500;
  thermocouple_monitor.read_flag = false;
  thermocouple_monitor.error_flag = false;
  thermocouple_monitor.error = THERMOCOUPLE_ERROR_NONE;
  thermocouple_monitor.tip = t12_7G;
}

void thermocouple_monitor_tasks(void) {
  switch (thermocouple_monitor.state) {
    case THERMOCOUPLE_MONITOR_INIT:
      thermocouple_monitor.state = THERMOCOUPLE_MONITOR_WAIT;
      break;

    case THERMOCOUPLE_MONITOR_WAIT:
      if (millis() - thermocouple_monitor.last_read_ms >= thermocouple_monitor.read_every_ms) {
        thermocouple_monitor.state = THERMOCOUPLE_MONITOR_READ;
      }
      break;

    case THERMOCOUPLE_MONITOR_READ:
      read_thermocouple();
      //thermocouple_monitor.last_read_ms = millis();
      thermocouple_monitor.state = THERMOCOUPLE_MONITOR_WAIT;
      break;

    default:
      thermocouple_monitor.state = THERMOCOUPLE_MONITOR_INIT;
      break;
  }
}


void read_thermocouple(){
  tc_monitor.wand_celsius = tc_monitor.thermocouple.readCelsius();
  #ifdef TC_MAX31855
    tc_monitor.ambient_celsius = convert_temperature_reading(tc_monitor.tip, tc_monitor.thermocouple.readCelsius(), tc_monitor.thermocouple.readInternal());
  #endif
  #ifdef TC_MAX6675
    tc_monitor.ambient_celsius = convert_temperature_reading(tc_monitor.tip, tc_monitor.thermocouple.readCelsius(), 25.0);
  #endif
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
  switch(tip.thermocouple_type) {
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