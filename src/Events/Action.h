#pragma once
#include <functional>

namespace Events
{
    template<typename T>
    class Action
    {
    private:
        std::function<void(T)> Callback;

    public:
        template<typename U>
        Action(U Owner, void (* Method)(T))
        {
            Callback = std::bind(Method, Owner);
        }

    public:
        void Invoke()
        {
            Callback();
        }
    };
} // Events
