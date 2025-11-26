#include "aoyue906.h"

extern thermocoupleMonitorData tc_monitor;

void MCU_initialize(void){
  pinMode(IRON_RELAY, OUTPUT);
  pinMode(THERMOCOUPLE_CONNECT, OUTPUT);
  pinMode(ERROR_LED, OUTPUT);

  digitalWrite(THERMOCOUPLE_CONNECT, LOW);
  digitalWrite(IRON_RELAY, LOW);

  // just to indicate initialization startup on the ERROR_LED
  for(int i = 0; i < 3; i++){
    digitalWrite(ERROR_LED, HIGH);
    delay(150);
    digitalWrite(ERROR_LED, LOW);
    delay(150);
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


