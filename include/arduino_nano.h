
#ifndef H_ARDUINO_NANO
#define H_ARDUINO_NANO


/*****************************************************************************/
/* ARDUINO_NANO SETUP                                                       */
/*****************************************************************************/

// Thermocouple 
#define THERMOCOUPLE_DATA         11
#define THERMOCOUPLE_CLOCK        13
#define THERMOCOUPLE_CHIP_SELECT  10
#define ERROR_LED                  8

#define TEMPERATURE_SET_PIN       10  // ADC pin for temperature set potentiometer
#define IRON_RELAY                 6

#define SDA_PIN                   18
#define SCL_PIN                   19

//#define ZERO_CROSSING_PIN        XX 

#define MCU_SERIAL_BAUD          115200
#endif 