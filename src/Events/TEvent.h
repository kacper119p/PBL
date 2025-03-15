#pragma once
#include <functional>
#include <vector>

#include "TAction.h"

namespace Events
{
    template<typename T>
    class TEvent
    {

    private:
        std::vector<TAction<T>> Listeners;

    public:
        TEvent() = default;

    public:
        ~TEvent() = default;

    public:
        void AddListener(const TAction<T>& Listener)
        {
            Listeners.push_back(Listener);
        }

        void RemoveListener(const TAction<T>& Listener)
        {
            std::erase(Listeners, Listener);
        }

        TEvent& operator+=(const TAction<T>& Listener)
        {
            AddListener(Listener);
            return *this;
        }

        TEvent& operator-=(const TAction<T>& Listener)
        {
            RemoveListener(Listener);
            return *this;
        }

        void RemoveAllListeners()
        {
            Listeners.clear();
        }

        void Invoke(T Argument)
        {
            for (TAction<T>& Listener : Listeners)
            {
                Listener.Invoke(Argument);
            }
        }
    };

} // namespace Events
