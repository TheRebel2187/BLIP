#pragma once
#include <vector>
#include <cstdint>
#include "BLIPTypes.hpp"
#include "NimBLEDevice.h"
#include <Arduino.h>


class BLIP {
    public:
        mediumType medium;
        bool tx(packetType t, packetSubType st, std::vector<uint8_t> p);
        bool rx();
        connectionDetails queryConnection();
        sachet getLastSachet();
        BLIP(){};
    private:
        std::vector<packet> packetise(packetType t, packetSubType st, std::vector<uint8_t> p);
        connectionDetails details;
        std::vector<sachet> sachetList;
        bool txUSB(packet p);
        bool txBLE(packet p);
        bool txWIFI(packet p);
        bool txLORA(packet p);
        bool rxUSB();
        bool rxBLE();
        bool rxWIFI();
        bool rxLORA();
};
