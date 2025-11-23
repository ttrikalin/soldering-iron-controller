#ifndef SUPPLY_HARDWARE_CHARACTERISTICS_H
#define SUPPLY_HARDWARE_CHARACTERISTICS_H

// select platform 
#define ESP32_WROOM
//#define ARDUINO_NANO


// select which thermocouple and thermocouple library to use
// Adafruit MAX31855 or MAX6675
//#define TC_MAX31855
#define TC_MAX6675

#define SUPPLY_POWER_WATTS  30.0
#define SUPPLY_VOLTAGE_VOLTS 24.0
#define MIN_TEMP_CELSIUS  180
#define MAX_TEMP_CELSIUS  480
#define PID_WINDOW_SIZE_MS 150


#define ACTIVE_TIP t12_7G //aoyue_906 //t12_7

#define TURN_OFF_TEMPERATURE_CELSIUS  197.0
#define TURN_ON_TEMPERATURE_CELSIUS   200.0

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)



#endif // SUPPLY_HARDWARE_CHARACTERISTICS_H
