//!
//! @author Yue Wang
//! @date   19.11.2014
//!

#ifndef THREADSAFE_STACK_HPP
#define THREADSAFE_STACK_HPP

#include <exception>
#include <memory>
#include <mutex>
#include <stack>

namespace para {

/**
 * @brief   EmptyStack
 *          inherited from std::exception
 *
 * @modifications
 *          Listing 3.4 provides declaration only, here's my implementation.
 */
class EmptyStackException : std::exception
{
public:
    //! default Ctor
    EmptyStackException():
        msg_{"para exception : empty stack"}
    {}

    //! Ctor
    explicit EmptyStackException(const char* msg):
        msg_{msg}
    {}

    //! Ctor
    explicit EmptyStackException(const std::string& msg):
        msg_{msg.c_str()}
    {}

    //! move Ctor
    explicit EmptyStackException(std::string&& msg)noexcept:
        msg_{std::move(msg).c_str()}
    {}

    /**
     * @brief what
     * @note        specifier `throw()` is extensively considered as uselessness and
     *              even a bad practise. Listing 3.4 using it ,however, here
     *              follows this style.
     */
    virtual const char* what ()  const throw() override
    {
        return msg_.c_str();
    }

    ~EmptyStackException(){}

private:
    const std::string msg_;
};

/**
 * @brief   ThreadsafeStack
 * @note    based on Listing 3.4
 * @modifications
 *      1. alias for std::lock_guard<std::mutex>
 *      2. alias for std::shared_ptr<Value>
 *      3. a wrapper for exception throwing
 */
template<typename Value>
class ThreadsafeStack
{
    using Guard         =   std::lock_guard<std::mutex>;
public:
    using SharedPointer =   std::shared_ptr<Value>;

    ThreadsafeStack(){}

    //! copy Ctor
    ThreadsafeStack(const ThreadsafeStack& other)
    {
        Guard g{other.mutex_};
        data_ = other.data_;
    }

    ThreadsafeStack& operator = (const ThreadsafeStack&) = delete;

    //! push
    void push(Value new_val)
    {
        Guard g{mutex_};
        data_.push(new_val);
    }

    //! pop returnning shared pointer
    SharedPointer pop()
    {
        Guard g{mutex_};
        throw_exception_if_empty();
        const auto pointer = std::make_shared<Value>(data_.top());
        data_.pop();

        return pointer;
    }

    //! pop with reference as argument
    void pop(Value& dest)
    {
        Guard g{mutex_};
        throw_exception_if_empty();
        dest = data_.top();
        data_.pop();
    }

    //! empty
    bool empty()const
    {
        Guard g{mutex_};
        return data_.empty();
    }

private:
    std::stack<Value> data_;
    mutable std::mutex mutex_;

    //! wrapper for exception throwing
    void throw_exception_if_empty()
    {
        if(data_.empty())   throw para::EmptyStackException{};
    }
};
}//namespace
#endif // THREADSAFE_STACK_HPP
