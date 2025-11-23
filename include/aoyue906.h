#ifndef H_AOYUE906 
#define H_AOYUE906 

// comment the following to disable Serial monitor 
#define ENABLE_SERIAL

#include "user_hardware.h"


//#include <cmath>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h> 
#include <QuickPID.h>

#ifdef TC_MAX31855
  #include <Adafruit_MAX31855.h>
  #define THERMOCOUPLE_DELAY_MS 100
#endif
#ifdef TC_MAX6675
  #include <MAX6675.h>
  #define THERMOCOUPLE_DELAY_MS 250
#endif


#ifdef ESP32_WROOM  
  #include <cmath>
  #include "esp32_WROOM.h"
#endif
#ifdef ARDUINO_NANO
  #include <math.h>
  #include "arduino_nano.h"
#endif

/*****************************************************************************/
/* MCU WROOM32 SETUP                                                       */
/*****************************************************************************/

void MCU_initialize(void);

/*****************************************************************************/
/* THERMOCOUPLE MONITOR                                                      */
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

const tipProfile t12_7G = {"T12-7G",  7.0, 'K', true};
const tipProfile t12_9 = {"T12-9",  9.0, 'K', false};
const tipProfile t12_12 = {"T12-12",  12.0, 'K', false};
const tipProfile aoyue_906 = {"Aoyue-906", 18.0, 'K', false};

typedef enum {
  THERMOCOUPLE_ERROR_NONE = 0,
  THERMOCOUPLE_ERROR_DISCONNECTED = 1,
  THERMOCOUPLE_ERROR_SHORTED_TO_GROUND = 2,
  THERMOCOUPLE_ERROR_SHORTED_TO_VCC = 3
} thermocouple_error;

typedef struct {
  #ifdef TC_MAX6675
    MAX6675 * thermocouple;
  #endif
  #ifdef TC_MAX31855
    Adafruit_MAX31855 * thermocouple; 
  #endif
  volatile thermocouple_monitor_states state;
  float wand_celsius;
  float ambient_celsius; 
  unsigned long last_read_ms;
  unsigned long read_every_ms;
  //bool read_flag; 
  bool error_flag;
  thermocouple_error error;
  tipProfile tip;
} thermocoupleMonitorData;

void thermocouple_monitor_initialize(void);
void thermocouple_monitor_tasks(void);
void read_thermocouple();

//void IRAM_ATTR zero_crossing_ISR(void);

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
  volatile potentiometer_monitor_states state; 
  unsigned int value;
  float current_celsius; 
  float previous_celsius;
  unsigned long last_change_time_ms;
  bool changed_flag;
  unsigned long read_every_ms;
} potentiometerMonitorData;

void potentiometer_monitor_initialize(void);
void potentiometer_monitor_tasks(void);
void read_potentiometer();


/*****************************************************************************/
/* DISPLAY MONITOR                                                     */
/*****************************************************************************/

typedef enum {
  DISPLAY_MONITOR_INIT = 0,
  DISPLAY_MONITOR_WAIT = 1,
  DISPLAY_MONITOR_POTENTIOMETER_TEMPERATURE = 2,
  DISPLAY_MONITOR_THERMOCOUPLE_TEMPERATURE = 3,
  DISPLAY_MONITOR_OFF_MESSAGE = 4,
  DISPLAY_MONITOR_POWER_OFF_MESSAGE = 5,
  DISPLAY_MONITOR_NO_WAND_MESSAGE = 6,
  DISPLAY_MONITOR_THERMOCOUPLE_ERROR_INFO = 7,
  DISPLAY_MONITOR_NO_WAND_ERROR_INFO = 8
} display_monitor_states;


typedef struct {
  volatile display_monitor_states state;
  bool heater_off_color_scheme;
  unsigned int text_color;
  unsigned int bar_color;
  unsigned int display_counter;
  unsigned long last_display_counter_update_time_ms;
  unsigned long update_display_counter_every_ms;
} displayMonitorData;


void display_monitor_initialize(void);
void display_monitor_tasks(void);

void display_splash_screen_message(void);
void display_off_message(void);
void display_power_off_message(void);
void display_no_wand_message();

void display_thermocouple_error_info(void);
void display_no_wand_error_info(void);

void show_temperature(bool show_tc_temp);
void display_potentiometer_temperature(void);
void display_thermocouple_temperature(void);



/*****************************************************************************/
/* HEATER CONTROL MONITOR                                                    */
/*****************************************************************************/
typedef enum{
  HEATER_CONTROL_MONITOR_INIT = 0, 
  HEATER_CONTROL_MONITOR_WAIT = 1, 
  HEATER_CONTROL_MONITOR_COMPUTE = 2
} heater_control_monitor_states;


typedef struct {
  float Kp; 
  float Ki; 
  float Kd;
} Tunings; 



typedef struct {
  volatile heater_control_monitor_states state;
  Tunings aggressive_tune;
  Tunings conservative_tune;
  float gap_to_switch_to_aggressive_tune;
  byte debounce_time_ms;

  unsigned long pid_output_window_size_ms;
  float pid_max_output_ms;
  float pid_output_ms;
  unsigned long pid_window_start_time_ms;

  unsigned long next_relay_switch_time_ms;
  unsigned long now_ms;
  
  bool relay_on; 
  //bool can_compute_flag;

} heaterControlMonitorData;

void heater_control_initialize(void);
void heater_control_tasks(void);
void pid_compute(void);
unsigned long get_pid_max_output(const tipProfile&profile, unsigned long range_max, float supply_power, float supply_voltage);

#endif