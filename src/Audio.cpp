#include "Audio.h"

#include "Logging.h"

namespace Chip8 {

static int s_CurrentSample = 0;

AudioHandler::AudioHandler() {
  SDL_AudioSpec spec;
  spec.freq = SAMPLE_RATE;
  spec.format = SDL_AUDIO_F32;
  spec.channels = 1;

  m_AudioStream = SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &spec,
                                            AudioHandler::AudioCallback, nullptr);

  if (!m_AudioStream) {
    LOG_ERROR("Failed to initialize audio stream: {}", SDL_GetError());
    exit(1);
  }
}

void AudioHandler::AudioCallback(void* user_data, SDL_AudioStream* audio_stream,
                                 int additional_amount, int total_amount) {
  additional_amount /= sizeof(float);
  while (additional_amount > 0) {
    float samples[128];
    const int total = SDL_min(additional_amount, SDL_arraysize(samples));

    for (int i = 0; i < total; ++i) {
      const int freq = 440;
      const float phase = s_CurrentSample * FREQUENCY / (float)SAMPLE_RATE;
      samples[i] = SDL_sinf(phase * 2 * SDL_PI_F);
      s_CurrentSample++;
    }

    s_CurrentSample %= SAMPLE_RATE;

    SDL_PutAudioStreamData(audio_stream, samples, total * sizeof(float));

    additional_amount -= total;
  }
}

}  // namespace Chip8
