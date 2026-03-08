#pragma once
#include "Task.h"
#include "utils/MsgType.h"
#include <string>

/**
 * @brief Task for handling file upload requests (UploadBegin and UploadChunk).
 *
 * @details Dispatches to handleUploadBegin() or handleUploadChunk() based on message type.
 *          Executed asynchronously by the thread pool.
 */
class FileUploadTask : public Task
{
public:
    /**
     * @brief Construct a FileUploadTask.
     *
     * @param _conn The client connection.
     * @param _msgType The message type (UploadBegin or UploadChunk).
     * @param _payload The binary payload of the message.
     */
    FileUploadTask(std::shared_ptr<TcpConnection> const& _conn,
                   MsgType _msgType,
                   const std::string& _payload);
    ~FileUploadTask() override;

    /**
     * @brief Process the upload request based on message type.
     */
    void process() override;

private:
    /**
     * @brief Handle UploadBegin: parse file metadata, create FileUpload and FileContext.
     */
    void handleUploadBegin();

    /**
     * @brief Handle UploadChunk: write chunk data to disk via FileUpload.
     */
    void handleUploadChunk();

    MsgType m_msgType;
    std::string m_payload;
};
