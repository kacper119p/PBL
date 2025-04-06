#pragma once

#include "Audio/AudioManager.h"
#include "Engine/EngineObjects/Camera.h"

namespace Engine
{
    /**
     * @brief Represents the audio listener in the 3D world.
     *
     * This component synchronizes the audio listener's position and orientation
     * with a camera. It ensures spatial audio behaves correctly from the player's
     * point of view.
     */
    class AudioListener
    {
    public:
        /**
         * @brief Constructs the AudioListener using a reference to a Camera.
         * @param Camera The camera to synchronize the audio listener with.
         */
        AudioListener(Camera& Camera);

        /**
         * @brief Updates the audio listener's position and orientation.
         */
        void UpdateListener();

    private:
        Camera& Camera; ///< Reference to the camera driving the listener's transform.
        AudioManager& AudioManager; ///< Reference to the singleton AudioManager.
    };
}
