#pragma once

// Audio player for PCM5101 DAC via I2S
// Plays MP3/WAV/AAC/FLAC files from SD card through the onboard speaker

void audio_player_init(void);
void audio_player_play(const char* path);  // e.g. "/mario.mp3"
void audio_player_stop(void);
bool audio_player_is_playing(void);
void audio_player_loop(void);  // Must be called from main loop()
