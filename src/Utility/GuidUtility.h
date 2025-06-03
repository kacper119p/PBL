#pragma once

#pragma comment(lib, "Ole32.lib")
#include <windows.h>
#include <combaseapi.h>

#include "AssertionsUtility.h"

namespace Utility
{
    [[nodiscard]] inline GUID GenerateGuid()
    {
        GUID out;
        const HRESULT result = CoCreateGuid(&out); //ID Initialized here.
        CHECK_MESSAGE(result == S_OK, "Failed to initialize GUID")
        return out;
    }

}
