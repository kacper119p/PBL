#pragma once
#include <functional>
#include <vector>

#include "Action.h"

namespace Events
{
    /**
     * @brief Event Actions can subscribe to.
     */
    class Event
    {

    private:
        std::vector<Action> Listeners;

    public:
        /**
         * @brief Constructs a new empty Event.
         */
        Event() = default;

    public:
        ~Event() = default;

    public:
        /**
         * @brief Adds a new listener to this Event.
         * @param Listener A listener to add.
         */
        void AddListener(const Action& Listener)
        {
            Listeners.push_back(Listener);
        }

        /**
         * @brief Removes listener from this event.
         * @param Listener A lister to remove.
         */
        void RemoveListener(const Action& Listener)
        {
            std::erase(Listeners, Listener);
        }

        /**
         * @brief Adds a new listener to this Event.
         * @param Listener Listener A lister to Add.
         * @return Reference to this Event.
         */
        Event& operator+=(const Action& Listener)
        {
            AddListener(Listener);
            return *this;
        }

        /**
         * @brief Removes listener from this event.
         * @param Listener A listener to add.
         * @return Reference to this Event.
         */
        Event& operator-=(const Action& Listener)
        {
            RemoveListener(Listener);
            return *this;
        }

        /**
         * @brief Removes all listeners from this event.
         */
        void RemoveAllListeners()
        {
            Listeners.clear();
        }

        /**
         * @brief Calls methods associated with every listener.
         */
        void Invoke()
        {
            for (Action& Listener : Listeners)
            {
                Listener.Invoke();
            }
        }
    };

} // namespace Events
