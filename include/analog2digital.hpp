#pragma once

#include "imxrt.h"

namespace adc {

enum Result {
    SUCCESS,
    FAILURE,
};

enum Module {
    ADC1,
    ADC2,
};

typedef void (*void_function_ptr)(void);

IMXRT_ADCS_t* getModulePointer(Module eModule);
void enableClock(IMXRT_ADCS_t* pModule);
void disableClock(IMXRT_ADCS_t* pModule);

void preCalibrationSetupADC1();
void setup(IMXRT_ADCS_t* pModule);
void setupISR(IMXRT_ADCS_t* pModule, void_function_ptr function);

Result calibrate(IMXRT_ADCS_t* pModule);
void triggerConversion(IMXRT_ADCS_t* pModule, unsigned int teensy_pin_nr);

} // namespace adc
