#include "utils/TrainHelper.h"
#include <cstring>

std::string TrainHelper::buildTrainMsg(MsgType type, const std::string& payload)
{
    size_t length = payload.size();
    int msgType = static_cast<int>(type);

    std::string msg;
    msg.resize(sizeof(size_t) + sizeof(int) + length);

    char* ptr = msg.data();
    std::memcpy(ptr, &length, sizeof(size_t));
    ptr += sizeof(size_t);
    std::memcpy(ptr, &msgType, sizeof(int));
    ptr += sizeof(int);
    std::memcpy(ptr, payload.data(), length);

    return msg;
}

std::string TrainHelper::serializeFileStatus(MsgType origin, LoadStatusType status)
{
    std::string payload;
    payload.resize(sizeof(MsgType) + sizeof(LoadStatusType));

    char* ptr = payload.data();
    std::memcpy(ptr, &origin, sizeof(MsgType));
    ptr += sizeof(MsgType);
    std::memcpy(ptr, &status, sizeof(LoadStatusType));

    return buildTrainMsg(MsgType::FileStatus, payload);
}
