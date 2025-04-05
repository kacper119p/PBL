#pragma once

#include "Engine/Components/Component.h"
#include "audio/AudioManager.h"

namespace Engine
{
    /**
     * @brief Class for handling audio-related user interface elements.
     *
     * This class is responsible for managing the UI elements related to the audio system.
     * It interacts with the AudioManager to load and manipulate sounds and their properties.
     */
    class AudioUi final : public Component
    {
    private:
        AudioManager& AudioManager; ///< Reference to the AudioManager instance.

        float GlobalSoundVolume = 1.0f; ///< Global volume level affecting all sounds.

        /**
        * @brief Struct for storing the position of a sound in 3D space.
        *
        * This struct stores the X, Y, and Z coordinates for a specific sound's position in 3D space.
        */
        struct Position
        {
            float X = 0.0f; ///< X position of the selected sound.
            float Y = 0.0f; ///< Y position of the selected sound.
            float Z = 0.0f; ///< Z position of the selected sound.
        };

        /**
        * @brief Struct for storing settings of a specific sound.
        *
        * This struct contains the ID, volume, looping flag, and position of a specific sound.
        */
        struct SoundSettings
        {
            std::string SoundId; ///< Unique identifier for the sound.
            float SoundVolume = 1.0f; ///< Volume level of the selected sound.
            bool Looping = false; ///< Flag indicating whether the selected sound should loop.
            Position Position; ///< Position of the selected sound in 3D space.
        };

        std::unordered_map<std::string, SoundSettings> SoundConfigs;
        ///< Map storing the settings for each sound by its ID.
        std::string SelectedSoundId; ///< ID of the currently selected sound for manipulation in the UI.

        SERIALIZATION_EXPORT_CLASS(AudioUi);

    public:
        /**
         * @brief Constructor.
         *
         * Initializes the AudioUI instance by obtaining the reference to the AudioManager.
         */
        AudioUi();

        /**
         * @brief Loads all available sounds from the sound directory.
         *
         * This method iterates through the sound directory, loading all audio files
         * into the AudioManager with the corresponding identifiers.
         */
        void LoadSounds();

        /**
         * @brief Renders the user interface for controlling audio settings.
         *
         * This method creates the ImGui interface that allows users to interact with
         * various audio settings.
         */
        void RenderUi();
    };
}
