#include "ethernet_communication.hpp"
#include "NativeEthernetUdp.h"

namespace eth {

EthernetUDP Udp;

byte MAC[6] = {
    0xDE, 0xAD, 0xBE, 0xEF, 0xBA, 0xBE,
};

IPAddress LocalIP(10, 0, 0, 1);
IPAddress DestinationIP(10, 0, 0, 2);

unsigned LocalPort = 42042;
unsigned DestinationPort = 42142;

void setup() {
#ifdef SERIAL_DEBUG
    Serial.println(">>> eth::setup()");
#endif
    Ethernet.begin(MAC, LocalIP);
    if (Ethernet.hardwareStatus() == EthernetNoHardware) {
        while (true) {
            ; // endless loop if no Hardware available
        }
    }
    Udp.begin(LocalPort);
#ifdef SERIAL_DEBUG
    Serial.println("<<< eth:setup()");
#endif
}

void sendData(packet_t* pPacket) {
#ifdef SERIAL_DEBUG
    Serial.println(">>> eth::sendData()");
    Serial.printf("Packet: 0x%X | %u\r\n", pPacket->Header, pPacket->Payload);
#endif
    Udp.beginPacket(DestinationIP, DestinationPort);

    uint8_t* PacketStart = reinterpret_cast<uint8_t*>(pPacket);
    Udp.write(PacketStart, sizeof(packet_t));

    Udp.endPacket();
#ifdef SERIAL_DEBUG
    Serial.println("<<< eth::sendData()");
#endif
}

packet_t receiveData() {
#ifdef SERIAL_DEBUG
    Serial.println(">>> receiveData()");
#endif
    int PacketSize = Udp.parsePacket();
#ifdef SERIAL_DEBUG
    Serial.printf("PacketSize: %d\r\n", PacketSize);
#endif
    packet_t Packet;
    Packet.Header = 0;
    Packet.Payload = 0;

    if (PacketSize == sizeof(packet_t)) {
        Udp.read(reinterpret_cast<uint8_t*>(&Packet), sizeof(packet_t));
    }
#ifdef SERIAL_DEBUG
    Serial.printf("PacketHeader: 0x%X\r\n", Packet.Header);
    Serial.printf("PacketPayload: %u\r\n", Packet.Payload);
#endif

    Udp.flush();
#ifdef SERIAL_DEBUG
    Serial.println("<<< receiveData()");
#endif
    return Packet;
}

} // namespace eth
