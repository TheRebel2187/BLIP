#include "BLIP.hpp"
#include "BLIPTypes.hpp"

//BLIND LOSSY PACKET PROTOCOL BL(I)P


sachet deserialise(std::string d){
    sachet s;

    
    uint8_t metaByte1 = static_cast<uint8_t>(d[0]);
    uint8_t metaByte2 = static_cast<uint8_t>(d[1]);

    // Reverse metaByte1
    uint8_t header = metaByte1 & 0x03;
    uint8_t rawType = (metaByte1 >> 2) & 0x03;
    uint8_t length = (metaByte1 >> 4) & 0x0F;

    // Reverse metaByte2
    uint8_t rawSubType = metaByte2 & 0x0F;
    uint8_t checksum = (metaByte2 >> 4) & 0x0F;

    //All residual data is the payload data

    packetType type = static_cast<packetType>(rawType);
    packetSubType subType = static_cast<packetSubType>(rawSubType);

    std::vector<uint8_t> data;
    for (size_t i = 2; i < d.size(); ++i){
        data.push_back(static_cast<uint8_t>(d[i]));
    }

    packetPayload p(subType,data);
    s.type = type;
    s.payload = p;

    return s;
}


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
bool BLIP::txLORA(packet p){
    bool sent = false;
    serialPacket sp = p.serialise();
    lora.beginPacket();
    lora.write(sp.data,sp.length);
    lora.endPacket();
    sent = true;
    return sent;
}
bool BLIP::rxUSB(){}


bool BLIP::rxBLE(){
    if (rxCharacteristic == nullptr){
        return false;
    }
    std::string data = rxCharacteristic->getValue();
    //susceptible to losing data if polling too slow 

    if (data.empty()){
        return false;
    };
    
    sachet s = deserialise(data);
    sachetList.push_back(s);

    return true;
}


bool BLIP::rxWIFI(){}
bool BLIP::rxLORA(){
    // lora.receive();
    // int packetSize = LoRa.parsePacket();
    // if (packetSize == 0){
    //     return false;
    // }else{
    //     uint8_t buf[32];
    //     int len = 0;
    //     if (packetSize) {
    //         while (LoRa.available() && len < sizeof(buf)) {
    //             buf[len++] = LoRa.read();
    //         }
    //     }


    //     sachet s = deserialise(data);
    //     sachetList.push_back(s);
    // }
    // return true;
}

sachet BLIP::getLastSachet(){
    sachet p = sachetList.front();
    sachetList.erase(sachetList.begin());
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


bool BLIP::startBLE(std::string deviceName){
    NimBLEDevice::init(deviceName);
    NimBLEServer* server = NimBLEDevice::createServer();
    NimBLEService* service = server->createService("12345678-1234-1234-1234-123456789ABC");

    txCharacteristic = service->createCharacteristic(
        "12345678-1234-1234-1234-123456789ABD",
        NIMBLE_PROPERTY::NOTIFY
    );

    rxCharacteristic = service->createCharacteristic(
        "12345678-1234-1234-1234-123456789ABE",
        NIMBLE_PROPERTY::WRITE |
        NIMBLE_PROPERTY::WRITE_NR
    );

    

    service->start();
    NimBLEAdvertising* advertising = NimBLEDevice::getAdvertising();

    advertising->addServiceUUID(service->getUUID());
    advertising->start();

    return true;
}

bool BLIP::startLORA(){
    bool started = false;
    lora.setPins(5, 6, 7); // Set the pins for LoRa module (SS, Reset, DIO0)  Currently filler values
    started = lora.begin(868E6); 
    return started;
}


bool startWIFI(){}
bool startUSB(){}
