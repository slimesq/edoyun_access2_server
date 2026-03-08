#pragma once

#include <string>
#include "MsgType.h"
#include "LoadStatusType.h"

class TrainHelper
{
public:
    /**
     * @brief Build a complete Train binary message.
     *
     * @details Wire format: [length: size_t (8 bytes)] [msgType: int (4 bytes)] [payload: length bytes]
     *
     * @param type The message type enum value.
     * @param payload The message body to be sent.
     * @return std::string The complete Train binary message ready to send.
     */
    static std::string buildTrainMsg(MsgType type, const std::string& payload);

    /**
     * @brief Serialize a FileStatus response message.
     *
     * @details Builds a Train(FileStatus) message containing the original request type
     *          and the current status. Used to reply to upload/download requests.
     *
     * @param origin The message type of the original request (e.g. UploadBegin, UploadChunk).
     * @param status The current status of the file operation.
     * @return std::string The complete Train(FileStatus) binary message ready to send.
     */
    static std::string serializeFileStatus(MsgType origin, LoadStatusType status);
};
