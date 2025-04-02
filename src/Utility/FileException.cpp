#include "FileException.h"


Utility::FileException::FileException(const runtime_error& other) :
    runtime_error(other)
{
}

Utility::FileException::FileException(const char* message) :
    runtime_error(message)
{
}

Utility::FileException::FileException(const std::string& message) :
    runtime_error(message)
{
}
