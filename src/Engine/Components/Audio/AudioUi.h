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
    class AudioUi
    {
    private:
        AudioManager& AudioManager; ///< Reference to the AudioManager instance.

        float SoundVolume = 1.0f; ///< Volume level of the selected sound.
        float GlobalSoundVolume = 1.0f; ///< Global volume level affecting all sounds.
        bool Looping = false; ///< Flag indicating whether the selected sound should loop.
        float SoundPos[3] = {0.0f, 0.0f, 0.0f}; ///< Position of the selected sound in 3D space.


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
