#include "Arduino.h"
#include "input_output.hpp"
#include "imxrt.h"

namespace gpio {
    void test_isr() {
        NVIC_DISABLE_IRQ(IRQ_GPIO6789);
#ifdef SERIAL_DEBUG
        Serial.println("lessgo");
#endif
        if (IMXRT_GPIO7.ISR)
            IMXRT_GPIO7.ISR = 0xFFFFFFFF;
        if (IMXRT_GPIO8.ISR)
            IMXRT_GPIO8.ISR = 0xFFFFFFFF;
        if (IMXRT_GPIO9.ISR)
            IMXRT_GPIO9.ISR = 0xFFFFFFFF;

        NVIC_ENABLE_IRQ(IRQ_GPIO6789);
    }

    void setup() {
        IMXRT_GPIO_t* pModule = &IMXRT_GPIO8;

        pModule->GDIR = 0x0;
        pModule->ICR2 = 0x2000; // set 22 to rising edge
        pModule->IMR = 0x400000; // activate pin 22 interrupt mask

        attachInterruptVector(IRQ_GPIO6789, test_isr);

        NVIC_ENABLE_IRQ(IRQ_GPIO6789);
    }
}

