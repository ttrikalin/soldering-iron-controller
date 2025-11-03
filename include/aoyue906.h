#ifndef H_AOYUE906 
#define H_AOYUE906 



// comment the following to disable Serial monitor 
#define ENABLE_SERIAL

#include "user_hardware.h"


#ifdef TC_MAX31855
  #include <Adafruit_MAX31855.h>
  #include <SPI.h>
  #define THERMOCOUPLE_DELAY_MS 100
#endif
#ifdef TC_MAX6675
  #include <MAX6675.h>
  #define THERMOCOUPLE_DELAY_MS 250
#endif
#include <QuickPID.h>
#include "oled_SD1306.h"


// Thermocouple 
#define THERMOCOUPLE_DATA        19
#define THERMOCOUPLE_CLOCK       18
#define THERMOCOUPLE_CHIP_SELECT  5
#define ERROR_LED                33

#define TEMPERATURE_SET_PIN      34  // ADC pin for temperature set potentiometer
#define IRON_RELAY               2



/*****************************************************************************/
/* THERMOCOUPLE MONITOR                                                       */
/*****************************************************************************/

#define TC_GAIN_K 41.276
#define TC_GAIN_J 57.953
#define TC_GAIN_N 36.256
#define TC_GAIN_T 52.18

#define TC_GAIN_RATIO_K_TO_J  0.7122323
#define TC_GAIN_RATIO_K_TO_N  1.13846
#define TC_GAIN_RATIO_K_TO_T  0.791031

typedef enum {
  THERMOCOUPLE_MONITOR_INIT = 0,
  THERMOCOUPLE_MONITOR_WAIT = 1,
  THERMOCOUPLE_MONITOR_READ = 2  
} thermocouple_monitor_states; 

typedef struct {
    const char* name;
    float resistance;
    char tc_type;
    bool tc_grounded;
} tipProfile;

typedef enum {
  THERMOCOUPLE_ERROR_NONE = 0,
  THERMOCOUPLE_ERROR_DISCONNECTED = 1,
  THERMOCOUPLE_ERROR_SHORTED_TO_GROUND = 2,
  THERMOCOUPLE_ERROR_SHORTED_TO_VCC = 3
} thermocouple_error;

typedef struct {
  #ifdef TC_MAX6675
    MAX6675 thermocouple;
  #endif
  #ifdef TC_MAX31855
    Adafruit_MAX31855 thermocouple; 
  #endif
  thermocouple_monitor_states state;
  float wand_celsius;
  float ambient_celsius; 
  unsigned long last_read_ms;
  unsigned long read_every_ms;
  bool read_flag; 
  bool error_flag;
  thermocouple_error error;
  tipProfile tip;
} thermocoupleMonitorData;

void thermocouple_monitor_initialize(void);
void thermocouple_monitor_tasks(void);
void read_thermocouple();

float voltage_from_temperature(float gain, float temperature, float ambient_temperature);
float temperature_from_voltage(float gain, float voltage, float ambient_temperature);
float convert_temperature1_to_temperature2(float temperature1, float gain1, float gain2, float ambient_temperature);
float convert_temperatureK_to_temperatureJ(float temperatureK, float ambient_temperature);
float convert_temperatureK_to_temperatureN(float temperatureK, float ambient_temperature);
float convert_temperatureK_to_temperatureT(float temperatureK, float ambient_temperature); 
float convert_temperature_reading(const tipProfile &tip, float temperature_reading, float ambient_temperature);




/*****************************************************************************/
/* POTENTIOMETER MONITOR                                                     */
/*****************************************************************************/

typedef enum {
  POTENTIOMETER_MONITOR_INIT = 0, 
  POTENTIOMETER_MONITOR_WAIT = 1, 
  POTENTIOMETER_MONITOR_READ = 2
} potentiometer_monitor_states;

typedef struct {
  potentiometer_monitor_states state; 
  unsigned int value;
  float current_celsius; 
  float previous_celsius;
  unsigned long last_change_time_ms;
  bool changed;
  unsigned long read_every_ms;
} potentiometerMonitorData;

void potentiometer_monitor_initialize(void);
void potentiometer_monitor_tasks(void);
void read_potentiometer();





/*****************************************************************************/
/* DISPLAY MONITOR                                                     */
/*****************************************************************************/



#endif