#pragma once
#include <cstddef>
#include <fstream>
#include <string>

/**
 * @brief Manages the server-side state of a file download.
 *
 * @details Opens a file from the storage/ directory by SHA1 hash and provides
 *          chunk-based reading for streaming to the client.
 */
class FileDownload
{
public:
    /**
     * @brief Construct a FileDownload instance.
     *
     * @param _fileSha1sum The SHA1 hash identifying the file to download.
     */
    FileDownload(const std::string& _fileSha1sum);
    ~FileDownload();

    /**
     * @brief Read the next chunk of data from the file.
     *
     * @param buf Buffer to store the read data.
     * @param maxSize Maximum number of bytes to read.
     * @return size_t Actual number of bytes read. 0 if EOF or error.
     */
    size_t readChunk(char* buf, size_t maxSize);

    /**
     * @brief Check if the entire file has been read.
     *
     * @return true if all data has been read or EOF reached.
     */
    bool isFinished() const;

    /**
     * @brief Get the total file size in bytes.
     *
     * @return size_t The file size.
     */
    size_t getFileSize() const;

    /**
     * @brief Check if the file was opened successfully.
     *
     * @return true if the file exists and is readable.
     */
    bool isValid() const;

private:
    std::string m_fileSha1sum{};
    std::string m_filePath{};
    std::ifstream m_ifs;
    size_t m_fileSize{0};
    size_t m_readOffset{0};
    bool m_finished{false};
};
