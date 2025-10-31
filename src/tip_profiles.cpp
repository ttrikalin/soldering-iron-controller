#include "tip_profiles.h"


unsigned long get_pid_max_output(const TipProfile& profile, unsigned long range_max, float supply_power, float supply_voltage) {
  unsigned long max_output = (unsigned long) sqrt(profile.resistance * supply_power) *((float) range_max)/ supply_voltage;
  max_output = max_output > range_max ? range_max : max_output;
  return max_output;
}