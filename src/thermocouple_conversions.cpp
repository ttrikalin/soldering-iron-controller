#include "thermocouple_conversions.h"


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

float convert_temperature_reading(const TipProfile &tip, float temperature_reading, float ambient_temperature){
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