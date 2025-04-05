#pragma once

#include <memory>
#include <miniaudio.h>
#include <string>
#include <unordered_map>

namespace Engine
{
    /**
     * @brief Class for managing audio using MiniAudio.
     *
     * This class implements the Singleton pattern for handling audio in the application.
     */
    class AudioManager
    {
    private:
        static AudioManager* Instance; ///< Singleton instance.

        ma_engine Engine; ///< MiniAudio engine for sound playback.
        std::unordered_map<std::string, std::shared_ptr<ma_sound>> Sounds; ///< Map of sounds indexed by string ID.

        /**
         * @brief Private constructor.
         * Initializes the MiniAudio engine.
         */
        AudioManager();

    public:
        /**
         * @brief Returns the Singleton instance.
         *
         * @return AudioManager& The instance of AudioManager.
         */
        static AudioManager& GetInstance();

        /**
         * @brief Destroys the Singleton instance.
         *
         * Deletes the instance and frees any resources.
         */
        static void DestroyInstance();

        /**
         * @brief Retrieves the map of loaded sounds.
         *
         * @return Sounds
         */
        std::unordered_map<std::string, std::shared_ptr<ma_sound>> GetSounds();

        /**
         * @brief Loads an audio file with a specific ID.
         *
         * @param Filename The path to the audio file.
         * @param Id The unique string ID for the sound.
         * @return true If the file was successfully loaded, false otherwise.
         */
        bool LoadSound(const std::string& Filename, const std::string& Id);

        /**
         * @brief Plays or resumes a sound based on its ID.
         *
         * @param Id The string ID of the sound to play.
         */
        void PlayAudio(const std::string& Id);

        /**
         * @brief Sets the global volume.
         *
         * @param Volume The volume level (0.0 to 1.0).
         */
        void SetVolumeGlobal(float Volume);

        /**
         * @brief Sets the volume for a specific sound.
         *
         * @param Id The string ID of the sound.
         * @param Volume The volume level (0.0 to 1.0).
         */
        void SetVolume(const std::string& Id, float Volume);

        /**
         * @brief Pauses a sound based on its ID.
         *
         * @param Id The string ID of the sound to pause.
         */
        void PauseSound(const std::string& Id);

        /**
         * @brief Stops a sound based on its ID.
         *
         * @param Id The string ID of the sound to stop.
         */
        void StopSound(const std::string& Id);

        /**
         * @brief Sets whether a sound should loop.
         *
         * @param Id The string ID of the sound.
         * @param Loop True to enable looping, false to disable.
         */
        void SetLooping(const std::string& Id, bool Loop);

        /**
         * @brief Sets the listener's position in 3D space.
         *
         * @param X X coordinate.
         * @param Y Y coordinate.
         * @param Z Z coordinate.
         */
        void SetListenerPosition(float X, float Y, float Z);

        /**
         * @brief Sets the listener's orientation in 3D space.
         *
         * @param ForwardX X component of the forward vector.
         * @param ForwardY Y component of the forward vector.
         * @param ForwardZ Z component of the forward vector.
         * @param UpX X component of the up vector.
         * @param UpY Y component of the up vector.
         * @param UpZ Z component of the up vector.
         */
        void SetListenerOrientation(float ForwardX, float ForwardY, float ForwardZ, float UpX, float UpY, float UpZ);

        /**
         * @brief Sets the position of a specific sound in 3D space.
         *
         * @param Id The string ID of the sound.
         * @param X X coordinate.
         * @param Y Y coordinate.
         * @param Z Z coordinate.
         */
        void SetSoundPosition(const std::string& Id, float X, float Y, float Z);

        /**
         * @brief Configures the attenuation of a 3D sound.
         *
         * @param Id The identifier of the sound.
         * @param MinDist The minimum distance at which the sound is at full volume.
         * @param MaxDist The maximum distance beyond which the sound becomes inaudible.
         */
        void ConfigureSoundAttenuation(const std::string& Id, float MinDist, float MaxDist);

    };
}
