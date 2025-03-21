#pragma once

#include <rpc.h>

namespace Serialization
{

    struct GuidHasher
    {
        size_t operator()(const GUID& Guid) const
        {
            RPC_STATUS status;
            return UuidHash(&const_cast<GUID&>(Guid), &status);
        }
    };
} // Serialization
