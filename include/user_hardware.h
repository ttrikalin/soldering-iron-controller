#ifndef SUPPLY_HARDWARE_CHARACTERISTICS_H
#define SUPPLY_HARDWARE_CHARACTERISTICS_H


// select which thermocouple and thermocouple library to use
// Adafruit MAX31855 or MAX6675


//#define TC_MAX31855
#define TC_MAX6675

#define SUPPLY_POWER_WATTS  30.0
#define SUPPLY_VOLTAGE_VOLTS 24.0
#define MIN_TEMP_CELSIUS  180
#define MAX_TEMP_CELSIUS  480
#define PID_WINDOW_SIZE_MS 500

#define ACTIVE_TIP t12_7G //aoyue_906 //t12_7





#endif // SUPPLY_HARDWARE_CHARACTERISTICS_H
