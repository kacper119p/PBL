#pragma once
#include "Cloneable.h"

namespace Utility
{

    template<class T>
    class TCloneable : public Cloneable
    {
    public:
        ~TCloneable() override = default;

        [[nodiscard]] virtual T* CloneAsConcrete() const = 0;

        [[nodiscard]] TCloneable* CloneAsTCloneable() const
        {
            return CloneAsConcrete();
        }

        [[nodiscard]] Cloneable* CloneAsCloneable() const override
        {
            return CloneAsConcrete();
        }
    };
}
