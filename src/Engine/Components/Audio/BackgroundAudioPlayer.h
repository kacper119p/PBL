#pragma once

#include <memory>
#include <string>
#include "Audio/AudioManager.h"

namespace Engine
{
    class BackgroundAudioPlayer
    {
    public:
        BackgroundAudioPlayer();

        ~BackgroundAudioPlayer();

        void PlayLooping(const std::string& SoundId, float Volume = 1.0f);

        void Stop();

        void Pause();

        void Resume();

        bool IsPlaying() const;

    private:
        std::shared_ptr<ma_sound> SoundInstance;
        AudioManager& AudioManagerRef;
    };
}
