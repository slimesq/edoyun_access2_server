#include "FileDownload.h"
#include <sys/stat.h>

FileDownload::FileDownload(const std::string& _fileSha1sum)
    : m_fileSha1sum(_fileSha1sum),
      m_filePath("storage/" + _fileSha1sum)
{
    struct stat st;
    if (::stat(m_filePath.c_str(), &st) == 0)
    {
        m_fileSize = static_cast<size_t>(st.st_size);
    }
    m_ifs.open(m_filePath, std::ios::binary);
}

FileDownload::~FileDownload()
{
    if (m_ifs.is_open())
    {
        m_ifs.close();
    }
}

size_t FileDownload::readChunk(char* buf, size_t maxSize)
{
    if (m_finished || !m_ifs.is_open())
    {
        return 0;
    }
    m_ifs.read(buf, maxSize);
    size_t bytesRead = static_cast<size_t>(m_ifs.gcount());
    m_readOffset += bytesRead;
    if (bytesRead == 0 || m_readOffset >= m_fileSize)
    {
        m_finished = true;
    }
    return bytesRead;
}

bool FileDownload::isFinished() const
{
    return m_finished;
}

size_t FileDownload::getFileSize() const
{
    return m_fileSize;
}

bool FileDownload::isValid() const
{
    return m_ifs.is_open() && m_fileSize > 0;
}
