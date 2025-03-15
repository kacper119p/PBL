#pragma once

#include <vector>

#include "Action.h"

namespace Events
{
    class Event
    {

    private:
        std::vector<Action> Listeners;

    public:
        Event() = default;

    public:
        ~Event() = default;

    public:
        void AddListener(const Action& Listener)
        {
            Listeners.push_back(Listener);
        }

        void RemoveListener(const Action& Listener)
        {
            std::erase( Listeners, Listener);
        }

        Event& operator+=(const Action& Listener)
        {
            AddListener(Listener);
            return *this;
        }

        Event& operator-=(const Action& Listener)
        {
            RemoveListener(Listener);
            return *this;
        }

        void RemoveAllListeners()
        {
            Listeners.clear();
        }

        void Invoke()
        {
            for (Action& Listener : Listeners)
            {
                Listener.Invoke();
            }
        }
    };

} // namespace Events
