#pragma once
#include <vector>
#include <cstdint>
#include "BLIPTypes.hpp"
#include "NimBLEDevice.h"
#include <Arduino.h>
#include "LoRa.h"


class BLIP {
    public:
        mediumType medium;
        bool tx(packetType t, packetSubType st, std::vector<uint8_t> p);
        bool rx();
        connectionDetails queryConnection();
        sachet getLastSachet();
        BLIP(){
            startBLE();
            startLORA();
        };
    private:
        std::vector<packet> packetise(packetType t, packetSubType st, std::vector<uint8_t> p);
        connectionDetails details;
        std::vector<sachet> sachetList;
        uint16_t BLEHandle = BLE_HS_CONN_HANDLE_NONE;
        NimBLECharacteristic* txCharacteristic = nullptr;
        NimBLECharacteristic* rxCharacteristic = nullptr;
        LoRaClass lora;
        bool startLORA();
        bool startBLE();
        bool txUSB(packet p);
        bool txBLE(packet p);
        bool txWIFI(packet p);
        bool txLORA(packet p);
        bool rxUSB();
        bool rxBLE();
        bool rxWIFI();
        bool rxLORA();
};
