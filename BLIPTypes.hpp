#pragma once
#include <vector>
#include <cstdint>

#define MAXPACKETSIZE 16

enum class packetType : uint8_t {
    CONTROL,
    TELEMETRY,
    TOGGLE,
    CHAIN    //Used when stating that the current packet is part of a colletion of 'frames'
};

enum class packetSubType : uint8_t {
    GOTO,
    POSE,
    SPECIAL,
    STABILISE,
    EMOTE,
    JUMP,
    STAND,
    TEST,
    VIDEO,
    AUDIO,
    GPS,
    IMU,
    TEMP,
    PRES,
    MAG,
    VOLT,
    CURR,
    PING,
    REQUEST
};

struct packetPayload{
    packetSubType subType : 4;   //This needs ammending but the intention is correct
    std::vector<uint8_t> payload;
    packetPayload(packetSubType st, std::vector<uint8_t> p) : subType(st), payload(std::move(p)) {};
};

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
    packetPayload payload;
    uint8_t checksum : 4;
    public:
    serialPacket serialise(){
        std::vector<uint8_t> bytes;

        //This doesnt auto fill bytes appropriately

        uint8_t metaByte1 = (header & 0x03) | ((static_cast<uint8_t>(type) & 0x03) << 2) |((length & 0x0F) << 4);
        uint8_t metaByte2 = (static_cast<uint8_t>(payload.subType)&0x0F)|((checksum & 0x0F)<<4);

        bytes.push_back(metaByte1);
        bytes.push_back(metaByte2);
        bytes.insert(bytes.end(),payload.payload.begin(),payload.payload.end());

        return serialPacket(bytes);
    }
    packet(packetType t, packetSubType st, std::vector<uint8_t> p) : type(t), payload({st, p}), length(p.size()+16), header(00), checksum(00) {};
};

struct sachet{
    //uint8_t header : 2;
    packetType type : 2;
    //uint8_t length : 4;
    packetPayload payload;
    //uint8_t checksum : 8;
};

enum class mediumType : int{
    BLE,
    LORA,
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
