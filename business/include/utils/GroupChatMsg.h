#pragma once

#include <cstddef>

struct GroupChatMsg
{
    size_t usernameSize{};
    char* username{nullptr};
    size_t msgSize;
    char* msg{nullptr};
};