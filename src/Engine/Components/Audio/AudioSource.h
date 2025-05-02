#pragma once

#include "Engine/Components/Component.h"
#include "audio/AudioManager.h"
#include <glm/glm.hpp>

namespace Engine
{
    /**
     * @brief Component for managing sound playback on an entity.
     *
     * AudioSource provides per-entity sound control. It exposes an ImGui-based UI
     * to adjust audio settings in the editor. Each instance of AudioSource is
     * associated with a specific entity and works with shared sound data managed
     * by AudioManager.
     */
    class AudioSource final : public Component
    {
    private:
        AudioManager& AudioManager; ///< Reference to the global AudioManager instance.
        ma_sound* SelectedSound; ///< Currently selected sound to control for this source.

        float SoundVolume = 1.0f; ///< Volume level of the selected sound (0.0 to 1.0).
        bool Looping = false; ///< Whether the selected sound should loop.
        glm::vec3 Position = glm::vec3(0.0f); ///< 3D position of the sound in world space.
        float MinDist = 0.0f; ///< Minimum distance for full-volume playback.
        float MaxDist = 10.0f; ///< Maximum distance at which sound becomes inaudible.
        float RollOff = 1.0f; ///< Roll-off factor for sound attenuation.

        SERIALIZATION_EXPORT_CLASS(AudioSource);

    public:
        /**
         * @brief Constructs the AudioSource component.
         *
         * Acquires a reference to the global AudioManager instance.
         */
        AudioSource();

        /**
         * @brief Renders the ImGui interface for editing this audio source.
         */
        void RenderAudioSourceImGui();

        /**
         * @brief Resets settings of sounds in the ImGui interface to default.
         */
        void ResetAudioSettings();
    };
}
