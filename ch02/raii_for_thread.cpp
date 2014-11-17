#include <thread>
#include <iostream>
#include <stdexcept>

namespace concur
{
/**
 * @brief Provide exception safety by RAII.
 *
 * @note based on listing 2.3
 */
class ThreadGuard
{
    std::thread& t_;
public:
    explicit ThreadGuard(std::thread& thread) :
        t_(thread)
    {}

    ThreadGuard(const ThreadGuard& thread)          = delete;
    ThreadGuard& operator=(const ThreadGuard& rhs)  = delete;

    ~ThreadGuard()
    {
        if(t_.joinable())   t_.join();
        std::cout << "t_ has been join()." << std::endl;
    }
};
}//namespace

template<typename SomeLocalObject>
struct Func
{
    Func(SomeLocalObject& obj):
        object_{obj}
    {}

    void operator ()(){/*do stuff*/}

    SomeLocalObject& object_;
};

void do_something_that_may_throw_exceptions()
{
    try{
        throw std::runtime_error{"I just throw an exception\n"};
    }
    catch (std::exception e)
    {
        std::cout << e.what() << std::endl;
    }
}

int main()
{
    int some_local_object = 0 ;
    Func<int> func{some_local_object};
    std::thread t{func};

    {
        concur::ThreadGuard guard{t};
        do_something_that_may_throw_exceptions();
    }//<--where t join()
}
//! @output
//!
//std::exception
//t_ has been join().