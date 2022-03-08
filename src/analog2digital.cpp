#ifdef SERIAL_DEBUG
#include "Arduino.h"
#endif

#include "analog2digital.hpp"

namespace adc {

const uint8_t adc_channel_to_gpio_pin[16] = {
    27, // GPIO_AD_B1_11
    12, // GPIO_AD_B0_12
    13, // GPIO_AD_B0_13
    30, // GPIO_AD_B0_14
    31, // GPIO_AD_B0_15
    16, // GPIO_AD_B1_00
    17, // GPIO_AD_B1_01
    18, // GPIO_AD_B1_02
    19, // GPIO_AD_B1_03
    20, // GPIO_AD_B1_04
    21, // GPIO_AD_B1_05
    22, // GPIO_AD_B1_06
    23, // GPIO_AD_B1_07
    24, // GPIO_AD_B1_08
    25, // GPIO_AD_B1_09
    26, // GPIO_AD_B1_10
};

/*
const uint32_t* adc_channel_to_pad_control_register[16] = {
    27, // GPIO_AD_B1_11
    12, // GPIO_AD_B0_12
    13, // GPIO_AD_B0_13
    30, // GPIO_AD_B0_14
    31, // GPIO_AD_B0_15
    16, // GPIO_AD_B1_00
    17, // GPIO_AD_B1_01
    18, // GPIO_AD_B1_02
    19, // GPIO_AD_B1_03
    20, // GPIO_AD_B1_04
    21, // GPIO_AD_B1_05
    22, // GPIO_AD_B1_06
    23, // GPIO_AD_B1_07
    24, // GPIO_AD_B1_08
    25, // GPIO_AD_B1_09
    26, // GPIO_AD_B1_10
}*/

#ifdef SERIAL_DEBUG
void dumpConfigRegisters(IMXRT_ADCS_t* pModule) {
    Serial.printf("CFG: 0x%X\r\n", pModule->CFG);
    Serial.printf("GC: 0x%X\r\n", pModule->GC);
    Serial.printf("CV: 0x%X\r\n", pModule->CV);
    Serial.printf("OFS: 0x%X\r\n", pModule->OFS);
    Serial.printf("CAL: 0x%X\r\n", pModule->CAL);
}
#endif

IMXRT_ADCS_t* getModulePointer(Module eModule) {
    if (eModule == ADC1) {
        return &IMXRT_ADC1;
    } else if (eModule == ADC2) {
        return &IMXRT_ADC2;
    }
    return nullptr;
}

void enableClock(IMXRT_ADCS_t* pModule) {
#ifdef SERIAL_DEBUG
    Serial.printf(">>> adc::enableClock(0x%X)\r\n", pModule);
#endif
    if (pModule == &IMXRT_ADC1) {
        CCM_CCGR1 |= CCM_CCGR1_ADC1(CCM_CCGR_ON); // Enable Clock to ADC1
    } else if (pModule == &IMXRT_ADC2) {
        CCM_CCGR1 |= CCM_CCGR1_ADC2(CCM_CCGR_ON); // Enable Clock to ADC2
    }
#ifdef SERIAL_DEBUG
    Serial.printf("<<< adc::enableClock(0x%X)\r\n", pModule);
#endif
}

void disableClock(IMXRT_ADCS_t* pModule) {
#ifdef SERIAL_DEBUG
    Serial.printf(">>> adc::disableClock(0x%X)\r\n", pModule);
#endif
    if (pModule == &IMXRT_ADC1) {
        CCM_CCGR1 &= ~CCM_CCGR1_ADC1(CCM_CCGR_ON); // Disable Clock to ADC1
    } else if (pModule == &IMXRT_ADC2) {
        CCM_CCGR1 &= ~CCM_CCGR1_ADC2(CCM_CCGR_ON); // Disable Clock to ADC2
    }
#ifdef SERIAL_DEBUG
    Serial.printf("<<< adc::disableClock(0x%X)\r\n", pModule);
#endif
}

void setDefault(IMXRT_ADCS_t* pModule) {
    pModule->CFG = 0x00000200;
    pModule->GC = 0x0;
    pModule->CV = 0x0;
    pModule->OFS = 0x0;
    pModule->CAL = 0x0;
}

void preCalibrationSetupADC1() {
// TODO: figure out why disabling clock after this function resets ADC values
#ifdef SERIAL_DEBUG
    Serial.println(">>> adc::preCalibrationSetupADC1()");
#endif
    IMXRT_ADCS_t* pModule = &IMXRT_ADC1;

    setDefault(pModule);
    pModule->CFG |= ADC_CFG_MODE(2); // 12-bit mode
    pModule->CFG |= ADC_CFG_ADLSMP;  // Long Sample Time Mode
    pModule->CFG &= ~ADC_CFG_ADTRG;  // Software Trigger
    pModule->CFG |= ADC_CFG_OVWREN;  // Data Overwrite Enable

    enableClock(pModule);
#ifdef SERIAL_DEBUG
    dumpConfigRegisters(pModule);
    Serial.println("<<< adc::preCalibrationSetupADC1()");
#endif
}

Result calibrate(IMXRT_ADCS_t* pModule) {
#ifdef SERIAL_DEBUG
    Serial.printf(">>> adc::calibrate(0x%X)\r\n", pModule);
#endif
    if ((pModule != &IMXRT_ADC1) && (pModule != &IMXRT_ADC2)) {
#ifdef SERIAL_DEBUG
        Serial.printf("<<< adc::calibrate(0x%X)\r\n", pModule);
#endif
        return SUCCESS;
    }

    pModule->GC |= ADC_GC_CAL; // Trigger Calibration

    while (pModule->GC & ADC_GC_CAL) {
        // wait for self-calibration to finish
        // described in section 66.5.6
    }
    Result CalibrationStatus = (pModule->GS & ADC_GS_CALF) ? SUCCESS : FAILURE;

#ifdef SERIAL_DEBUG
    dumpConfigRegisters(pModule);
    Serial.printf("<<< adc::calibrate(0x%X)\r\n", pModule);
#endif

    return CalibrationStatus;
}

void setup(IMXRT_ADCS_t* pModule) {
#ifdef SERIAL_DEBUG
    Serial.printf(">>> adc::setup(0x%X)\r\n", pModule);
#endif
    if (pModule == &IMXRT_ADC1) {
        pModule->HC0 |= ADC_HC_AIEN; // enable interrupt
        NVIC_ENABLE_IRQ(IRQ_ADC1);
    } else if (pModule == &IMXRT_ADC2) {
        // TODO: setup if actually used
    }
#ifdef SERIAL_DEBUG
    Serial.printf("<<< adc::setup(0x%X)\r\n", pModule);
#endif
}

void setupISR(IMXRT_ADCS_t* pModule, void_function_ptr function) {
#ifdef SERIAL_DEBUG
    Serial.printf(">>> adc::setupISR(0x%X, 0x%X)\r\n", pModule, function);
#endif
    if (pModule == &IMXRT_ADC1) {
        attachInterruptVector(IRQ_ADC1, function);
    } else if (pModule == &IMXRT_ADC2) {
        attachInterruptVector(IRQ_ADC2, function);
    }
#ifdef SERIAL_DEBUG
    Serial.printf("<<< adc::setupISR(0x%X, 0x%X)\r\n", pModule, function);
#endif
}

void triggerConversion(IMXRT_ADCS_t* pModule, unsigned int channel) {
#ifdef SERIAL_DEBUG
    Serial.printf(">>> adc::triggerConversion(0x%X, %u)\r\n", pModule, channel);
#endif
    if (channel > 15) {
#ifdef SERIAL_DEBUG
        Serial.printf(
            "<<< adc::triggerConversion(0x%X, %u)\r\n", pModule, channel);
#endif
        return;
    }
    // clear ADCH to write channel number
    pModule->HC0 = (pModule->HC0 & ~0x1F) | channel;
#ifdef SERIAL_DEBUG
    Serial.printf("<<< adc::triggerConversion(0x%X, %u)\r\n", pModule, channel);
#endif
}

} // namespace adc
