#pragma once
#include <vector>
#include <cstdint>
#include <variant>

#define MAXPACKETSIZE 16

enum class packetType : uint8_t {
    CONTROL,
    TELEMETRY,
    TOGGLE,
    CHAIN    //Used when stating that the current packet is part of a colletion of 'frames'
};

enum class controlSubType : uint8_t {
    GOTO,
    POSE,
    SPECIAL,
    STABILISE,
    TEST,
    PING,
    REQUEST
};

enum class telemetrySubType : uint8_t {
    VIDEO,
    AUDIO,
    GPS,
    IMU,
    TEMP,
    PRES,
    MAG,
    VOLT,
    CURR,
};

enum class toggleSubType : uint8_t {};


struct serialPacket {
    uint8_t data[MAXPACKETSIZE];
    size_t length;
    serialPacket() : data{}, length(0){}
    serialPacket(const std::vector<uint8_t>& bytes) : data{}, length(bytes.size()){
        std::copy(bytes.begin(),bytes.begin() + length,data);
    }
};

struct packet{
    private:
    uint8_t header : 2;
    packetType type : 2;
    uint8_t length : 4;
    std::variant<controlSubType, telemetrySubType, toggleSubType> subType;
    std::vector<uint8_t> payload;
    uint8_t checksum : 4;
    public:
    serialPacket serialise(){
        std::vector<uint8_t> bytes;
        uint8_t subTypeValue; 
        //This doesnt auto fill bytes appropriately

        uint8_t metaByte1 = (header & 0x03) | ((static_cast<uint8_t>(type) & 0x03) << 2) |((length & 0x0F) << 4);
        switch (type){
        case packetType::CONTROL:
            subTypeValue = static_cast<uint8_t>(std::get<controlSubType>(subType));
            break;
        case packetType::TELEMETRY:
            subTypeValue = static_cast<uint8_t>(std::get<telemetrySubType>(subType));
            break;
        case packetType::TOGGLE:
            subTypeValue = static_cast<uint8_t>(std::get<toggleSubType>(subType));
            break;
        case packetType::CHAIN:
            // Handle CHAIN packet type if needed
            break;
        }

        uint8_t metaByte2 = (subTypeValue & 0x0F)| ((checksum & 0x0F) << 4);


        bytes.push_back(metaByte1);
        bytes.push_back(metaByte2);
        bytes.insert(bytes.end(),payload.begin(),payload.end());

        return serialPacket(bytes);
    }
    packet(packetType t, std::variant<controlSubType, telemetrySubType, toggleSubType> st, std::vector<uint8_t> p) : type(t), subType(st), payload(p), length(p.size()), header(00), checksum(00) {};
};

struct sachet{
    packetType type : 2;
    std::variant<controlSubType, telemetrySubType, toggleSubType> subType;
    std::vector<uint8_t> payload;
    sachet(){};
};

enum class mediumType : int{
    BLE,
    RF,
    USB,
    WIFI
};

enum class connectionStatus : int{
    CONNECTED,
    DISCONNECTED,
    ERROR
};

struct connectionDetails{
    float integrity;    //Percentage
    float latency;      //Timestamp packets on send and receive, only if toggled on
    float throughput; //Bytes per second
    connectionStatus status;
    //connectionDetails();

};
