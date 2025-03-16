#pragma once
#include <functional>
#include <combaseapi.h>

namespace Events
{
    /**
     * @brief Class representing callback to an event.
     * @tparam T Argument type callback will be called with.
     */
    template<typename T>
    class TAction
    {
    private:
        GUID Id;
        std::function<void(T)> Callback;

    public:
        TAction() = delete;

        /**
         * @brief Constructs new Action bound to an owner and its method.
         * @tparam U A Type of owner.
         * @param Owner An Object to call method on.
         * @param Method A Method to call.
         */
        template<class U>
        TAction(U* Owner, void (U::* Method)(T)) : // NOLINT(*-pro-type-member-init)
            Callback([Owner, Method](T Arg) { (Owner->*Method)(Arg); })
        {
            (void) CoCreateGuid(&Id); // ID initialized here.
        }

    public:
        /**
         * @brief Calls method associated with this Action.
         * @param Argument Argument method will be called with.
         */
        void Invoke(T Argument)
        {
            Callback(Argument);
        }

    public:
        /**
         * @brief Compares two Actions and returns true if they're the same Action.
         * @param Other An Action to compare this Action to.
         * @return True if other is equal. False otherwise.
         */
        bool operator==(const TAction<T>& Other) const
        {
            return Id == Other.Id;
        }
    };
} // Events
