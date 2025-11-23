#include "aoyue906.h"

extern thermocoupleMonitorData tc_monitor;

void MCU_initialize(void){
  pinMode(IRON_RELAY, OUTPUT);
  pinMode(ERROR_LED, OUTPUT);

  pinMode(ZERO_CROSSING_PIN, INPUT);
  //attachInterrupt(ZERO_CROSSING_PIN, zero_crossing_ISR, RISING);

  digitalWrite(IRON_RELAY, LOW);
  digitalWrite(ERROR_LED, HIGH); 
  #ifdef ENABLE_SERIAL
    Serial.begin(115200);
  #endif

  #ifdef TC_MAX31855
    vspi.begin(THERMOCOUPLE_CLOCK, THERMOCOUPLE_DATA, THERMOCOUPLE_CHIP_SELECT);
    thermocouple.begin(); 
    thermocouple.setFaultChecks(MAX31855_FAULT_ALL);
  #endif
  
  #ifdef TC_MAX6675
    // no setup required for MAX6675
  #endif

  digitalWrite(ERROR_LED, LOW); 
}


