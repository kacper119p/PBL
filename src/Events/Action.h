#pragma once
#include <functional>

class Action
{
    friend bool operator==(const Action& lhs, const Action& rhs);
private:
    std::function<void()> Callback;
    void* OwnerPtr;
    void* CallbackPtr;

public:
    template<typename U>
    Action(U* Owner, void (U::* Method)())
    {
        Callback = std::bind(Method, Owner);
        OwnerPtr = Owner;
        CallbackPtr = Method;
    }

public:
    void Invoke()
    {
        Callback();
    }
};

inline bool operator==(const Action& lhs, const Action& rhs)
{
    return lhs.OwnerPtr == rhs.OwnerPtr && lhs.CallbackPtr == rhs.CallbackPtr;
}