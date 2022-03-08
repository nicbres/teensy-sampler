#pragma once

#include "NativeEthernet.h"
#include "stdint.h"

namespace eth {

extern byte MAC[6];

extern IPAddress LocalIP;
extern IPAddress DestinationIP;

extern unsigned LocalPort;
extern unsigned DestinationPort;

enum eHeader {
    NONE = 0x0,
    START = 0x1, // Start Conversion (empty payload)
    DATA = 0xC,  // Data packet
};

typedef struct {
    // struct will be aligned to multiple of word size anyways so this
    // just uses two words for simplicity (could be optimized)
    uint32_t Header;
    uint32_t Payload;
} packet_t;

void setup();

void sendData(packet_t* TransmitPacket);
packet_t receiveData();

} // namespace eth
