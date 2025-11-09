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
QuickPID myPID(&tc_monitor.wand_celsius, &heater_control_monitor.pid_output_ms, &pot_monitor.current_celsius, heater_control_monitor.aggressive_tune.Kp, heater_control_monitor.aggressive_tune.Ki, heater_control_monitor.aggressive_tune.Kd,
               myPID.pMode::pOnError,
               myPID.dMode::dOnMeas,
               myPID.iAwMode::iAwClamp,
               myPID.Action::direct);

void heater_control_initialize(void){
  heater_control_monitor.state = HEATER_CONTROL_MONITOR_INIT;

  heater_control_monitor.aggressive_tune.Kp = 5.0;
  heater_control_monitor.aggressive_tune.Ki = 2.0;
  heater_control_monitor.aggressive_tune.Kd = 1.0;

  heater_control_monitor.conservative_tune.Kp = 1.0;
  heater_control_monitor.conservative_tune.Ki = 0.5;  
  heater_control_monitor.conservative_tune.Kd = 0.20;

  heater_control_monitor.gap_to_switch_to_aggressive_tune = 50.0;

  heater_control_monitor.debounce_time_ms = 20;
  
  heater_control_monitor.pid_output_window_size_ms = PID_WINDOW_SIZE_MS;
  heater_control_monitor.pid_max_output_ms = get_pid_max_output(tc_monitor.tip, heater_control_monitor.pid_output_window_size_ms, SUPPLY_POWER_WATTS, SUPPLY_VOLTAGE_VOLTS);
  heater_control_monitor.pid_output_ms = 0;

  heater_control_monitor.pid_window_start_time_ms = 0;
  heater_control_monitor.now_ms = 0;
  heater_control_monitor.next_relay_switch_time_ms = 0;
  
  heater_control_monitor.relay_on = false;
  //heater_control_monitor.can_compute_flag = false;

  myPID.SetOutputLimits(0, heater_control_monitor.pid_max_output_ms);
  myPID.SetSampleTimeUs(heater_control_monitor.pid_output_window_size_ms * 1000);
  myPID.SetMode(myPID.Control::automatic);
}



void heater_control_tasks(void){
  heater_control_monitor.now_ms = millis();
  switch(heater_control_monitor.state){

    case HEATER_CONTROL_MONITOR_INIT:
      heater_control_monitor.state = HEATER_CONTROL_MONITOR_WAIT;
      break;
    
    case HEATER_CONTROL_MONITOR_WAIT:
      if(!tc_monitor.error_flag && pot_monitor.current_celsius >= TURN_ON_TEMPERATURE_CELSIUS){ 
        heater_control_monitor.state = HEATER_CONTROL_MONITOR_COMPUTE;
      } else {
        heater_control_monitor.relay_on = false;
      }
      break;
    
    case HEATER_CONTROL_MONITOR_COMPUTE:
      Serial.println("heater control compute:");
      pid_compute();
      heater_control_monitor.state = HEATER_CONTROL_MONITOR_WAIT;
      break;
    
    default:
      heater_control_monitor.state = HEATER_CONTROL_MONITOR_INIT;
      break;
  }
}


void pid_compute(void){
  float gap = abs(pot_monitor.current_celsius - tc_monitor.wand_celsius); //distance away from setpoint

  if (gap < heater_control_monitor.gap_to_switch_to_aggressive_tune) {  
    myPID.SetTunings(heater_control_monitor.aggressive_tune.Kp, heater_control_monitor.aggressive_tune.Ki, heater_control_monitor.aggressive_tune.Kd);
  } else {
    myPID.SetTunings(heater_control_monitor.conservative_tune.Kp, heater_control_monitor.conservative_tune.Ki, heater_control_monitor.conservative_tune.Kd);
  }
  if (myPID.Compute()) {
    heater_control_monitor.pid_window_start_time_ms = heater_control_monitor.now_ms;
  }
  if (heater_control_monitor.now_ms > heater_control_monitor.next_relay_switch_time_ms) {
      heater_control_monitor.next_relay_switch_time_ms = heater_control_monitor.now_ms + heater_control_monitor.debounce_time_ms;
  }
  if (!heater_control_monitor.relay_on && 
      heater_control_monitor.pid_output_ms >= (heater_control_monitor.now_ms - heater_control_monitor.pid_window_start_time_ms)) {
      heater_control_monitor.relay_on = true;
  } else if (heater_control_monitor.relay_on && 
             heater_control_monitor.pid_output_ms < (heater_control_monitor.now_ms - heater_control_monitor.pid_window_start_time_ms)) {
      heater_control_monitor.relay_on = false;
  }
}


unsigned long get_pid_max_output(const tipProfile& profile, unsigned long range_max, float supply_power, float supply_voltage) {
  unsigned long max_output = (unsigned long) sqrt(profile.resistance * supply_power) *((float) range_max)/ supply_voltage;
  max_output = max_output > range_max ? range_max : max_output;
  return max_output;
}