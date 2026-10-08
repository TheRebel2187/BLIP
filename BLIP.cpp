#include "BLIP.hpp"
#include "BLIPTypes.hpp"

//BLIND LOSSY PACKET PROTOCOL BL(I)P


sachet deserialise(std::string d){
    

    
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

    std::variant<controlSubType, telemetrySubType, toggleSubType> subType;

    switch(type){
        case packetType::CONTROL:
            subType = static_cast<controlSubType>(rawSubType);
        break;
        case packetType::TELEMETRY:
            subType = static_cast<telemetrySubType>(rawSubType);
        break;
        case packetType::TOGGLE:
            subType = static_cast<toggleSubType>(rawSubType);
        break;
    }

    std::vector<uint8_t> data;
    for (size_t i = 2; i < d.size(); ++i){
        data.push_back(static_cast<uint8_t>(d[i]));
    }
    

    sachet s = sachet(type, subType, data);

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
bool BLIP::txRF(packet p){
    bool sent = false;
    serialPacket sp = p.serialise();
    rf.transmit(sp.data,sp.length);
    sent = true;
    return sent;
}
bool BLIP::rxUSB(){}


bool BLIP::rxBLE(){
    if (rxCharacteristic == nullptr){
        return false;
    }
    std::string data = rxCharacteristic->getValue();
    rxCharacteristic->setValue(""); 
    //susceptible to losing data if polling too slow 

    if (data.empty()){
        return false;
    };
    
    sachet s = deserialise(data);
    sachetList.push_back(s);

    return true;
}


bool BLIP::rxWIFI(){}
bool BLIP::rxRF(){
    String data;
    rf.receive(data);
    //sachet s = deserialise(data.c_str());   Change in the future, de-serialise is poorly written
    //sachetList.push_back(s);
    
    return true;
}

sachet BLIP::getLastSachet(){
    sachet p = sachetList.front();
    sachetList.erase(sachetList.begin());
    return p;
};

bool BLIP::tx(sachet s){
    bool sent = false;
    packet p = packet(s);
    switch(medium){
        case mediumType::BLE:
            sent = txBLE(p);
        break;
        case mediumType::RF:
            sent = txRF(p);
        break;
        case mediumType::USB:
            sent = txUSB(p);
        break;
        case mediumType::WIFI:
            sent = txWIFI(p);
        break;
    }
    return sent;
}


bool BLIP::tx(std::vector<sachet> s){          //This is for where the size of a packet is less than the size of the payload
    bool sent = false;
    for(sachet s : s){
        sent = tx(s);
    }
    return sent;
}

bool BLIP::rx(){
    bool recieved = false;
    switch(medium){
        case mediumType::BLE:
            recieved = rxBLE();
        break;
        case mediumType::RF:
             recieved = rxRF();
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

bool BLIP::startRF(){
    rfSPI.begin(11,12,13,14);
    int state = rf.begin(433.0);
    if (state != RADIOLIB_ERR_NONE) {
        Serial.printf("CC1101 init failed: %d\n", state);
        return false;
    }

    return true;
}


bool startWIFI(){}
bool startUSB(){}
