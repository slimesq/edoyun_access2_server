#pragma once
#include <cstddef>
#include <mutex>
#include <string>
#include "utils/LoadStatusType.h"

/**
 * @brief Manages the server-side state of a file upload.
 *
 * @details Receives file data in chunks and writes them to the storage/ directory,
 *          using the SHA1 hash as the filename. Tracks upload progress and status
 *          (uncompleted/completed/error) in a thread-safe manner.
 */
class FileUpload
{
public:
    /**
     * @brief Construct a FileUpload instance.
     *
     * @param _fileSha1sum The SHA1 hash of the file, used as the filename on disk.
     * @param _fileName The original filename, used for broadcasting file info on completion.
     * @param _fileSize The total expected file size in bytes.
     */
    FileUpload(const std::string& _fileSha1sum,
               const std::string& _fileName,
               size_t _fileSize);
    ~FileUpload();

    LoadStatusType getStatus();
    void writeFile(size_t _offset, size_t _size, const char* buf);
    const std::string& getFileSha1sum() const;
    const std::string& getFileName() const;
    size_t getFileSize() const;

private:
    std::string m_fileSha1sum{};
    std::string m_fileName{};
    std::string m_filePath{};
    size_t m_fileSize{0};
    size_t m_currSize{0};
    std::mutex m_mutex{};
    LoadStatusType m_status{LoadStatusType::uncompleted};
};
