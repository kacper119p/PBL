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
     * @brief Plays a sound based on its ID.
     *
     * @param id The string ID of the sound to play.
     */
    void playSound(const std::string& id);

    /**
     * @brief Sets the global volume.
     *
     * @param volume The volume level (0.0 to 1.0).
     */
    void setVolume(float volume);

    /**
     * @brief Stops a sound based on its ID.
     *
     * @param id The string ID of the sound to stop.
     */
    void stopSound(const std::string& id);
};

#endif
