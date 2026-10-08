#pragma once
#include <vector>
#include <cstdint>
#include <variant>
#include <string>

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

enum class toggleSubType : uint8_t {
    ALLSENSORS,
    LOCATION,
    ALLMOVEMENT,
};


struct serialPacket {
    uint8_t data[MAXPACKETSIZE];
    size_t length;
    serialPacket() : data{}, length(0){}
    serialPacket(const std::vector<uint8_t>& bytes) : data{}, length(bytes.size()){
        std::copy(bytes.begin(),bytes.begin() + length,data);
    }
};

struct sachet{
    packetType type;
    std::variant<controlSubType, telemetrySubType, toggleSubType> subType;
    std::vector<uint8_t> payload;
    std::string plaintextPayload;
    std::string plaintextType;
    std::string plaintextSubType;
    sachet(packetType t, std::variant<controlSubType, telemetrySubType, toggleSubType> st, std::vector<uint8_t> p) : type(t), subType(st), payload(p) {
        // Convert payload to plaintext string
        plaintextPayload = std::string(payload.begin(), payload.end());
        // Convert type to plaintext string
        switch (t) {
            case packetType::CONTROL:
                plaintextType = "CONTROL";
                break;
            case packetType::TELEMETRY:
                plaintextType = "TELEMETRY";
                break;
            case packetType::TOGGLE:
                plaintextType = "TOGGLE";
                break;
            case packetType::CHAIN:
                plaintextType = "CHAIN";
                break;
        }
        // Convert subType to plaintext string
        switch (t) {
            case packetType::CONTROL:
                switch (std::get<controlSubType>(st)) {
                    case controlSubType::GOTO:
                        plaintextSubType = "GOTO";
                        break;
                    case controlSubType::POSE:
                        plaintextSubType = "POSE";
                        break;
                    case controlSubType::SPECIAL:
                        plaintextSubType = "SPECIAL";
                        break;
                    case controlSubType::STABILISE:
                        plaintextSubType = "STABILISE";
                        break;
                    case controlSubType::TEST:
                        plaintextSubType = "TEST";
                        break;
                    case controlSubType::PING:
                        plaintextSubType = "PING";
                        break;
                    case controlSubType::REQUEST:
                        plaintextSubType = "REQUEST";
                        break;
                }
                break;
            case packetType::TELEMETRY:
                switch (std::get<telemetrySubType>(st)) {
                    case telemetrySubType::VIDEO:
                        plaintextSubType = "VIDEO";
                        break;
                    case telemetrySubType::AUDIO:
                        plaintextSubType = "AUDIO";
                        break;
                    case telemetrySubType::GPS:
                        plaintextSubType = "GPS";
                        break;
                    case telemetrySubType::IMU:
                        plaintextSubType = "IMU";
                        break;
                    case telemetrySubType::TEMP:
                        plaintextSubType = "TEMP";
                        break;
                    case telemetrySubType::PRES:
                        plaintextSubType = "PRES";
                        break;
                    case telemetrySubType::MAG:
                        plaintextSubType = "MAG";
                        break;
                    case telemetrySubType::VOLT:
                        plaintextSubType = "VOLT";
                        break;
                    case telemetrySubType::CURR:
                        plaintextSubType = "CURR";
                        break;
                }
                break;
            case packetType::TOGGLE:
                switch (std::get<toggleSubType>(st)) {
                    case toggleSubType::LOCATION:
                        plaintextSubType = "LOCATION";
                        break;
                    case toggleSubType::ALLSENSORS:
                        plaintextSubType = "ALLSENSORS";
                        break;
                    case toggleSubType::ALLMOVEMENT:
                        plaintextSubType = "ALLMOVEMENT";
                        break;
                }
                break;
        }
    };
    sachet(std::string Type, std::string SubType, std::string Payload) : plaintextType(Type), plaintextSubType(SubType), plaintextPayload(Payload) {
        payload = std::vector<uint8_t>(Payload.begin(), Payload.end());
        type = static_cast<packetType>(std::stoi(Type));
        switch (type) {
            case packetType::CONTROL:
                subType = static_cast<controlSubType>(std::stoi(SubType));
                break;
            case packetType::TELEMETRY:
                subType = static_cast<telemetrySubType>(std::stoi(SubType));
                break;
            case packetType::TOGGLE:
                subType = static_cast<toggleSubType>(std::stoi(SubType));
                break;
            case packetType::CHAIN:
                // Handle CHAIN packet type if needed
                break;
        }

    };  
};


struct packet{
    private:
    uint8_t header : 2;
    uint8_t length : 4;
    sachet s;
    uint8_t checksum : 4;
    public:
    serialPacket serialise(){
        std::vector<uint8_t> bytes;
        uint8_t subTypeValue; 
        //This doesnt auto fill bytes appropriately

        uint8_t metaByte1 = (header & 0x03) | ((static_cast<uint8_t>(s.type) & 0x03) << 2) |((length & 0x0F) << 4);
        switch (s.type){
        case packetType::CONTROL:
            subTypeValue = static_cast<uint8_t>(std::get<controlSubType>(s.subType));
            break;
        case packetType::TELEMETRY:
            subTypeValue = static_cast<uint8_t>(std::get<telemetrySubType>(s.subType));
            break;
        case packetType::TOGGLE:
            subTypeValue = static_cast<uint8_t>(std::get<toggleSubType>(s.subType));
            break;
        case packetType::CHAIN:
            // Handle CHAIN packet type if needed
            break;
        }

        uint8_t metaByte2 = (subTypeValue & 0x0F)| ((checksum & 0x0F) << 4);


        bytes.push_back(metaByte1);
        bytes.push_back(metaByte2);
        bytes.insert(bytes.end(),s.payload.begin(),s.payload.end());

        return serialPacket(bytes);
    }
    //packet(sachet s) : type(s.type), subType(s.subType), payload(s.payload), length(s.payload.size()), header(00), checksum(00) {};
    packet(sachet Sachet) : s(Sachet), length(s.payload.size()), header(00), checksum(00) {};
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
