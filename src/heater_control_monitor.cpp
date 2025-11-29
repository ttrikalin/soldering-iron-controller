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

#include "aoyue906.h"

extern heaterControlMonitorData heater_control_monitor;
extern thermocoupleMonitorData tc_monitor;
extern potentiometerMonitorData pot_monitor;
extern mcuMonitorData mcu_monitor;

QuickPID myPID(&tc_monitor.wand_celsius, &heater_control_monitor.pid_duty_cycle, &pot_monitor.current_celsius, heater_control_monitor.aggressive_tune.Kp, heater_control_monitor.aggressive_tune.Ki, heater_control_monitor.aggressive_tune.Kd,
               myPID.pMode::pOnError,
               myPID.dMode::dOnMeas,
               myPID.iAwMode::iAwClamp,
               myPID.Action::direct);

void heater_control_initialize(void){
  heater_control_monitor.state = HEATER_CONTROL_MONITOR_INIT;

  heater_control_monitor.aggressive_tune.Kp = 8.0;
  heater_control_monitor.aggressive_tune.Ki = 0.8;
  heater_control_monitor.aggressive_tune.Kd = 0.00;

  heater_control_monitor.conservative_tune.Kp = 8.0;
  heater_control_monitor.conservative_tune.Ki = 0.8;  
  heater_control_monitor.conservative_tune.Kd = 0.00;

  heater_control_monitor.gap_to_switch_to_aggressive_tune = 150.0;
  heater_control_monitor.gap = 0.0;

  heater_control_monitor.dead_time_us = 500; // microseconds

  heater_control_monitor.pid_duty_cycle = 0.0;
  heater_control_monitor.pid_duty_cycle_int = 0;
  
  myPID.SetOutputLimits(0, (float) ((1<<mcu_monitor.pid_resolution)-1));
  myPID.SetSampleTimeUs(heater_control_monitor.pid_sample_window_ms * 1000);
  myPID.SetMode(myPID.Control::automatic);
}



void heater_control_tasks(void){
  //heater_control_monitor.now_ms = millis();
  switch(heater_control_monitor.state){

    case HEATER_CONTROL_MONITOR_INIT:
      heater_control_monitor.state = HEATER_CONTROL_MONITOR_WAIT;
      break;
    
    case HEATER_CONTROL_MONITOR_WAIT:
      if(!tc_monitor.error_flag 
        && tc_monitor.new_measurement_flag
        && mcu_monitor.machine_on){ 
        heater_control_monitor.state = HEATER_CONTROL_MONITOR_COMPUTE;
      } 
      if (tc_monitor.connect_flag || tc_monitor.error_flag || !mcu_monitor.machine_on) {
        heater_control_monitor.pid_duty_cycle = 0.0;
        heater_control_monitor.pid_duty_cycle_int = 0;
      }
      break;
    
    case HEATER_CONTROL_MONITOR_COMPUTE:
      #ifdef ENABLE_SERIAL
        Serial.println("heater control compute:");
      #endif
      pid_compute();
      tc_monitor.new_measurement_flag = false;
      heater_control_monitor.state = HEATER_CONTROL_MONITOR_WAIT;
      break;
    
    default:
      heater_control_monitor.state = HEATER_CONTROL_MONITOR_INIT;
      break;
  }
}


void pid_compute(void){
  heater_control_monitor.gap = abs(pot_monitor.current_celsius - tc_monitor.wand_celsius); //distance away from setpoint
  if (heater_control_monitor.gap < heater_control_monitor.gap_to_switch_to_aggressive_tune) {  
    myPID.SetTunings(heater_control_monitor.aggressive_tune.Kp, heater_control_monitor.aggressive_tune.Ki, heater_control_monitor.aggressive_tune.Kd);
  } else {
    myPID.SetTunings(heater_control_monitor.conservative_tune.Kp, heater_control_monitor.conservative_tune.Ki, heater_control_monitor.conservative_tune.Kd);
  }
  if (myPID.Compute()) {
      heater_control_monitor.pid_duty_cycle_int = (int) heater_control_monitor.pid_duty_cycle;
  } else {
      #ifdef ENABLE_SERIAL
        Serial.println("PID compute failed");
      #endif
      heater_control_monitor.pid_duty_cycle = 0.0;
      heater_control_monitor.pid_duty_cycle_int = 0;
  }
}



