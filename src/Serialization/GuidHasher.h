#pragma once
#pragma comment(lib, "Rpcrt4.lib")
#include <windows.h>
#include <rpc.h>

namespace Serialization
{
    /**
     * @brief Used to generate hashes from Guid.
     */
    struct GuidHasher
    {
        size_t operator()(const GUID& Guid) const
        {
            RPC_STATUS status;
            return UuidHash(&const_cast<GUID&>(Guid), &status);
        }
    };
} // Serialization
