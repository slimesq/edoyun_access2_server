#pragma once

#include <memory>
#include "TcpConnection.h"
#include "TcpServer.h"
#include "ThreadPool.h"
#include "utils/NonCopyable.h"

/**
 * @brief Application-level server that dispatches Train protocol messages to task handlers.
 *
 * @details Reads Train binary header from each connection, determines the message type,
 *          and creates the corresponding Task (MyTask/FileUploadTask/FileDownloadTask)
 *          for execution in the thread pool.
 */
class ChatServer : public NonCopyable
{
public:
    /**
     * @brief Construct a ChatServer.
     *
     * @param _threadNum Number of worker threads in the thread pool.
     * @param _queSize Maximum task queue size.
     * @param _ip The IP address to bind.
     * @param _port The port to listen on.
     */
    ChatServer(size_t _threadNum, size_t _queSize, std::string const& _ip, unsigned short _port);
    ~ChatServer();

    /**
     * @brief Start the thread pool and TCP server.
     */
    void start();

    /**
     * @brief Stop the TCP server and thread pool.
     */
    void stop();

    /**
     * @brief Callback when a new client connection is established.
     *
     * @param _conn The new connection.
     */
    void onNewConnection(std::shared_ptr<TcpConnection> const& _conn);

    /**
     * @brief Callback when a connection has data to read.
     *
     * @details Reads the Train header (length + msgType), reads the payload,
     *          and dispatches to the appropriate Task based on msgType.
     *
     * @param _conn The readable connection.
     */
    void onMessage(std::shared_ptr<TcpConnection> const& _conn);

    /**
     * @brief Callback when a client connection is closed.
     *
     * @param _conn The closed connection.
     */
    void onClose(std::shared_ptr<TcpConnection> const& _conn);

private:
    ThreadPool m_pool;
    TcpServer m_server;
};
