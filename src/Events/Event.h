#pragma once
#include <functional>
#include <vector>

#include "Action.h"

namespace Events
{
    template<typename T>
    class Event
    {

    private:
        std::vector<Action<T>> Listeners;

    public:
        Event() = default;

    public:
        ~Event() = default;

    public:
        void AddListener(const Action<T>& Listener)
        {
            Listeners.push_back(Listener);
        }

        void RemoveListener(const Action<T>& Listener)
        {
            std::erase(Listeners.begin(), Listeners.end(), Listener);
        }

        Event& operator+=(const Action<T>& Listener)
        {
            AddListener(Listener);
            return this;
        }

        Event& operator-=(const Action<T>& Listener)
        {
            RemoveListener(Listener);
            return this;
        }

        void Invoke()
        {
            for (const Action<T>& Listener : Listeners)
            {
                Listener.Invoke();
            }
        }
    };

} // namespace Events
