#pragma once
#include <cstdint>
#include <vector>

#include "BloodEraser.h"
#include "BloodSource.h"
#include "BloodStain.h"
#include "Models/AabBox.h"
#include "Shaders/ComputeShader.h"
#include "Shaders/Shader.h"

namespace Engine
{

    class BloodManager
    {
    private:
        static constexpr int32_t MaskSize = 4096;
        static constexpr int LocalSize = 16;
        static constexpr int GroupsOneDimension = (MaskSize + LocalSize - 1) / LocalSize;
        static constexpr int TotalGroups = GroupsOneDimension * GroupsOneDimension;
        static_assert(MaskSize % 16 == 0, "MaskSize must be a multiple of 16");

        static BloodManager* Instance;

        uint32_t FrameBuffer = 0;
        uint32_t ColorBuffer = 0;
        glm::mat4 ViewProjectionMatrix;
        Shaders::Shader MaskShader;

        BloodEraser* Eraser = nullptr;
        std::vector<const BloodSource*> BloodSources;
        std::vector<const BloodStain*> BloodStains;

        int32_t ViewProjectionMatrixUniformLocation = 0;
        int32_t ModelMatrixUniformLocation = 0;
        int32_t ColorUniformLocation = 0;

        uint32_t AccumulationBuffer = 0;
        Shaders::ComputeShader AccumulationShader;
        glm::ivec3 DispatchSize;

        float BloodFill;

        float* AccumulationPtr = nullptr;

    public:
        explicit BloodManager(const Models::AABBox3& SceneBounds);

    public:
        ~BloodManager();

        [[nodiscard]] uint32_t GetMaskId() const
        {
            return ColorBuffer;
        }

        [[nodiscard]] glm::mat4 GetMaskSpaceTransform() const
        {
            return ViewProjectionMatrix;
        }

        [[nodiscard]] static BloodManager* GetCurrent()
        {
            return Instance;
        }

        [[nodiscard]] int32_t GetViewProjectionMatrixUniformLocation() const
        {
            return ViewProjectionMatrixUniformLocation;
        }

        [[nodiscard]] int32_t GetModelMatrixUniformLocation() const
        {
            return ModelMatrixUniformLocation;
        }

        [[nodiscard]] int32_t GetColorUniformLocation() const
        {
            return ColorUniformLocation;
        }

        void AddBloodSource(const BloodSource* const BloodSource)
        {
            BloodSources.push_back(BloodSource);
        }

        void RemoveBloodSource(const BloodSource* const BloodSource)
        {
            std::erase(BloodSources, BloodSource);
        }

        void AddBloodStain(const BloodStain* const BloodStain)
        {
            BloodStains.push_back(BloodStain);
        }

        /**
         * @brief Returns relative amount of blood in the scene in [0,1] range.
         */
        [[nodiscard]] float GetBloodFill() const
        {
            return BloodFill;
        }

        void AddBloodEraser(BloodEraser* const BloodEraser)
        {
            CHECK_MESSAGE(Eraser == nullptr, "Blood eraser already added");
            Eraser = BloodEraser;
        }

        void RemoveBloodEraser(const BloodEraser* const BloodEraser)
        {
            if (Eraser == BloodEraser)
            {
                Eraser = nullptr;
            }
        }

    public:
        void Update(float DeltaTime);
    };

}
