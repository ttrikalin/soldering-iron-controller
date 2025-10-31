#ifndef H_THERMOCOUPLE_CONVERSIONS
#define H_THERMOCOUPLE_CONVERSIONS

#include "tip_profiles.h"

#define TC_GAIN_K 41.276
#define TC_GAIN_J 57.953
#define TC_GAIN_N 36.256
#define TC_GAIN_T 52.18

#define TC_GAIN_RATIO_K_TO_J  0.7122323
#define TC_GAIN_RATIO_K_TO_N  1.13846
#define TC_GAIN_RATIO_K_TO_T  0.791031


float voltage_from_temperature(float gain, float temperature, float ambient_temperature);
float temperature_from_voltage(float gain, float voltage, float ambient_temperature);
float convert_temperature1_to_temperature2(float temperature1, float gain1, float gain2, float ambient_temperature);
float convert_temperatureK_to_temperatureJ(float temperatureK, float ambient_temperature);
float convert_temperatureK_to_temperatureN(float temperatureK, float ambient_temperature);
float convert_temperatureK_to_temperatureT(float temperatureK, float ambient_temperature); 
float convert_temperature_reading(const TipProfile &tip, float temperature_reading, float ambient_temperature);

#endif //H_THERMOCOUPLE_CONVERSIONS