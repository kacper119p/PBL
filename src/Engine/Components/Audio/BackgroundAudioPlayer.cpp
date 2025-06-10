#include "BackgroundAudioPlayer.h"

namespace Engine
{
    BackgroundAudioPlayer::BackgroundAudioPlayer() :
        AudioManagerRef(AudioManager::GetInstance())
    {
    }

    BackgroundAudioPlayer::~BackgroundAudioPlayer()
    {
        Stop();
        SoundInstance.reset();
    }

    void BackgroundAudioPlayer::PlayLooping(const std::string& SoundId, float Volume)
    {
        if (SoundInstance)
        {
            AudioManagerRef.StopSound(SoundInstance);
            SoundInstance.reset();
        }

        SoundInstance = AudioManagerRef.CreateSoundInstance(SoundId);
        if (SoundInstance)
        {
            ma_sound_set_spatialization_enabled(SoundInstance.get(), false);
            ma_sound_set_looping(SoundInstance.get(), true);
            ma_sound_set_volume(SoundInstance.get(), Volume);
            ma_sound_start(SoundInstance.get());
        }
    }

    void BackgroundAudioPlayer::Stop()
    {
        if (SoundInstance)
        {
            AudioManagerRef.StopSound(SoundInstance);
            SoundInstance.reset();
        }
    }

    void BackgroundAudioPlayer::Pause()
    {
        if (SoundInstance)
        {
            AudioManagerRef.PauseSound(SoundInstance);
        }
    }

    void BackgroundAudioPlayer::Resume()
    {
        if (SoundInstance)
        {
            AudioManagerRef.PlayAudio(SoundInstance);
        }
    }

    bool BackgroundAudioPlayer::IsPlaying() const
    {
        return SoundInstance && ma_sound_is_playing(SoundInstance.get());
    }
}
