#pragma once

#include <stdexcept>

namespace Utility
{
    class FileException : public std::runtime_error
    {
    public:
        explicit FileException(const runtime_error& Other);

        explicit FileException(const char* Message);

        explicit FileException(const std::string& Message);
    };
} // Utility
