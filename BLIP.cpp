#include "BLIP.hpp"
#include "BLIPTypes.hpp"

//BLIND LOSSY PACKET PROTOCOL BL(I)P


bool BLIP::txUSB(packet p){}
bool BLIP::txBLE(packet p){
    //Use BLE open advertising
    //Should update the current BLE advert to the new packet
    static bool initialized = false;
    static bool live = false;


    if(!initialized){
        NimBLEDevice::init("");
        initialized = true;
    }

 
    NimBLEAdvertisementData advData;
    serialPacket sp = p.serialise();
    std::string manufacturerData(reinterpret_cast<const char*>(sp.data),sp.length);
    advData.setManufacturerData(manufacturerData);
    for (unsigned char c : manufacturerData){
        Serial.printf("%02X ", c);
    }
    Serial.println();

    NimBLEAdvertising* advertising = NimBLEDevice::getAdvertising();

    if(live){
        live = advertising->stop();
    }
    advertising->setAdvertisementData(advData);
    live = advertising->start();

    return live;
}
bool BLIP::txWIFI(packet p){}
bool BLIP::txLORA(packet p){}
bool BLIP::rxUSB(){}
bool BLIP::rxBLE(){
    bool newRead = false;

    NimBLEScan* pScan = NimBLEDevice::getScan();
    pScan->setActiveScan(false);
    NimBLEScanResults results = pScan->getResults(1000);

    for (int i = 0; i < results.getCount(); i++) {
        const NimBLEAdvertisedDevice* device = results.getDevice(i);
        if (!device.haveManufacturerData()) {
            continue;
        }

        std::string data = device.getManufacturerData();
}


}
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
