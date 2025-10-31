/****************************************************************************
  PID Relay Output Example
  https://github.com/Dlloydev/QuickPID/tree/master/examples/PID_RelayOutput

  Similar to basic example, except the output is a digital pin controlling
  a mechanical relay, SSR, MOSFET or other device. To interface the PID output
  to a digital pin, we use "time proportioning control" (software PWM).
  First we decide on a window size (5000mS for example). We then set the pid
  to adjust its output between 0 and that window size and finally we set the
  PID sample time to that same window size.

  The digital output has the following features:
  • The PID compute rate controls the rate of updating the digital output
  • All transitions are debounced (rising and falling)
  • Full control range (0 to windowSize) isn't limited by debounce
  • Only one call to digitalWrite() per transition
  *****************************************************************************/

// comment the following to disable Serial monitor 
#define ENABLE_SERIAL
#define ENABLE_OLED_DISPLAY


#include <Arduino.h>
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
#include "thermocouple_conversions.h"
#include "tip_profiles.h"
#ifdef ENABLE_OLED_DISPLAY
  #include "oled_SD1306.h"
#endif

// Thermocouple 
#define THERMOCOUPLE_DATA        19
#define THERMOCOUPLE_CLOCK       18
#define THERMOCOUPLE_CHIP_SELECT  5
#define ERROR_LED                35

#define TEMPERATURE_SET_PIN      34
#define IRON_RELAY               2

#ifdef TC_MAX31855
  SPIClass vspi(VSPI);
  //Adafruit_MAX31855 thermocouple(THERMOCOUPLE_CLOCK, THERMOCOUPLE_CHIP_SELECT, THERMOCOUPLE_DATA);
  Adafruit_MAX31855 thermocouple(THERMOCOUPLE_CHIP_SELECT, &vspi);
#endif
#ifdef TC_MAX6675
  MAX6675 thermocouple(THERMOCOUPLE_CLOCK, THERMOCOUPLE_CHIP_SELECT, THERMOCOUPLE_DATA);
#endif


#ifdef ENABLE_OLED_DISPLAY
  // display OLED pins for ESP32
  #define SDA_PIN 21 // default SDA pin for ESP32
  #define SCL_PIN 22 // default SCL pin for ESP32
  Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
#endif

TipProfile active_tip = ACTIVE_TIP;

int pot_value = 0; 

float Setpoint = 300, Input = 0, Output = 0;
#define GAP_FOR_CONSERVATIVE_TUNINGS 25
struct Tunings {
  float Kp; 
  float Ki; 
  float Kd;
}; 
Tunings aggressive = {10.0, 5.0, 2.0}; 
Tunings conservative = {1.0, 0.05, 0.25}; 


// user settings -- adjust as needed
const unsigned long windowSize = PID_WINDOW_SIZE_MS;
const unsigned long max_output = get_pid_max_output(active_tip, windowSize, SUPPLY_POWER_WATTS, SUPPLY_VOLTAGE_VOLTS); // for Aoyue 906 tip
const byte debounce = 20;

// status
unsigned long windowStartTime = 0, nextSwitchTime = 0, msNow = 0;
boolean relayStatus = false;

QuickPID myPID(&Input, &Output, &Setpoint, aggressive.Kp, aggressive.Ki, aggressive.Kd,
               myPID.pMode::pOnError,
               myPID.dMode::dOnMeas,
               myPID.iAwMode::iAwClamp,
               myPID.Action::direct);


void readSetPoint();
void readInput();
void PIDCompute();

void setup() {
  pinMode(IRON_RELAY, OUTPUT);
  pinMode(ERROR_LED, OUTPUT);
  digitalWrite(ERROR_LED, HIGH); 
  #ifdef ENABLE_SERIAL
    Serial.begin(115200);
  #endif
  #ifdef ENABLE_OLED_DISPLAY
    setup_OLED_display(display, SDA_PIN, SCL_PIN);
  #endif
    // Wire.begin(SDA_PIN, SCL_PIN);
    // if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
    //   Serial.println(F("SSD1306 allocation failed"));
    //   for(;;); // Don't proceed, loop forever
    // }
    // display.clearDisplay();
    // //delay(1000); 
  #ifdef TC_MAX31855
    vspi.begin(THERMOCOUPLE_CLOCK, THERMOCOUPLE_DATA, THERMOCOUPLE_CHIP_SELECT);
    thermocouple.begin(); 
    thermocouple.setFaultChecks(MAX31855_FAULT_ALL);
  #endif
  #ifdef TC_MAX6675
    // no setup required for MAX6675
  #endif
  myPID.SetOutputLimits(0, max_output);
  myPID.SetSampleTimeUs(windowSize * 1000);
  myPID.SetMode(myPID.Control::automatic);

}



void loop() {
  // put your main code here, to run repeatedly:
  
  msNow = millis();
  readInput(); 

  if(isnan(Input)) {
    #ifdef ENABLE_SERIAL
      Serial.print("Failed to read temperature sensor! "); 
      #ifdef TC_MAX31855
        Serial.println(thermocouple.readError());
      #endif
    #endif
    digitalWrite(ERROR_LED, HIGH); 
  } else {
    digitalWrite(ERROR_LED, LOW); 
    #ifdef ENABLE_SERIAL
      Serial.print("Temperature is: "); 
      Serial.print(Input); 
      Serial.println(" oC"); 
      Serial.print("Output: ");
      Serial.println(Output);
    #endif

    readSetPoint(); 
    
    PIDCompute();
    
    #ifdef ENABLE_OLED_DISPLAY
     update_OLED_display(display, Setpoint, Input, Output, max_output, msNow, windowStartTime, windowSize, active_tip);
    #endif 
  }
}


void readInput(){
  delay(THERMOCOUPLE_DELAY_MS-debounce);
  digitalWrite(IRON_RELAY, LOW);
  delay(debounce);
  Input = thermocouple.readCelsius();
  #ifdef TC_MAX31855
    Input = convert_temperature_reading(active_tip, thermocouple.readCelsius(), thermocouple.readInternal());
  #endif
  #ifdef TC_MAX6675
    Input = convert_temperature_reading(active_tip, thermocouple.readCelsius(), 25.0);
  #endif
  if (relayStatus) {
    digitalWrite(IRON_RELAY, HIGH);
  }
}


void readSetPoint() {
  pot_value = 0; 
  for(int i = 0; i<8; i++){
    pot_value += analogRead(TEMPERATURE_SET_PIN);
  }
  pot_value = (pot_value >> 3); 

  Setpoint = MIN_TEMP_CELSIUS + (MAX_TEMP_CELSIUS - MIN_TEMP_CELSIUS) *  ((float)pot_value) / 4095;
  #ifdef ENABLE_SERIAL
    Serial.print("Pot value is "); 
    Serial.println(pot_value); 

    Serial.print("Temperature set: "); 
    Serial.print(Setpoint); 
    Serial.println(" oC"); 
  #endif
}


void PIDCompute(){
  float gap = abs(Setpoint - Input); //distance away from setpoint
  if (gap < GAP_FOR_CONSERVATIVE_TUNINGS) { 
    myPID.SetTunings(conservative.Kp, conservative.Ki, conservative.Kd);
  } else {
    myPID.SetTunings(aggressive.Kp, aggressive.Ki, aggressive.Kd);
  }    
  if (myPID.Compute()) windowStartTime = msNow;
  if (!relayStatus && Output > (msNow - windowStartTime)) {
    if (msNow > nextSwitchTime) {
      nextSwitchTime = msNow + debounce;
      relayStatus = true;
      digitalWrite(IRON_RELAY, HIGH);
    }
  } else if (relayStatus && Output < (msNow - windowStartTime)) {
    if (msNow > nextSwitchTime) {
      nextSwitchTime = msNow + debounce;
      relayStatus = false;
      digitalWrite(IRON_RELAY, LOW);
    }
  }
}

