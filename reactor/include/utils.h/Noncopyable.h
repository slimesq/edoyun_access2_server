#pragma once
class Noncopyable
{
public:
    Noncopyable() = default;

    Noncopyable(Noncopyable const&) = delete;
    Noncopyable& operator=(Noncopyable const&) = delete;
    Noncopyable(Noncopyable const&&) = delete;
    Noncopyable& operator=(Noncopyable const&&) = delete;
};