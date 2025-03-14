#pragma once

#include <stdexcept>

namespace Utility
{
    class FileException : public std::runtime_error
    {
    public:
        explicit FileException(const runtime_error& other);

        explicit FileException(const char* message);

        explicit FileException(const std::string& message);
    };
} // Utility
