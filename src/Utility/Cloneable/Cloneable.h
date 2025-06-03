#pragma once

namespace Utility
{

    class Cloneable
    {
    public:
        virtual ~Cloneable() = default;

    public:
        [[nodiscard]] virtual Cloneable* CloneAsCloneable() const = 0;
    };
}
