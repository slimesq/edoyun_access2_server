#include "FileDownloadTask.h"
#include "FileDownload.h"
#include "TcpConnection.h"
#include "utils/TrainHelper.h"
#include "utils/DownloadBeginMsg.h"
#include <cstring>
#include <iostream>

static const size_t CHUNK_SIZE = 4096;

FileDownloadTask::FileDownloadTask(std::shared_ptr<TcpConnection> const& _conn,
                                   const std::string& _payload)
    : Task(_conn), m_payload(_payload)
{
}

FileDownloadTask::~FileDownloadTask()
{
}

void FileDownloadTask::process()
{
    const char* ptr = m_payload.data();

    DownloadBeginMsg msg;
    std::memcpy(&msg.fileSha1sumSize, ptr, sizeof(size_t));
    ptr += sizeof(size_t);
    msg.fileSha1sum = const_cast<char*>(ptr);

    std::string sha1sum(msg.fileSha1sum, msg.fileSha1sumSize);

    std::cout << "DownloadBegin: sha1=" << sha1sum << std::endl;

    FileDownload download(sha1sum);
    if (!download.isValid())
    {
        std::cerr << "DownloadBegin: file not found: " << sha1sum << std::endl;
        std::string reply = TrainHelper::serializeFileStatus(
            MsgType::DownloadBegin, LoadStatusType::error);
        m_conn->sendInLoop(reply);
        return;
    }

    char buf[CHUNK_SIZE];
    size_t offset = 0;
    while (!download.isFinished())
    {
        size_t bytesRead = download.readChunk(buf, CHUNK_SIZE);
        if (bytesRead == 0)
        {
            break;
        }

        // Build DownloadChunk payload: sha1sumSize + sha1sum + offset + dataSize + data
        std::string payload;
        size_t sha1Size = sha1sum.size();
        payload.resize(sizeof(size_t) + sha1Size + sizeof(size_t) + sizeof(size_t) + bytesRead);

        char* p = payload.data();
        std::memcpy(p, &sha1Size, sizeof(size_t));
        p += sizeof(size_t);
        std::memcpy(p, sha1sum.data(), sha1Size);
        p += sha1Size;
        std::memcpy(p, &offset, sizeof(size_t));
        p += sizeof(size_t);
        std::memcpy(p, &bytesRead, sizeof(size_t));
        p += sizeof(size_t);
        std::memcpy(p, buf, bytesRead);

        std::string chunkMsg = TrainHelper::buildTrainMsg(MsgType::DownloadChunk, payload);
        m_conn->sendInLoop(chunkMsg);

        offset += bytesRead;
    }

    // Send DownloadEnd
    std::string endPayload;
    size_t sha1Size = sha1sum.size();
    endPayload.resize(sizeof(size_t) + sha1Size);
    char* p = endPayload.data();
    std::memcpy(p, &sha1Size, sizeof(size_t));
    p += sizeof(size_t);
    std::memcpy(p, sha1sum.data(), sha1Size);

    std::string endMsg = TrainHelper::buildTrainMsg(MsgType::DownloadEnd, endPayload);
    m_conn->sendInLoop(endMsg);

    std::cout << "Download completed: " << sha1sum << std::endl;
}
