#include "FileUpload.h"
#include <fstream>
#include <sys/stat.h>

FileUpload::FileUpload(std::string const& _fileSha1sum,
                       std::string const& _fileName,
                       size_t _fileSize)
    : m_fileSha1sum(_fileSha1sum),
      m_fileName(_fileName),
      m_filePath("storage/" + _fileSha1sum),
      m_fileSize(_fileSize),
      m_status(LoadStatusType::uncompleted)
{
    ::mkdir("storage", 0755);
}

FileUpload::~FileUpload()
{
}

LoadStatusType FileUpload::getStatus()
{
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_status;
}

void FileUpload::writeFile(size_t _offset, size_t _size, char const* buf)
{
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        if (m_status == LoadStatusType::completed)
        {
            return;
        }
    }
    std::ofstream ofs(m_filePath, std::ios::binary | std::ios::in | std::ios::out);
    if (!ofs.is_open())
    {
        // File does not exist yet, create it
        ofs.open(m_filePath, std::ios::binary | std::ios::out);
    }
    ofs.seekp(_offset);
    ofs.write(buf, _size);

    {
        std::lock_guard<std::mutex> lock(m_mutex);
        if (ofs.fail())
        {
            m_status = LoadStatusType::error;
            return;
        }
        m_currSize += _size;
        if (m_currSize >= m_fileSize)
        {
            m_status = LoadStatusType::completed;
        }
    }
}

const std::string& FileUpload::getFileSha1sum() const
{
    return m_fileSha1sum;
}

const std::string& FileUpload::getFileName() const
{
    return m_fileName;
}

size_t FileUpload::getFileSize() const
{
    return m_fileSize;
}
