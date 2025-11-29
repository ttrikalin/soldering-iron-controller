#include "aoyue906.h"

extern thermocoupleMonitorData tc_monitor;
extern heaterControlMonitorData heater_control_monitor;
extern mcuMonitorData mcu_monitor;

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
  tc_monitor.raw_reading = 0.0;
  tc_monitor.last_read_ms = 0;
  tc_monitor.read_every_ms = 500;
  tc_monitor.error_flag = false;
  tc_monitor.connect_flag = false;
  tc_monitor.new_measurement_flag = false;
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
      //if(heater_control_monitor.now_ms - heater_control_monitor.pid_window_start_time_ms >= heater_control_monitor.pid_max_output_ms){
      if(mcu_monitor.now_ms - tc_monitor.last_read_ms >= tc_monitor.read_every_ms){
        tc_monitor.state = THERMOCOUPLE_MONITOR_READ;
      }
      break;

    case THERMOCOUPLE_MONITOR_READ:
      read_thermocouple();
      tc_monitor.last_read_ms = millis();
      tc_monitor.new_measurement_flag = true;
      tc_monitor.state = THERMOCOUPLE_MONITOR_WAIT;
      break;

    default:
      tc_monitor.state = THERMOCOUPLE_MONITOR_INIT;
      break;
  }
}


void read_thermocouple(){
  analogWrite(IRON_RELAY, 0);
  delayMicroseconds(heater_control_monitor.dead_time_us); // deadtime + allow time for signal to stabilize 
  tc_monitor.connect_flag = true;
  digitalWrite(THERMOCOUPLE_CONNECT, HIGH);
  delay(THERMOCOUPLE_CONVERSION_DELAY_MS);
  tc_monitor.raw_reading = tc_monitor.thermocouple->readCelsius();
  tc_monitor.wand_celsius = get_calibrated_measurement(tc_monitor.raw_reading);
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
  delayMicroseconds(heater_control_monitor.dead_time_us); // deadtime to protect thermocouple 
  analogWrite(IRON_RELAY, heater_control_monitor.pid_duty_cycle_int);
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




float get_calibrated_measurement(float temperature_reading){
 return get_calibrated_measurement_cubic( temperature_reading);
}

float get_calibrated_measurement_quadratic( float temperature_reading){
  return 
  11.229128342 + 2.371390177 * temperature_reading - 0.002833668 *  temperature_reading * temperature_reading;
}
float get_calibrated_measurement_cubic( float temperature_reading){
  return  15.21739 +  2.078516 * temperature_reading +  
          1.495710e-04 * temperature_reading * temperature_reading  
          -7.694609e-06 * temperature_reading * temperature_reading * temperature_reading; 
}
