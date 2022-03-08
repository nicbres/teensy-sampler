#include "Arduino.h"
#include "analog2digital.hpp"
#include "ethernet_communication.hpp"

int main(void) {
#ifdef SERIAL_DEBUG
    Serial.begin(9600);
    while (!Serial) {
        ; // wait for serial to connect
    }
#endif

    while (true) {
        delay(1);
    }
}
