#include "AudioListener.h"

namespace Engine
{
    AudioListener::AudioListener(Engine::Camera& Camera) :
        Camera(Camera), AudioManager(AudioManager::GetInstance())
    {
    }

    void AudioListener::UpdateListener()
    {
        AudioManager.SetListenerPosition(Camera.GetPosition());
        AudioManager.SetListenerOrientation(Camera.GetForward(), Camera.GetUp());
    }

}
