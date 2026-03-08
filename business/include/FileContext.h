#pragma once
#include <memory>
#include "UserContext.h"

class FileUpload;
class FileDownload;

/**
 * @brief User context for file transfer operations.
 *
 * @details Attached to a TcpConnection to maintain file upload/download state
 *          across multiple message exchanges within the same connection.
 */
class FileContext : public UserContext
{
public:
    ~FileContext() override;

    /**
     * @brief Set the FileUpload object for the current connection.
     *
     * @param _upload The FileUpload instance managing the upload state.
     */
    void setFileUpload(std::shared_ptr<FileUpload> _upload);

    /**
     * @brief Get the FileUpload object for the current connection.
     *
     * @return std::shared_ptr<FileUpload> The FileUpload instance, or nullptr if not set.
     */
    std::shared_ptr<FileUpload> getFileUpload() const;

    /**
     * @brief Set the FileDownload object for the current connection.
     *
     * @param _download The FileDownload instance managing the download state.
     */
    void setFileDownload(std::shared_ptr<FileDownload> _download);

    /**
     * @brief Get the FileDownload object for the current connection.
     *
     * @return std::shared_ptr<FileDownload> The FileDownload instance, or nullptr if not set.
     */
    std::shared_ptr<FileDownload> getFileDownload() const;

private:
    std::shared_ptr<FileUpload> m_fileUpload{nullptr};
    std::shared_ptr<FileDownload> m_fileDownload{nullptr};
};
