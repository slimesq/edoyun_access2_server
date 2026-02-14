#pragma once

#include "MsgType.h"
#include <cstddef>

struct Train
{
    size_t length{0};
    MsgType msgType{MsgType::UnKnown};
    char* data{nullptr};
};