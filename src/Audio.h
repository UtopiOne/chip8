#pragma once

#include <SDL3/SDL.h>

namespace Chip8 {

constexpr int FREQUENCY = 440;
constexpr int SAMPLE_RATE = 44100;

class AudioHandler {
public:
  AudioHandler();

  SDL_AudioStream* GetStream() const { return m_AudioStream; }

  void PauseStream() {
    SDL_PauseAudioStreamDevice(m_AudioStream);
    m_IsStreamPaused = true;
  }
  void UnpauseStream() {
    SDL_ResumeAudioStreamDevice(m_AudioStream);
    m_IsStreamPaused = false;
  }

  bool IsStreamPaused() const { return m_IsStreamPaused; }

  static void AudioCallback(void* user_data, SDL_AudioStream* audio_stream, int additional_amount,
                            int total_amount);

private:
  SDL_AudioStream* m_AudioStream;

  bool m_IsStreamPaused = false;
};

}  // namespace Chip8
