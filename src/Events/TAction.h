#pragma once
#include <functional>

namespace Events
{
    template<typename T>
    class TAction
    {
    private:
        std::function<void(T)> Callback;
        void* OwnerPtr;
        void* CallbackPtr;

    public:
        template<typename U>
        TAction(U* Owner, void (U::* Method)(T))
        {
            Callback = std::bind(Method, Owner);
            OwnerPtr = Owner;
            CallbackPtr = Method;
        }

    public:
        void Invoke(T Argument)
        {
            Callback(Argument);;
        }
    };

    template<typename T>
    inline bool operator==(const TAction<T>& lhs, const TAction<T>& rhs)
    {
        return lhs.OwnerPtr == rhs.OwnerPtr && lhs.CallbackPtr == rhs.CallbackPtr;
    }
} // Events
