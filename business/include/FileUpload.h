#pragma once
#include <cstddef>
#include <mutex>
#include "utils/LoadStatusType.h"
class FileUpload
{
public:
    FileUpload(const std::string& _fileSha1sum, size_t _fileSize);
    ~FileUpload();
    LoadStatusType getStatus();
    void writeFile(size_t _offset,size_t _size,const char* buf);

private:
    std::string m_fileSha1sum{};
    size_t m_fileSize{0};
    size_t m_currSize{0};
    std::mutex m_mutex{};
    LoadStatusType m_status{LoadStatusType::uncompleted};
};