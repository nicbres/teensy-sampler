#ifdef SERIAL_DEBUG
#include "Arduino.h"
#endif

#include "imxrt.h"
#include "timer.hpp"

namespace timer {

#ifdef SERIAL_DEBUG
void dumpPeriodicRegisters() {
    Serial.printf("PIT.MCR: 0x%X\r\n", PIT_MCR);
    Serial.printf("PIT.LDVAL0: 0x%X\r\n", PIT_LDVAL0);
    Serial.printf("PIT.LDVAL1: 0x%X\r\n", PIT_LDVAL1);
    Serial.printf("PIT.LDVAL2: 0x%X\r\n", PIT_LDVAL2);
    Serial.printf("PIT.LDVAL3: 0x%X\r\n", PIT_LDVAL3);
    Serial.printf("PIT.TCTRL0: 0x%X\r\n", PIT_TCTRL0);
    Serial.printf("PIT.TCTRL1: 0x%X\r\n", PIT_TCTRL1);
    Serial.printf("PIT.TCTRL2: 0x%X\r\n", PIT_TCTRL2);
    Serial.printf("PIT.TCTRL3: 0x%X\r\n", PIT_TCTRL3);
}
#endif

static void dummyISR(void) {}

void_function_ptr isr_table_periodic[4] = {dummyISR, dummyISR, dummyISR,
                                           dummyISR};

void setupPeriodicISR(uint8_t number, void_function_ptr function) {
    if (number >= 4)
        return;
    isr_table_periodic[number] = function;
}

void ISR(void) {
    // disable interrupts
    NVIC_DISABLE_IRQ(IRQ_PIT);

    // check which timer has caused interrupt
    if (PIT_TFLG0) {
        PIT_TFLG0 = 0x1; // clear interrupt flag
        (*isr_table_periodic[0])();
    }
    if (PIT_TFLG1) {
        PIT_TFLG1 = 0x1; // clear interrupt flag
        (*isr_table_periodic[1])();
    }
    if (PIT_TFLG2) {
        PIT_TFLG2 = 0x1; // clear interrupt flag
        (*isr_table_periodic[2])();
    }
    if (PIT_TFLG3) {
        PIT_TFLG3 = 0x1; // clear interrupt flag
        (*isr_table_periodic[3])();
    }

    // enable interrupts
    NVIC_ENABLE_IRQ(IRQ_PIT);
}

void setupPeriodic() {
#ifdef SERIAL_DEBUG
    Serial.println(">>> timer::setupPeriodic()");
#endif
    // Setup Peripheral Clock
    // derive Pre-Peripheral Clock from PLL2
    CCM_CBCMR |= CCM_CBCMR_PRE_PERIPH_CLK_SEL(0);
    // derive Peripheral from Pre-Peripheral Clock
    CCM_CBCDR &= ~CCM_CBCDR_PERIPH_CLK_SEL;
    CCM_CBCDR |= CCM_CBCDR_AHB_PODF(0); // divide /1 -> AHB Clock
    CCM_CBCDR |= CCM_CBCDR_IPG_PODF(3); // divide /4
    CCM_CSCMR1 &= ~CCM_CSCMR1_PERCLK_CLK_SEL;
    CCM_CSCMR1 &= ~CCM_CSCMR1_PERCLK_PODF(0); // divide /1
    // Clock: 132 Mhz -> Period: 7.5757576 ns
    // Fs: 500 kHz -> Period: 2 us -> 264 Clock Periods (-1)

    PIT_MCR &= ~PIT_MCR_MDIS; // Enable PIT Module

    // Periodic Timer 3
    // PIT_LDVAL3 = 263; // 500kHz
    PIT_LDVAL3 = 527; // 250kHz
    // PIT_LDVAL3 = 0x0FFFFFFF;  // Slow periodic value for testing
    PIT_TCTRL3 |= PIT_TCTRL_TIE; // Enable Interrupt
    attachInterruptVector(IRQ_PIT, ISR);
    NVIC_ENABLE_IRQ(IRQ_PIT);
#ifdef SERIAL_DEBUG
    dumpPeriodicRegisters();
    Serial.println("<<< timer::setupPeriodic()");
#endif
}

void startPeriodic() {
#ifdef SERIAL_DEBUG
    Serial.println(">>> timer::startPeriodic()");
#endif
    PIT_TCTRL3 |= PIT_TCTRL_TEN; // Enable Timer
#ifdef SERIAL_DEBUG
    Serial.println("<<< timer::startPeriodic()");
#endif
}

} // namespace timer
