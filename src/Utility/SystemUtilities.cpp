#include <Windows.h>
#include <sysinfoapi.h>
#include <intrin.h> //MSVC specific
#include <array>
#include "SystemUtilities.h"

namespace Utility
{
    std::string GetCpuInfo()
    {
        std::array<int, 4> integerBuffer = {};
        constexpr size_t sizeofIntegerBuffer = sizeof(int) * integerBuffer.size();

        std::array<char, 64> charBuffer = {};

        constexpr std::array<int64_t, 3> functionIds = {
            0x80000002, //manufacturer
            0x80000003, //model
            0x80000004 //clockspeed
        };

        std::string cpu;

        for (const int id : functionIds)
        {
            __cpuid(integerBuffer.data(), id);
            std::memcpy(charBuffer.data(), integerBuffer.data(), sizeofIntegerBuffer);
            cpu += std::string(charBuffer.data());
        }

        return cpu;
    }

    int GetTotalRamGB()
    {
        uint64_t result;
        GetPhysicallyInstalledSystemMemory(&result);
        return static_cast<int>(result / 1048576ull);
    }
} // Utility
