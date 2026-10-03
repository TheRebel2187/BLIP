#pragma once
#include <vector>
#include <cstdint>
#include "BLIPTypes.hpp"
#include "NimBLEDevice.h"
#include <Arduino.h>
#include "RadioLib.h"
//#include "LoRa.h"




class BLIP {
    public:
        mediumType medium;
        bool tx(packetType t, packetSubType st, std::vector<uint8_t> p);
        bool rx();
        connectionDetails queryConnection();
        sachet getLastSachet();
        BLIP(std::string deviceName){
            startBLE(deviceName);
            startRF();
        };
    private:
        std::vector<packet> packetise(packetType t, packetSubType st, std::vector<uint8_t> p);
        connectionDetails details;
        std::vector<sachet> sachetList;
        uint16_t BLEHandle = BLE_HS_CONN_HANDLE_NONE;
        NimBLECharacteristic* txCharacteristic = nullptr;
        NimBLECharacteristic* rxCharacteristic = nullptr;
        SPIClass rfSPI = SPIClass(FSPI);
        CC1101 rf = new Module(14,RADIOLIB_NC,RADIOLIB_NC,RADIOLIB_NC,rfSPI);
        bool startRF();
        bool startBLE(std::string deviceName);
        bool txUSB(packet p);
        bool txBLE(packet p);
        bool txWIFI(packet p);
        bool txRF(packet p);
        bool rxUSB();
        bool rxBLE();
        bool rxWIFI();
        bool rxRF();
};
