#pragma once

#include <cstddef>
struct UploadBeginMsg
{
    size_t usernameSize{0};
    char* username{nullptr};
    size_t filenameSize{0};
    char* fileName{nullptr};
    size_t fileSha1sumSize{0};
    char* fileSha1sum{nullptr};
};