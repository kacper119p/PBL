#include "FileException.h"


Utility::FileException::FileException(const runtime_error& Other) :
    runtime_error(Other)
{
}

Utility::FileException::FileException(const char* Message) :
    runtime_error(Message)
{
}

Utility::FileException::FileException(const std::string& Message) :
    runtime_error(Message)
{
}
