#include "BLIP.hpp"
#include "BLIPTypes.hpp"

//BLIND LOSSY PACKET PROTOCOL BL(I)P


bool BLIP::txUSB(packet p){}
bool BLIP::txBLE(packet p){
    if (txCharacteristic == nullptr)
        return false;

    serialPacket sp = p.serialise();
    txCharacteristic->setValue(sp.data,sp.length);
    txCharacteristic->notify();

    return true;
}



bool BLIP::txWIFI(packet p){}
bool BLIP::txLORA(packet p){}
bool BLIP::rxUSB(){}
bool BLIP::rxBLE(){}
bool BLIP::rxWIFI(){}
bool BLIP::rxLORA(){}

sachet BLIP::getLastSachet(){
    sachet p = sachetList.back();
    sachetList.pop_back();
    return p;
};

bool BLIP::tx(packetType t, packetSubType st, std::vector<uint8_t> p){
    bool sent = false;
    std::vector<packet> packetList= packetise(t,st,p);

    while(!packetList.empty()){
        packet current = packetList.front();
        packetList.erase(packetList.begin());

        switch(medium){
            case mediumType::BLE:
                sent = txBLE(current);
            break;
            case mediumType::LORA:
                sent = txLORA(current);
            break;
            case mediumType::USB:
                sent = txUSB(current);
            break;
            case mediumType::WIFI:
                sent = txWIFI(current);
            break;
        }
        if(!sent){
            return false;
        }
    }
    return sent;
}

bool BLIP::rx(){
    bool recieved = false;
    switch(medium){
        case mediumType::BLE:
            recieved = rxBLE();
        break;
        case mediumType::LORA:
             recieved = rxLORA();
        break;
        case mediumType::USB:
            recieved = rxUSB();
        break;
        case mediumType::WIFI:
            recieved = rxWIFI();
        break;
    }
    return recieved;
}

connectionDetails BLIP::queryConnection(){}

std::vector<packet> BLIP::packetise(packetType t, packetSubType st, std::vector<uint8_t> p){
            int packetSize;
            std::vector<packet> packets;
            switch(t){
                case packetType::CONTROL:
                    packetSize=p.size();
                break;
                case packetType::TELEMETRY:
                    //DYNAMIC PACKET SIZE
                    packetSize = p.size();
                break;
                case packetType::TOGGLE:
                    //FIXED PACKET SIZE
                break;
                case packetType::CHAIN:
                    packetSize = MAXPACKETSIZE-2;
                break;
            }
            //Break the payload into packets of size packetSize
            for(int i = 0; i < p.size(); i += packetSize){
                std::vector<uint8_t> packetPayload(p.begin() + i, p.begin() + std::min(i + packetSize, (int)p.size()));
                packets.push_back(packet(t, st, packetPayload));
            }
            return packets;
        };


bool BLIP::startBLE(){
    NimBLEDevice::init("BLIP");
    NimBLEServer* server = NimBLEDevice::createServer();
    NimBLEService* service = server->createService("12345678-1234-1234-1234-123456789ABC");

    txCharacteristic = service->createCharacteristic(
            "12345678-1234-1234-1234-123456789ABD",
            NIMBLE_PROPERTY::WRITE |
            NIMBLE_PROPERTY::WRITE_NR |
            NIMBLE_PROPERTY::NOTIFY
    );

    service->start();
    NimBLEAdvertising* advertising = NimBLEDevice::getAdvertising();

    advertising->addServiceUUID(service->getUUID());
    advertising->start();

    return true;
}

bool getLORA(){}
bool getWIFI(){}
bool getUSB(){}
