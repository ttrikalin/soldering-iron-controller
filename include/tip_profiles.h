#ifndef TIP_PROFILES_H
#define TIP_PROFILES_H

#include <cmath>
// profiles for different tip types 

// We should choose a better alignment for the struct members for padding, but this is fine for now.
struct TipProfile {
    const char* name;
    float resistance;
    char thermocouple_type;
    bool thermocouple_grounded;
};

const TipProfile t12_7G = {"T12-7G",  7.0, 'K', true};
const TipProfile t12_9 = {"T12-9",  9.0, 'K', false};
const TipProfile t12_12 = {"T12-12",  12.0, 'K', false};
const TipProfile aoyue_906 = {"Aoyue-906", 18.0, 'K', false};

const TipProfile tipProfiles[] = {
    t12_7G,
    t12_9,
    t12_12,
    aoyue_906
};

unsigned long get_pid_max_output(const TipProfile &profile, unsigned long range_max, float supply_power, float supply_voltage);


#endif // TIP_PROFILES_H