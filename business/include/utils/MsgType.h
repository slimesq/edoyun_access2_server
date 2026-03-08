#pragma once
enum class MsgType : int
{
    GroupChat = 1001,
    UploadBegin = 1002,
    UploadChunk = 1003,
    DownloadBegin = 1004,
    DownloadEnd = 1005,
    UnKnown = 1006,
    DownloadChunk = 1007,
    FileStatus = 1008
};