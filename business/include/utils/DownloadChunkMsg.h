#pragma once

#include <cstddef>

struct DownloadChunkMsg
{
    size_t fileSha1sumSize{0};
    char* fileSha1sum{nullptr};
    size_t offset{0};
    size_t dataSize{0};
    char* data{nullptr};
};
