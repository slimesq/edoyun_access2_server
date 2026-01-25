#include <cstdlib>
#include <ctime>
#include <iostream>
#include "MyTask.h"
#include <unistd.h>
MyTask::MyTask()
{
}

MyTask::~MyTask()
{
}

// Tasks need to be added based on the specific business logic.
void MyTask::process()
{
    ::srand(::clock());              // seed the random number generator
    int number{::rand() % 100 + 1};  // random sleep time between 1 and 5 seconds
    std::cout << "MyTask number =  " << number << std::endl;
    ::sleep(1);
}