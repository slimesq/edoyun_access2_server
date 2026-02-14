#pragma once

#include <cstddef>

struct DownloadBeginMsg
{
    size_t fileSha1sumSize{0};
    char* fileSha1sum{nullptr};
};