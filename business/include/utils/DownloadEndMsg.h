#pragma once

#include <cstddef>

struct DownloadEndMsg
{
    size_t fileSha1sumSize{0};
    char* fileSha1sum{nullptr};
};