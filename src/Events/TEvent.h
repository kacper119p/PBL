#pragma once
#include <functional>
#include <vector>

#include "TAction.h"

namespace Events
{
    /**
     * @brief Event Actions can subscribe to.
     * @tparam T Argument type listeners will be called with.
     */
    template<typename T>
    class TEvent
    {

    private:
        std::vector<TAction<T>> Listeners;

    public:
        /**
         * @brief Constructs a new empty Event.
         */
        TEvent() = default;

    public:
        ~TEvent() = default;

    public:
        /**
         * @brief Adds a new listener to this Event.
         * @param Listener A listener to add.
         */
        void AddListener(const TAction<T>& Listener)
        {
            Listeners.push_back(Listener);
        }

        /**
         * @brief Removes listener from this event.
         * @param Listener A lister to remove.
         */
        void RemoveListener(const TAction<T>& Listener)
        {
            std::erase(Listeners, Listener);
        }

        /**
         * @brief Adds a new listener to this Event.
         * @param Listener Listener A lister to Add.
         * @return Reference to this Event.
         */
        TEvent& operator+=(const TAction<T>& Listener)
        {
            AddListener(Listener);
            return *this;
        }

        /**
         * @brief Removes listener from this event.
         * @param Listener A listener to add.
         * @return Reference to this Event.
         */
        TEvent& operator-=(const TAction<T>& Listener)
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
         * @brief Calls methods associated with every listener using provided argument.
         * @param Argument Argument method will be called with.
         */
        void Invoke(T Argument)
        {
            for (TAction<T>& Listener : Listeners)
            {
                Listener.Invoke(Argument);
            }
        }
    };

} // namespace Events
