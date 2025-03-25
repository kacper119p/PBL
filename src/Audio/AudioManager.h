#ifndef TIDEENGINE_AUDIOMANAGER_H
#define TIDEENGINE_AUDIOMANAGER_H

#include <soloud.h>
#include <soloud_wav.h>
#include <string>

/**
 * @brief Class for managing audio.
 *
 * This class implements the Singleton pattern for managing audio in the application,
 * using the SoLoud library (MiniAudio backend, .wav files).
 */
class AudioManager {
private:
    static AudioManager* instance;  ///< Singleton instance.

    SoLoud::Soloud soloud;  ///< Object for managing audio.
    SoLoud::Wav currentSound;  ///< Object representing the loaded audio file.

    /**
     * @brief Private constructor.
     */
    AudioManager();

    /**
     * @brief Private destructor.
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
     * @brief Loads an audio file.
     *
     * @param filename The name of the audio file.
     * @return true If the file was successfully loaded, false otherwise.
     */
    bool loadSound(const std::string& filename);

    /**
     * @brief Plays the loaded audio.
     */
    void playSound();

    /**
     * @brief Sets the global volume.
     *
     * @param volume The volume level (0.0 to 1.0).
     */
    void setVolume(float volume);

    /**
     * @brief Pauses all sounds.
     */
    void pauseSound();

    /**
     * @brief Resumes all sounds.
     */
    void resumeSound();

    /**
     * @brief Stops all sounds.
     */
    void stopSound();
};

#endif
