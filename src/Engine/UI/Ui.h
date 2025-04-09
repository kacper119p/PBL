#pragma once
#include <string>

#include "UiElement.h"
#include "UiSerializationRaii.h"
#include "Engine/Components/Interfaces/IUpdateable.h"

#define SERIALIZATION_EXPORT_UI(__CLASS__)\
public:\
inline static const std::string TypeName = std::string(#__CLASS__);\
\
[[nodiscard]] std::string GetType() const override\
{\
return TypeName;\
}\
\
private:\
static inline const UiSerializationRaii<__CLASS__> RaiiHandle = UiSerializationRaii<__CLASS__>(#__CLASS__);

namespace Engine::Ui
{
    class Ui : IUpdateable
    {
    private:
        std::vector<UiElement*> UiElements;

    public:
        Ui();

    public:
        ~Ui() override;

    public:
        template<class T>
        T* AddElement(UiElement* Parent)
        {
            static_assert(std::is_base_of_v<UiElement, T>, "T must be derived from UiElement.");
            T* newElement = new T();
            if (Parent != nullptr)
            {
                newElement->GetRect().SetParent(&Parent->GetRect());
            }
            UiElements.emplace_back(newElement);
            return newElement;
        }

    public:
        void Render() const;

        [[nodiscard]] virtual std::string GetType() const = 0;

        virtual void Update(float DeltaTime) = 0;
    };
}
