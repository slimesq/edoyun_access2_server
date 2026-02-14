#pragma once
#include <cstddef>
struct UploadChunkMsg
{
    size_t usernameSize{0};
    char* username{nullptr};
    size_t dataSize{0};
    char* data{nullptr};
};