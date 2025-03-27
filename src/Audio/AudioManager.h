#ifndef TIDEENGINE_AUDIOMANAGER_H
#define TIDEENGINE_AUDIOMANAGER_H

#include <memory>
#include <miniaudio.h>
#include <string>
#include <unordered_map>

/**
 * @brief Class for managing audio using MiniAudio.
 *
 * This class implements the Singleton pattern for handling audio in the application.
 */
class AudioManager {
private:
    static AudioManager* instance; ///< Singleton instance.

    ma_engine engine; ///< MiniAudio engine for sound playback.
    std::unordered_map<std::string, std::shared_ptr<ma_sound>> sounds; ///< Map of sounds indexed by string ID.

    /**
     * @brief Private constructor.
     * Initializes the MiniAudio engine.
     */
    AudioManager();

    /**
     * @brief Private destructor.
     * Cleans up resources and shuts down MiniAudio.
     */
    ~AudioManager();

public:
    /**
     * @brief Returns the Singleton instance.
     *
     * @return AudioManager& The instance of AudioManager.
     */
    static AudioManager& getInstance();

    /**
     * @brief Destroys the Singleton instance.
     *
     * Deletes the instance and frees any resources.
     */
    static void destroyInstance();

    /**
     * @brief Loads an audio file with a specific ID.
     *
     * @param filename The path to the audio file.
     * @param id The unique string ID for the sound.
     * @return true If the file was successfully loaded, false otherwise.
     */
    bool loadSound(const std::string& filename, const std::string& id);

    /**
     * @brief Plays or resumes a sound based on its ID.
     *
     * @param id The string ID of the sound to play.
     */
    void playSound(const std::string& id);

    /**
     * @brief Sets the global volume.
     *
     * @param volume The volume level (0.0 to 1.0).
     */
    void setVolumeGlobal(float volume);

    /**
     * @brief Sets the volume for a specific sound.
     *
     * @param id The string ID of the sound.
     * @param volume The volume level (0.0 to 1.0).
     */
    void setVolume(const std::string& id, float volume);

    /**
     * @brief Pauses a sound based on its ID.
     *
     * @param id The string ID of the sound to pause.
     */
    void pauseSound(const std::string& id);

    /**
     * @brief Stops a sound based on its ID.
     *
     * @param id The string ID of the sound to stop.
     */
    void stopSound(const std::string& id);

    /**
     * @brief Sets whether a sound should loop.
     *
     * @param id The string ID of the sound.
     * @param loop True to enable looping, false to disable.
     */
    void setLooping(const std::string& id, bool loop);

    /**
     * @brief Sets the listener's position in 3D space.
     *
     * @param x X coordinate.
     * @param y Y coordinate.
     * @param z Z coordinate.
     */
    void setListenerPosition(float x, float y, float z);

    /**
     * @brief Sets the listener's orientation in 3D space.
     *
     * @param forwardX X component of the forward vector.
     * @param forwardY Y component of the forward vector.
     * @param forwardZ Z component of the forward vector.
     * @param upX X component of the up vector.
     * @param upY Y component of the up vector.
     * @param upZ Z component of the up vector.
     */
    void setListenerOrientation(float forwardX, float forwardY, float forwardZ, float upX, float upY, float upZ);

    /**
     * @brief Sets the position of a specific sound in 3D space.
     *
     * @param id The string ID of the sound.
     * @param x X coordinate.
     * @param y Y coordinate.
     * @param z Z coordinate.
     */
    void setSoundPosition(const std::string& id, float x, float y, float z);

    /**
     * @brief Configures the attenuation of a 3D sound.
     *
     * @param id The identifier of the sound.
     * @param minDist The minimum distance at which the sound is at full volume.
     * @param maxDist The maximum distance beyond which the sound becomes inaudible.
     */
    void configureSoundAttenuation(const std::string& id, float minDist, float maxDist);
};

#endif
