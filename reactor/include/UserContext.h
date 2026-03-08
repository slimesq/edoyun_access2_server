#pragma once

class UserContext
{
public:
    virtual ~UserContext() = 0;
private:
};

inline UserContext::~UserContext() {}