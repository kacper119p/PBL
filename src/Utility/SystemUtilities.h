#pragma once

#include <string>

namespace Utility
{
    /**
     * @brief Retrieves information about CPU.
     * Requires MSVC compiler <br>
     * https://stackoverflow.com/questions/850774/how-to-determine-the-hardware-cpu-and-ram-on-a-machine
     * @return String containing manufacturer, model and clockspeed of CPU
     */
    [[nodiscard]] std::string GetCpuInfo();

    /**
     * @brief Function to access currently installed RAM using WinApi.
     * @return Currently installed RAM in Gibibytes.
     */
    [[nodiscard]] int GetTotalRamGB();
} // Utility
