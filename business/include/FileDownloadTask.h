#pragma once
#include "Task.h"
#include <string>

/**
 * @brief Task for handling file download requests (DownloadBegin).
 *
 * @details Reads the requested file in 4KB chunks and sends each chunk as
 *          Train(DownloadChunk), followed by a Train(DownloadEnd) message.
 *          Executed asynchronously by the thread pool.
 */
class FileDownloadTask : public Task
{
public:
    /**
     * @brief Construct a FileDownloadTask.
     *
     * @param _conn The client connection.
     * @param _payload The binary payload containing the file SHA1 hash.
     */
    FileDownloadTask(std::shared_ptr<TcpConnection> const& _conn,
                     const std::string& _payload);
    ~FileDownloadTask() override;

    /**
     * @brief Process the download request: read file and send chunks to client.
     */
    void process() override;

private:
    std::string m_payload;
};
