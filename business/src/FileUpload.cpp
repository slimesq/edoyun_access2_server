#include "FileUpload.h"
#include <fstream>

FileUpload::FileUpload(std::string const& _fileSha1sum, size_t _fileSize)
    : m_fileSha1sum(_fileSha1sum), m_fileSize(_fileSize),m_status(LoadStatusType::uncompleted)
{
}

FileUpload::~FileUpload()
{
}

LoadStatusType FileUpload::getStatus()
{
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
    std::ofstream ofs(m_fileSha1sum, std::ios::binary | std::ios::app | std::ios::out);
    ofs.seekp(_offset);
    ofs.write(buf, _size);

    {
        std::lock_guard<std::mutex> lock(m_mutex);
        if(ofs.fail())
        {
            m_status = LoadStatusType::error;
            return;
        }
        m_currSize += _size;
        if (m_currSize == m_fileSize)
        {
            m_status = LoadStatusType::completed;
        }
    }
}