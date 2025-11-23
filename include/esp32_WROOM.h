#ifndef H_ESP32_WROOM
#define H_ESP32_WROOM 


/*****************************************************************************/
/* ESP32 WROOM32 SETUP                                                       */
/*****************************************************************************/

// Thermocouple 
#define THERMOCOUPLE_DATA        19
#define THERMOCOUPLE_CLOCK       18
#define THERMOCOUPLE_CHIP_SELECT  5
#define ERROR_LED                32

#define TEMPERATURE_SET_PIN      34  // ADC pin for temperature set potentiometer
#define IRON_RELAY                2

#define SDA_PIN                  21
#define SCL_PIN                  22

//#define ZERO_CROSSING_PIN        35 

#define MCU_SERIAL_BAUD          115200
#endif 