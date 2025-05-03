#pragma once

#include <miniaudio.h>
#include <filesystem>
#include <glm/glm.hpp>
#include <map>

namespace Engine
{
    /**
     * @brief Manages all audio operations using MiniAudio.
     *
     * AudioManager is a Singleton class responsible for managing all sound-related functionality.
     */
    class AudioManager
    {
    private:
        static AudioManager* Instance; ///< Singleton instance of the AudioManager.

        ma_engine Engine; ///< MiniAudio engine instance used for handling audio playback.

        std::map<std::string, std::shared_ptr<ma_sound>> Sounds;
        ///< Container holding all loaded sounds, mapped by filename.

        /**
         * @brief Constructs the AudioManager and initializes the MiniAudio engine.
         *
         */
        AudioManager();

        /**
         * @brief Destructor.
         *
         * Cleans up the MiniAudio engine and deallocates any loaded sounds.
         */
        ~AudioManager();

    public:
        /**
         * @brief Retrieves the singleton instance of the AudioManager.
         *
         * @return Reference to the AudioManager instance.
         */
        static AudioManager& GetInstance();

        /**
         * @brief Creates a new sound instance from a loaded sound based on the given SoundId.
         *
         * @param SoundId The ID of the sound to create an instance of.
         * @return A shared pointer to the new sound instance, or nullptr if an error occurs.
         */
        std::shared_ptr<ma_sound> CreateSoundInstance(const std::string& SoundId);

        /**
         * @brief Destroys the singleton instance of the AudioManager.
         *
         * Frees all associated resources and deinitializes the audio engine.
         */
        static void DestroyInstance();

        /**
         * @brief Loads a single sound file into memory.
         *
         * @param Filename Path to the audio file to be loaded.
         * @param Sound Output reference to the created sound object.
         * @return true if the sound was successfully loaded, false otherwise.
         */
        bool LoadSound(const std::string& Filename, ma_sound& Sound);

        /**
         * @brief Returns the collection of all currently loaded sounds.
         *
         * @return Vector of shared pointers to ma_sound objects.
         */
        std::map<std::string, std::shared_ptr<ma_sound>> GetLoadedSounds() const;

        /**
         * @brief Loads predefined or batch sounds into the manager.
         *
         * This function can be used to initialize a group of audio assets at once.
         */
        void LoadSounds();

        /**
         * @brief Plays or resumes playback of the specified sound.
         *
         * @param Sound The sound object to play.
         */
        void PlayAudio(std::shared_ptr<ma_sound> Sound);

        /**
         * @brief Pauses playback of the specified sound.
         *
         * @param Sound The sound object to pause.
         */
        void PauseSound(std::shared_ptr<ma_sound> Sound);

        /**
         * @brief Stops playback of the specified sound.
         *
         * @param Sound The sound object to stop.
         */
        void StopSound(std::shared_ptr<ma_sound> Sound);

        /**
         * @brief Sets whether a sound should loop when it reaches the end.
         *
         * @param Sound The sound to configure.
         * @param Looping True to enable looping; false to disable.
         */
        void SetLooping(std::shared_ptr<ma_sound> Sound, bool Looping);

        /**
         * @brief Sets the volume level of the specified sound.
         *
         * @param Sound The sound to modify.
         * @param Volume The volume value in range [0.0, 1.0].
         */
        void SetVolume(std::shared_ptr<ma_sound> Sound, float Volume);

        /**
         * @brief Retrieves the global output volume.
         *
         * @return Global volume in range [0.0, 1.0].
         */
        float GetGlobalVolume();

        /**
         * @brief Sets the global output volume.
         *
         * @param Volume Volume level in range [0.0, 1.0].
         */
        void SetGlobalVolume(float Volume);

        /**
         * @brief Sets the listener's position in 3D space.
         *
         * @param Position 3D coordinates representing the listener's position.
         */
        void SetListenerPosition(const glm::vec3& Position);

        /**
         * @brief Sets the listener's orientation based on forward and up vectors.
         *
         * @param Forward Vector representing the listener's facing direction.
         * @param Up Vector representing the listener's upward direction.
         */
        void SetListenerOrientation(glm::vec3 Forward, glm::vec3 Up);

        /**
         * @brief Sets the position of a sound in 3D space.
         *
         * @param Sound The sound to position.
         * @param Position World-space position of the sound.
         */
        void SetSoundPosition(std::shared_ptr<ma_sound> Sound, const glm::vec3& Position);

        /**
         * @brief Configures distance attenuation parameters for 3D audio.
         *
         * @param Sound The sound to apply attenuation to.
         * @param MinDist Minimum distance for full volume.
         * @param MaxDist Maximum distance beyond which the sound is inaudible.
         * @param RollOff Attenuation roll-off factor.
         */
        void ConfigureSoundAttenuation(std::shared_ptr<ma_sound> Sound, float MinDist, float MaxDist, float RollOff);

        /**
         * @brief Renders a separate ImGui window to control the global volume.
         */
        static void RenderGlobalVolumeImGui();
    };
}
