#include "FileUploadTask.h"
#include "FileContext.h"
#include "FileUpload.h"
#include "TcpConnection.h"
#include "utils/TrainHelper.h"
#include "utils/UploadBeginMsg.h"
#include "utils/UploadChunkMsg.h"
#include <cstring>
#include <iostream>

FileUploadTask::FileUploadTask(std::shared_ptr<TcpConnection> const& _conn,
                               MsgType _msgType,
                               const std::string& _payload)
    : Task(_conn), m_msgType(_msgType), m_payload(_payload)
{
}

FileUploadTask::~FileUploadTask()
{
}

void FileUploadTask::process()
{
    switch (m_msgType)
    {
    case MsgType::UploadBegin:
        handleUploadBegin();
        break;
    case MsgType::UploadChunk:
        handleUploadChunk();
        break;
    default:
        break;
    }
}

void FileUploadTask::handleUploadBegin()
{
    const char* ptr = m_payload.data();

    UploadBeginMsg msg;
    std::memcpy(&msg.usernameSize, ptr, sizeof(size_t));
    ptr += sizeof(size_t);
    msg.username = const_cast<char*>(ptr);
    ptr += msg.usernameSize;

    std::memcpy(&msg.filenameSize, ptr, sizeof(size_t));
    ptr += sizeof(size_t);
    msg.fileName = const_cast<char*>(ptr);
    ptr += msg.filenameSize;

    std::memcpy(&msg.fileSha1sumSize, ptr, sizeof(size_t));
    ptr += sizeof(size_t);
    msg.fileSha1sum = const_cast<char*>(ptr);
    ptr += msg.fileSha1sumSize;

    std::memcpy(&msg.fileSize, ptr, sizeof(size_t));

    std::string sha1sum(msg.fileSha1sum, msg.fileSha1sumSize);
    std::string username(msg.username, msg.usernameSize);
    std::string filename(msg.fileName, msg.filenameSize);

    std::cout << "UploadBegin: user=" << username
              << " file=" << filename
              << " sha1=" << sha1sum
              << " size=" << msg.fileSize << std::endl;

    auto upload = std::make_shared<FileUpload>(sha1sum, filename, msg.fileSize);
    auto ctx = std::make_shared<FileContext>();
    ctx->setFileUpload(upload);
    m_conn->setUserContext(ctx);

    std::string reply = TrainHelper::serializeFileStatus(
        MsgType::UploadBegin, LoadStatusType::uncompleted);
    m_conn->sendInLoop(reply);
}

void FileUploadTask::handleUploadChunk()
{
    const char* ptr = m_payload.data();

    UploadChunkMsg msg;
    std::memcpy(&msg.usernameSize, ptr, sizeof(size_t));
    ptr += sizeof(size_t);
    msg.username = const_cast<char*>(ptr);
    ptr += msg.usernameSize;

    std::memcpy(&msg.offset, ptr, sizeof(size_t));
    ptr += sizeof(size_t);

    std::memcpy(&msg.dataSize, ptr, sizeof(size_t));
    ptr += sizeof(size_t);
    msg.data = const_cast<char*>(ptr);

    auto userCtx = m_conn->getUserContext();
    if (!userCtx)
    {
        std::cerr << "UploadChunk: no user context" << std::endl;
        std::string reply = TrainHelper::serializeFileStatus(
            MsgType::UploadChunk, LoadStatusType::error);
        m_conn->sendInLoop(reply);
        return;
    }

    auto fileCtx = std::dynamic_pointer_cast<FileContext>(userCtx);
    if (!fileCtx || !fileCtx->getFileUpload())
    {
        std::cerr << "UploadChunk: no file upload context" << std::endl;
        std::string reply = TrainHelper::serializeFileStatus(
            MsgType::UploadChunk, LoadStatusType::error);
        m_conn->sendInLoop(reply);
        return;
    }

    auto upload = fileCtx->getFileUpload();
    upload->writeFile(msg.offset, msg.dataSize, msg.data);

    LoadStatusType status = upload->getStatus();
    if (status == LoadStatusType::error)
    {
        std::string reply = TrainHelper::serializeFileStatus(
            MsgType::UploadChunk, LoadStatusType::error);
        m_conn->sendInLoop(reply);
    }
    else if (status == LoadStatusType::completed)
    {
        std::cout << "Upload completed: " << upload->getFileSha1sum() << std::endl;

        // Broadcast file info to all other clients: [FILE]sha1|filename|filesize
        std::string fileInfo = "[FILE]" + upload->getFileSha1sum()
                             + "|" + upload->getFileName()
                             + "|" + std::to_string(upload->getFileSize());
        std::string broadcastMsg = TrainHelper::buildTrainMsg(MsgType::GroupChat, fileInfo);

        const auto conns = m_conn->getTcpConnections();
        for (auto& [fd, connection] : conns)
        {
            if (fd != m_conn->getFd())
            {
                connection->sendInLoop(broadcastMsg);
            }
        }
    }
}
