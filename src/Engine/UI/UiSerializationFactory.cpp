#include "UiSerializationFactory.h"

#include "UiImplementations/EmptyUi.h"

namespace Engine::Ui
{
    Ui* UiSerializationFactory::CreateObject(const std::string& TypeName)
    {
        const auto iterator = GetInstance()->Builders.find(TypeName);
        if (iterator == GetInstance()->Builders.end())
        {
            return new EmptyUi();
        }
        return iterator->second->Build();
    }
}
