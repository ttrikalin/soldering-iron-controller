#include "aoyue906.h"

extern mcuMonitorData mcu_monitor;
extern thermocoupleMonitorData tc_monitor;
extern heaterControlMonitorData heater_control_monitor;

void MCU_initialize(void){
  mcu_monitor.now_ms = 0;
  mcu_monitor.machine_on = false;
  mcu_monitor.pid_resolution = 8; // 8-bit resolution for analogWrite
  mcu_monitor.pid_frequency_Hz = 100; // PWM frequency in Hz

  pinMode(IRON_RELAY, OUTPUT);
  pinMode(THERMOCOUPLE_CONNECT, OUTPUT);
  pinMode(ERROR_LED, OUTPUT);

  digitalWrite(THERMOCOUPLE_CONNECT, LOW);
  analogWriteResolution(IRON_RELAY, mcu_monitor.pid_resolution);
  analogWriteFrequency(IRON_RELAY, mcu_monitor.pid_frequency_Hz); // set PWM frequency to 100 Hz
  analogWrite(IRON_RELAY, 0);

  // just to indicate initialization startup on the ERROR_LED
  for(int i = 0; i < 3; i++){
    digitalWrite(ERROR_LED, HIGH);
    delay(200);
    digitalWrite(ERROR_LED, LOW);
    delay(200);
  } 

  #ifdef ENABLE_SERIAL
    Serial.begin(MCU_SERIAL_BAUD);
  #endif

  #ifdef TC_MAX31855
    vspi.begin(THERMOCOUPLE_CLOCK, THERMOCOUPLE_DATA, THERMOCOUPLE_CHIP_SELECT);
    thermocouple.begin(); 
    thermocouple.setFaultChecks(MAX31855_FAULT_ALL);
  #endif
  
  #ifdef TC_MAX6675
    // no setup required for MAX6675
  #endif
  
}

void MCU_tasks(void){
  mcu_monitor.now_ms = millis();
  digitalWrite(ERROR_LED, tc_monitor.error_flag ? HIGH : LOW);
  analogWrite(IRON_RELAY, mcu_monitor.machine_on ? heater_control_monitor.pid_duty_cycle_int : 0);
}

