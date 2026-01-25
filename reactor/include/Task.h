#pragma once

class Task
{
public:
    Task();
    virtual ~Task() = 0;

    /**
     * @brief Pure virtual functions, that is, the tasks to be executed.
     *
     */
    virtual void process() = 0;
};