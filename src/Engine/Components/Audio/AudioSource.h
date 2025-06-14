#pragma once

#include "Engine/Components/Component.h"
#include "audio/AudioManager.h"
#include <glm/glm.hpp>
#include "Engine/Components/Updateable.h"

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
    class AudioSource final : public Component, IUpdateable
    {
    private:
        AudioManager& AudioManager; ///< Reference to the global AudioManager instance.
        std::string SelectedSoundId; ///< ID of the currently selected sound.
        std::shared_ptr<ma_sound> SoundInstance; ///< Instance of the sound assigned to this entity.

        float SoundVolume = 1.0f; ///< Volume level of the selected sound (0.0 to 1.0).
        bool Looping = false; ///< Whether the selected sound should loop.
        float MinDist = 0.0f; ///< Minimum distance for full-volume playback.
        float MaxDist = 10.0f; ///< Maximum distance at which sound becomes inaudible.
        float RollOff = 1.0f; ///< Roll-off factor for sound attenuation.

        SERIALIZATION_EXPORT_CLASS(AudioSource);

    public:
        /**
         * @brief Constructs the AudioSource component.
         * Acquires a reference to the global AudioManager instance.
         */
        AudioSource();

        /**
         * @brief Destructor for cleaning up any resources used by the AudioSource component.
         */
        ~AudioSource();

        /**
         * @brief Creates SoundInstance on start.
         */
        void Start() override;

        void Update(float DeltaTime) override;

        /**
         * @brief SoundInstance getter.
         * @return SoundInstance.
         */
        std::shared_ptr<ma_sound> GetSoundInstance();

        /**
         * @brief Resets settings of sounds in the ImGui interface to default.
         */
        void ResetAudioSettings();

#if EDITOR
        void DrawImGui() override;
#endif
    };
}
