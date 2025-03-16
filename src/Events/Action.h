#pragma once
#include <functional>
#include <combaseapi.h>

namespace Events
{
    /**
     * @brief Class representing callback to an event.
     */
    class Action
    {
    private:
        GUID Id;
        std::function<void()> Callback;

    public:
        Action() = delete;

        /**
         * @brief Constructs new Action bound to an owner and its method.
         * @tparam U A Type of owner.
         * @param Owner An Object to call method on.
         * @param Method A Method to call.
         */
        template<class U>
        Action(U* Owner, void (U::* Method)()) : // NOLINT(*-pro-type-member-init)
            Callback([Owner, Method]() { (Owner->*Method)(); })
        {
            (void) CoCreateGuid(&Id); // ID initialized here.
        }

    public:
        /**
         * @brief Calls method associated with this Action.
         */
        void Invoke()
        {
            Callback();
        }

    public:
        /**
         * @brief Compares two Actions and returns true if they're the same Action.
         * @param Other An Action to compare this Action to.
         * @return True if other is equal. False otherwise.
         */
        bool operator==(const Action& Other) const
        {
            return Id == Other.Id;
        }
    };
} // Events
