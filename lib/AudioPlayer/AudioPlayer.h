#ifndef AUDIOPLAYER_H
#define AUDIOPLAYER_H

#include <Audio.h>
#include <SD.h>

class AudioPlayer {
private:
  AudioPlaySdWav playWav;
  AudioOutputI2S audioOutput;
  AudioConnection patchCord;

public:
  AudioPlayer();
  void playFile(const char* filename);
};

#endif // AUDIOPLAYER_H