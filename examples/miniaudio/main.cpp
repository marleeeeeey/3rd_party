#define MINIAUDIO_IMPLEMENTATION
#include <chrono>
#include <iostream>
#include <thread>

#include "miniaudio.h"

auto filepath1 = "C:\\Users\\marle\\OneDrive\\storage_public\\music\\Pop\\1969 - Elton John\\1969 - Empty Sky\\01. Empty Sky.mp3";
auto filepath2 = "C:\\Users\\marle\\OneDrive\\storage_public\\music\\Psychedelic Rock\\1967 - Jimi Hendrix\\1967 - Are You Experienced\\01. Purple Haze.flac";

int main() {
  ma_result result;
  ma_engine engine;
  ma_sound sound;

  // 1. Initialize engine
  if (ma_engine_init(nullptr, &engine) != MA_SUCCESS) return -1;

  // 2. Load sound (without playing immediately)
  // Use MA_SOUND_FLAG_DECODE for seeking support in compressed files (mp3)
  result = ma_sound_init_from_file(&engine, filepath2, MA_SOUND_FLAG_DECODE, nullptr, nullptr, &sound);
  if (result != MA_SUCCESS) return -1;

  // 3. Start playback
  ma_sound_start(&sound);

  // --- Examples of real-time control ---

  // CHANGE VOLUME (0.0f to 1.0f or more)
  // Let's set it to 50%
  ma_sound_set_volume(&sound, 0.5f);
  std::cout << "Volume set to 50%" << std::endl;

  std::this_thread::sleep_for(std::chrono::seconds(2));

  // SEEKING (Jump to 10th second)
  // Time is specified in PCM frames.
  // To seek by seconds: seconds * sample_rate
  ma_uint32 sampleRate;
  ma_sound_get_data_format(&sound, nullptr, nullptr, &sampleRate, nullptr, 0);

  ma_uint64 seekFrame = 10 * sampleRate;
  ma_sound_seek_to_pcm_frame(&sound, seekFrame);
  std::cout << "Jumped to 10.0s mark" << std::endl;

  std::this_thread::sleep_for(std::chrono::seconds(2));

  // GET CURRENT POSITION
  ma_uint64 currentFrame = ma_sound_get_time_in_pcm_frames(&sound);
  float currentSeconds = (float)currentFrame / sampleRate;
  std::cout << "Current position: " << currentSeconds << "s" << std::endl;

  // Play sounds. Wait user input
  std::cout << "Playing sounds... Press Enter to continue\n";
  std::cin.get();

  // ---

  // Mute first sound
  ma_sound_set_volume(&sound, 0.0f);

  // Play second track with no parameters and control it from outside
  ma_engine_play_sound(&engine, filepath1, nullptr);

  std::cout << "Playing sounds... Press Enter to continue\n";
  std::cin.get();

  // ---

  // Cleanup
  ma_sound_uninit(&sound);
  ma_engine_uninit(&engine);

  return 0;
}
