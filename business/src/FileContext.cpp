#include "FileContext.h"
#include "FileUpload.h"
#include "FileDownload.h"

FileContext::~FileContext()
{
}

void FileContext::setFileUpload(std::shared_ptr<FileUpload> _upload)
{
    m_fileUpload = std::move(_upload);
}

std::shared_ptr<FileUpload> FileContext::getFileUpload() const
{
    return m_fileUpload;
}

void FileContext::setFileDownload(std::shared_ptr<FileDownload> _download)
{
    m_fileDownload = std::move(_download);
}

std::shared_ptr<FileDownload> FileContext::getFileDownload() const
{
    return m_fileDownload;
}
