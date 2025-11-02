#ifndef H_AOYUE906 
#define H_AOYUE906 





/*****************************************************************************/
/* THERMOCOUPLE MONITOR                                                       */
/*****************************************************************************/

typedef enum {
  THERMOCOUPLE_MONITOR_INIT = 0,
  THERMOCOUPLE_MONITOR_WAIT = 1,
  THERMOCOUPLE_MONITOR_READ = 2  
} thermocouple_monitor_states; 


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
  bool busy; 
} thermocoupleMonitorData;

void thermocouple_monitor_initialize(void);
void thermocouple_monitor_tasks(void);
void read_thermocouple();



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
  unsigned int current_value; 
  unsigned int previous_value; 
} potentiometerMonitorData;

void potentiometer_monitor_initialize(void);
void potentiometer_monitor_tasks(void);
void read_potentiometer();

#endif