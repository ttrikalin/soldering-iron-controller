#ifndef OLED_SD1306_H 
#define OLED_SD1306_H

#include <cmath>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h> 
#include "tip_profiles.h"



#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)


void setup_OLED_display(Adafruit_SSD1306 &display, const int sda, const int scl);
void update_debug_OLED_display(Adafruit_SSD1306 &display, const float Setpoint, const float Input, const float Output, const float max_output, const unsigned long msNow, const unsigned long windowStartTime, const unsigned long windowSize, const TipProfile &activeTip);
void update_OLED_display(Adafruit_SSD1306 &display, const float Setpoint, const float Input, const float Output, const float max_output, const unsigned long msNow, const unsigned long windowStartTime, const unsigned long windowSize, const unsigned int lastSetpointChangeTime);
void show_temperature(Adafruit_SSD1306 &display, const bool is_actual, const float temperature);
void show_set_temperature(Adafruit_SSD1306 &display, const float Setpoint);
void show_actual_temperature(Adafruit_SSD1306 &display, const float Input);


#endif