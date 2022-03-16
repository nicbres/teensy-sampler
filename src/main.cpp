#ifdef SERIAL_DEBUG
#include "Arduino.h"
#endif

#include "analog2digital.hpp"
#include "ethernet_communication.hpp"
#include "input_output.hpp"
#include "timer.hpp"

static void periodicTimer3ISR(void) {
#ifdef SERIAL_DEBUG
    Serial.println(">>> periodicTimer3ISR()");
#endif
    adc::triggerConversion(&IMXRT_ADC1, 0); // trigger ADC1 channel 0 -> Pin 21
#ifdef SERIAL_DEBUG
    Serial.println("<<< periodicTimer3ISR()");
#endif
}

static void conversionADC1ISR(void) {
#ifdef SERIAL_DEBUG
    Serial.println(">>> conversionADC1ISR()");
#endif
    // TODO: disable keeper on pad
    uint32_t Data = IMXRT_ADC1.R0;
    eth::packet_t Packet;
    Packet.Header = eth::DATA;
    Packet.Payload = Data;
    eth::sendData(&Packet);
#ifdef SERIAL_DEBUG
    Serial.println("<<< conversionADC1ISR()");
#endif
}

int main(void) {
#ifdef SERIAL_DEBUG
    Serial.begin(9600);
    while (!Serial) {
        ; // wait for serial to connect
    }
    Serial.printf("Serial connected\r\n");
#endif

    gpio::setup();

    while (true) {
#ifdef SERIAL_DEBUG
        Serial.printf("Loop\r\n");
        delay(1000);
#endif
    }
}
