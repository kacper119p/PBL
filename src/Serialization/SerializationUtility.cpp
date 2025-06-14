#include "SerializationUtility.h"

#include <atlbase.h>
#include <locale>

#include "Engine/Textures/Texture.h"
#include "Engine/Textures/TextureManager.h"
#include "Materials/MaterialManager.h"
#include "Models/ModelManager.h"
#include "Models/Model.h"
#include "Shaders/ShaderManager.h"
#include "Shaders/ShaderSourceFiles.h"
#include "Serialization/SerializedObject.h"

namespace
{
    constexpr int GUID_STRING_SIZE = 39; // GUID is 36 chars + null terminator

    void GuidToStr(const GUID& Guid, char* Buffer)
    {
        snprintf(Buffer, GUID_STRING_SIZE * sizeof(char),
                 "{%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x}",
                 Guid.Data1, Guid.Data2, Guid.Data3,
                 Guid.Data4[0], Guid.Data4[1], Guid.Data4[2], Guid.Data4[3],
                 Guid.Data4[4], Guid.Data4[5], Guid.Data4[6], Guid.Data4[7]);
    }

    void StrToGuid(const std::string& str, GUID* guid)
    {
        sscanf_s(str.c_str(),
                 "{%8x-%4hx-%4hx-%2hhx%2hhx-%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx}",
                 &guid->Data1, &guid->Data2, &guid->Data3,
                 &guid->Data4[0], &guid->Data4[1], &guid->Data4[2], &guid->Data4[3],
                 &guid->Data4[4], &guid->Data4[5], &guid->Data4[6], &guid->Data4[7]);
    }
}

namespace Serialization
{
    rapidjson::Value Serialize(const int Value, rapidjson::Document::AllocatorType& Allocator)
    {
        rapidjson::Value object(rapidjson::kNumberType);
        object.SetInt(Value);
        return object;
    }

    rapidjson::Value Serialize(const float Value, rapidjson::Document::AllocatorType& Allocator)
    {
        rapidjson::Value object(rapidjson::kNumberType);
        object.SetFloat(Value);
        return object;
    }

    rapidjson::Value Serialize(const bool Value, rapidjson::Document::AllocatorType& Allocator)
    {
        rapidjson::Value object(rapidjson::kNumberType);
        object.SetBool(Value);
        return object;
    }

    rapidjson::Value Serialize(const glm::quat& Value, rapidjson::Document::AllocatorType& Allocator)
    {
        rapidjson::Value object(rapidjson::kObjectType);
        object.AddMember("x", Value.x, Allocator);
        object.AddMember("y", Value.y, Allocator);
        object.AddMember("z", Value.z, Allocator);
        object.AddMember("w", Value.w, Allocator);
        return object;
    }

    rapidjson::Value Serialize(const glm::vec4& Value, rapidjson::Document::AllocatorType& Allocator)
    {
        rapidjson::Value object(rapidjson::kObjectType);
        object.AddMember("x", Value.x, Allocator);
        object.AddMember("y", Value.y, Allocator);
        object.AddMember("z", Value.z, Allocator);
        object.AddMember("w", Value.w, Allocator);
        return object;
    }

    rapidjson::Value Serialize(const glm::vec3& Value, rapidjson::Document::AllocatorType& Allocator)
    {
        rapidjson::Value object(rapidjson::kObjectType);
        object.AddMember("x", Value.x, Allocator);
        object.AddMember("y", Value.y, Allocator);
        object.AddMember("z", Value.z, Allocator);
        return object;
    }

    rapidjson::Value Serialize(const glm::vec2& Value, rapidjson::Document::AllocatorType& Allocator)
    {
        rapidjson::Value object(rapidjson::kObjectType);
        object.AddMember("x", Value.x, Allocator);
        object.AddMember("y", Value.y, Allocator);
        return object;
    }

    rapidjson::Value Serialize(const Engine::Texture& Value, rapidjson::Document::AllocatorType& Allocator)
    {
        rapidjson::Value object(rapidjson::kStringType);
        const std::string path = Engine::TextureManager::GetTexturePath(Value);
        object.SetString(path.c_str(), static_cast<rapidjson::SizeType>(path.length()), Allocator);
        return object;
    }

    rapidjson::Value Serialize(const Models::Model* Value, rapidjson::Document::AllocatorType& Allocator)
    {
        rapidjson::Value object(rapidjson::kStringType);
        const std::string path = Models::ModelManager::GetModelPath(Value);
        object.SetString(path.c_str(), static_cast<rapidjson::SizeType>(path.length()), Allocator);
        return object;
    }

    rapidjson::Value Serialize(const Models::ModelAnimated* Value, rapidjson::Document::AllocatorType& Allocator)
    {
        rapidjson::Value object(rapidjson::kStringType);
        const std::string path = Models::ModelManager::GetAnimatedModelPath(Value);
        object.SetString(path.c_str(), static_cast<rapidjson::SizeType>(path.length()), Allocator);
        return object;
    }

    rapidjson::Value Serialize(const Models::Animation* Value, rapidjson::Document::AllocatorType& Allocator)
    {
        rapidjson::Value object(rapidjson::kStringType);
        const std::string path = Models::ModelManager::GetAnimationPath(Value);
        object.SetString(path.c_str(), static_cast<rapidjson::SizeType>(path.length()), Allocator);
        return object;
    }

    rapidjson::Value Serialize(const std::string& Value, rapidjson::Document::AllocatorType& Allocator)
    {
        rapidjson::Value object(rapidjson::kStringType);
        object.SetString(Value.c_str(), static_cast<rapidjson::SizeType>(Value.length()), Allocator);
        return object;
    }

    rapidjson::Value Serialize(const GUID& Guid, rapidjson::Document::AllocatorType& Allocator)
    {
        rapidjson::Value object(rapidjson::kStringType);
        char guidStr[GUID_STRING_SIZE];
        GuidToStr(Guid, guidStr);
        object.SetString(guidStr, static_cast<rapidjson::SizeType>(GUID_STRING_SIZE), Allocator);
        return object;
    }

    rapidjson::Value Serialize(const SerializedObject* const Value, rapidjson::Document::AllocatorType& Allocator)
    {
        rapidjson::Value object(rapidjson::kStringType);
        if (Value == nullptr)
        {
            object.SetNull();
            return object;
        }
        const GUID guid = Value->GetID();
        char guidStr[GUID_STRING_SIZE];
        GuidToStr(guid, guidStr);
        object.SetString(guidStr, sizeof(guidStr), Allocator);
        return object;
    }

    rapidjson::Value Serialize(const std::vector<SerializedObject*>& Value,
                               rapidjson::Document::AllocatorType& Allocator)
    {
        rapidjson::Value array(rapidjson::kArrayType);
        array.Reserve(static_cast<rapidjson::SizeType>(Value.size()), Allocator);
        for (const SerializedObject* const object : Value)
        {
            array.PushBack(Serialize(object, Allocator), Allocator);
        }
        return array;
    }

    rapidjson::Value Serialize(const Shaders::Shader Value, rapidjson::Document::AllocatorType& Allocator)
    {
        rapidjson::Value object(rapidjson::kObjectType);
        const Shaders::ShaderSourceFiles source = Shaders::ShaderManager::GetShaderSourceFiles(Value);
        object.AddMember("vertex", Serialize(source.VertexShader, Allocator), Allocator);
        object.AddMember("geometry", Serialize(source.GeometryShader, Allocator), Allocator);
        object.AddMember("fragment", Serialize(source.FragmentShader, Allocator), Allocator);
        return object;
    }

    rapidjson::Value Serialize(const Shaders::ComputeShader Value, rapidjson::Document::AllocatorType& Allocator)
    {
        rapidjson::Value object(rapidjson::kStringType);
        const std::string path = Shaders::ShaderManager::GetShaderSourceFile(Value);
        object.SetString(path.c_str(), path.length(), Allocator);
        return object;
    }

    rapidjson::Value Serialize(const Materials::Material* const Value, rapidjson::Document::AllocatorType& Allocator)
    {
        rapidjson::Value object(rapidjson::kStringType);
        const std::string path = Materials::MaterialManager::GetMaterialPath(Value);
        object.SetString(path.c_str(), path.length(), Allocator);
        return object;
    }

    rapidjson::Value Serialize(const Materials::TextureMaterialProperty& Value,
                               rapidjson::Document::AllocatorType& Allocator)
    {
        const Engine::Texture value = Value.GetValue();
        return Serialize(value, Allocator);
    }

    rapidjson::Value Serialize(const Models::AABBox3 Value, rapidjson::Document::AllocatorType& Allocator)
    {
        rapidjson::Value object(rapidjson::kObjectType);
        object.AddMember("min", Serialize(Value.min, Allocator), Allocator);
        object.AddMember("max", Serialize(Value.max, Allocator), Allocator);
        return object;
    }

    void Deserialize(const rapidjson::Value& Object, const char* Name, int& Value)
    {
        const auto iterator = Object.FindMember(Name);
        if (iterator == Object.MemberEnd() || !iterator->value.IsInt())
        {
            return;
        }
        Value = iterator->value.GetInt();
    }


    void Deserialize(const rapidjson::Value& Object, const char* const Name, float& Value)
    {
        const auto iterator = Object.FindMember(Name);
        if (iterator == Object.MemberEnd() || !iterator->value.IsFloat())
        {
            return;
        }
        Value = iterator->value.GetFloat();
    }

    void Deserialize(const rapidjson::Value& Object, const char* const Name, bool& Value)
    {
        const auto iterator = Object.FindMember(Name);
        if (iterator == Object.MemberEnd() || !iterator->value.IsBool())
        {
            return;
        }
        Value = iterator->value.GetBool();
    }

    void Deserialize(const rapidjson::Value& Object, const char* Name, glm::quat& Value)
    {
        const auto iterator = Object.FindMember(Name);
        if (iterator == Object.MemberEnd() || !iterator->value.IsObject())
        {
            return;
        }
        const rapidjson::Value& vector = iterator->value;
        const auto iteratorX = vector.FindMember("x");


        if (iteratorX == vector.MemberEnd() || !iteratorX->value.IsFloat())
        {
            return;
        }
        const auto iteratorY = vector.FindMember("y");
        if (iteratorY == vector.MemberEnd() || !iteratorY->value.IsFloat())
        {
            return;
        }
        const auto iteratorZ = vector.FindMember("z");
        if (iteratorZ == vector.MemberEnd() || !iteratorZ->value.IsFloat())
        {
            return;
        }

        const auto iteratorW = vector.FindMember("w");
        if (iteratorW == vector.MemberEnd() || !iteratorW->value.IsFloat())
        {
            return;
        }
        Value.x = iteratorX->value.GetFloat();
        Value.y = iteratorY->value.GetFloat();
        Value.z = iteratorZ->value.GetFloat();
        Value.w = iteratorW->value.GetFloat();
    }

    void Deserialize(const rapidjson::Value& Object, const char* Name, glm::vec4& Value)
    {
        const auto iterator = Object.FindMember(Name);
        if (iterator == Object.MemberEnd() || !iterator->value.IsObject())
        {
            return;
        }
        const rapidjson::Value& vector = iterator->value;
        const auto iteratorX = vector.FindMember("x");


        if (iteratorX == vector.MemberEnd() || !iteratorX->value.IsFloat())
        {
            return;
        }
        const auto iteratorY = vector.FindMember("y");
        if (iteratorY == vector.MemberEnd() || !iteratorY->value.IsFloat())
        {
            return;
        }
        const auto iteratorZ = vector.FindMember("z");
        if (iteratorZ == vector.MemberEnd() || !iteratorZ->value.IsFloat())
        {
            return;
        }

        const auto iteratorW = vector.FindMember("w");
        if (iteratorW == vector.MemberEnd() || !iteratorW->value.IsFloat())
        {
            return;
        }
        Value.x = iteratorX->value.GetFloat();
        Value.y = iteratorY->value.GetFloat();
        Value.z = iteratorZ->value.GetFloat();
        Value.w = iteratorW->value.GetFloat();
    }

    void Deserialize(const rapidjson::Value& Object, const char* const Name, glm::vec3& Value)
    {
        const auto iterator = Object.FindMember(Name);
        if (iterator == Object.MemberEnd() || !iterator->value.IsObject())
        {
            return;
        }
        const rapidjson::Value& vector = iterator->value;
        const auto iteratorX = vector.FindMember("x");


        if (iteratorX == vector.MemberEnd() || !iteratorX->value.IsFloat())
        {
            return;
        }
        const auto iteratorY = vector.FindMember("y");
        if (iteratorY == vector.MemberEnd() || !iteratorY->value.IsFloat())
        {
            return;
        }
        const auto iteratorZ = vector.FindMember("z");
        if (iteratorZ == vector.MemberEnd() || !iteratorZ->value.IsFloat())
        {
            return;
        }
        Value.x = iteratorX->value.GetFloat();
        Value.y = iteratorY->value.GetFloat();
        Value.z = iteratorZ->value.GetFloat();
    }

    void Deserialize(const rapidjson::Value& Object, const char* const Name, glm::vec2& Value)
    {
        const auto iterator = Object.FindMember(Name);
        if (iterator == Object.MemberEnd() || !iterator->value.IsObject())
        {
            return;
        }
        const rapidjson::Value& vector = iterator->value;
        const auto iteratorX = vector.FindMember("x");


        if (iteratorX == vector.MemberEnd() || !iteratorX->value.IsFloat())
        {
            return;
        }
        const auto iteratorY = vector.FindMember("y");
        if (iteratorY == vector.MemberEnd() || !iteratorY->value.IsFloat())
        {
            return;
        }
        Value.x = iteratorX->value.GetFloat();
        Value.y = iteratorY->value.GetFloat();
    }

    void Deserialize(const rapidjson::Value& Object, const char* const Name, Engine::Texture& Value)
    {
        const auto iterator = Object.FindMember(Name);
        if (iterator == Object.MemberEnd() || !iterator->value.IsString())
        {
            return;
        }
        Value = Engine::TextureManager::GetTexture(iterator->value.GetString());
    }

    void Deserialize(const rapidjson::Value& Object, const char* const Name, Models::Model*& Value)
    {
        const auto iterator = Object.FindMember(Name);
        if (iterator == Object.MemberEnd() || !iterator->value.IsString())
        {
            return;
        }
        Value = Models::ModelManager::GetModel(iterator->value.GetString());
    }

    void Deserialize(const rapidjson::Value& Object, const char* Name, Models::ModelAnimated*& Value)
    {
        const auto iterator = Object.FindMember(Name);
        if (iterator == Object.MemberEnd() || !iterator->value.IsString())
        {
            return;
        }
        Value = Models::ModelManager::GetAnimatedModel(iterator->value.GetString());
    }

    void Deserialize(const rapidjson::Value& Object, const char* Name, Models::Animation*& Value)
    {
        const auto iterator = Object.FindMember(Name);
        if (iterator == Object.MemberEnd() || !iterator->value.IsString())
        {
            return;
        }
        Value = Models::ModelManager::GetAnimation(iterator->value.GetString());
    }

    void Deserialize(const rapidjson::Value& Object, const char* const Name, std::string& Value)
    {
        const auto iterator = Object.FindMember(Name);
        if (iterator == Object.MemberEnd() || !iterator->value.IsString())
        {
            return;
        }
        Value = iterator->value.GetString();
    }

    void Deserialize(const rapidjson::Value& Object, const char* const Name, GUID& Value)
    {
        const auto iterator = Object.FindMember(Name);
        if (iterator == Object.MemberEnd() || !iterator->value.IsString())
        {
            return;
        }
        const std::string str = iterator->value.GetString();
        StrToGuid(str, &Value);
    }

    void Deserialize(const rapidjson::Value& Object, const char* const Name, SerializedObject*& Value,
                     std::unordered_map<GUID, SerializedObject*, GuidHasher>& ReferenceMap)
    {
        const auto guidIterator = Object.FindMember(Name);
        if (guidIterator == Object.MemberEnd() || !guidIterator->value.IsString())
        {
            Value = nullptr;
            return;
        }
        const std::string str = guidIterator->value.GetString();
        GUID guid;
        StrToGuid(str, &guid);
        const auto referenceIterator = ReferenceMap.find(guid);
        if (referenceIterator == ReferenceMap.end())
        {
            Value = nullptr;
            return;
        }
        Value = referenceIterator->second;
    }

    void Deserialize(const rapidjson::Value& Object, SerializedObject*& Value,
                     std::unordered_map<GUID, SerializedObject*, GuidHasher>& ReferenceMap)
    {
        if (!Object.IsString())
        {
            Value = nullptr;
            return;
        }
        GUID guid;
        StrToGuid(Object.GetString(), &guid);
        const auto referenceIterator = ReferenceMap.find(guid);
        if (referenceIterator == ReferenceMap.end())
        {
            Value = nullptr;
            return;
        }
        Value = referenceIterator->second;
    }

    void Deserialize(const rapidjson::Value& Object, const char* Name, Shaders::Shader& Value)
    {
        const auto iterator = Object.FindMember(Name);
        if (iterator == Object.MemberEnd() || !iterator->value.IsObject())
        {
            return;
        }
        const rapidjson::Value& files = iterator->value;
        const auto iteratorVertex = files.FindMember("vertex");

        if (iteratorVertex == files.MemberEnd() || !iteratorVertex->value.IsString())
        {
            return;
        }
        const auto iteratorGeometry = files.FindMember("geometry");
        if (iteratorGeometry == files.MemberEnd() || !iteratorGeometry->value.IsString())
        {
            return;
        }
        const auto iteratorFragment = files.FindMember("fragment");
        if (iteratorFragment == files.MemberEnd() || !iteratorFragment->value.IsString())
        {
            return;
        }

        const std::string vertexPath = iteratorVertex->value.GetString();
        const std::string geometryPath = iteratorGeometry->value.GetString();
        const std::string fragmentPath = iteratorFragment->value.GetString();

        const Shaders::ShaderSourceFiles sourceFiles(vertexPath, geometryPath, fragmentPath);
        Value = Shaders::ShaderManager::GetShader(sourceFiles);
    }

    void Deserialize(const rapidjson::Value& Object, const char* Name, Shaders::ComputeShader& Value)
    {
        const auto iterator = Object.FindMember(Name);
        if (iterator == Object.MemberEnd() || !iterator->value.IsString())
        {
            return;
        }

        Value = Shaders::ShaderManager::GetComputeShader(iterator->value.GetString());
    }

    void Deserialize(const rapidjson::Value& Object, const char* Name, Materials::Material*& Value)
    {
        const auto iterator = Object.FindMember(Name);
        if (iterator == Object.MemberEnd() || !iterator->value.IsString())
        {
            return;
        }

        Value = Materials::MaterialManager::GetMaterial(iterator->value.GetString());
    }

    void Deserialize(const rapidjson::Value& Object, const char* const Name, Materials::TextureMaterialProperty& Value)
    {
        Engine::Texture value = Value.GetValue();
        Deserialize(Object, Name, value);
        Value.SetValue(value);
    }

    void Deserialize(const rapidjson::Value& Object, const char* Name, Models::AABBox3& Value)
    {
        const auto iterator = Object.FindMember(Name);
        if (iterator == Object.MemberEnd() || !iterator->value.IsObject())
        {
            return;
        }
        const rapidjson::Value& aabb = iterator->value;
        const auto iteratorMin = aabb.FindMember("min");


        if (iteratorMin == aabb.MemberEnd() || !iteratorMin->value.IsObject())
        {
            return;
        }
        const auto iteratorMax = aabb.FindMember("max");
        if (iteratorMax == aabb.MemberEnd() || !iteratorMax->value.IsObject())
        {
            return;
        }

        Deserialize(aabb, "min", Value.min);
        Deserialize(aabb, "max", Value.max);
    }

} // Serialization
