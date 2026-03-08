#pragma once

#include "MsgType.h"
#include "LoadStatusType.h"

struct FileStatusMsg
{
    MsgType originMsgType{MsgType::UnKnown};
    LoadStatusType statusType{LoadStatusType::error};
};
