#include "AudioPlayer.h"

AudioPlayer::AudioPlayer() : playWav(), audioOutput(), patchCord(playWav, audioOutput) {
  #define SDCARD_CS_PIN BUILTIN_SDCARD
  #define SDCARD_MOSI_PIN 11
  #define SDCARD_SCK_PIN 13

  SPI.setMOSI(SDCARD_MOSI_PIN);
  SPI.setSCK(SDCARD_SCK_PIN);
  if (!(SD.begin(SDCARD_CS_PIN))) {
    Serial.println("SD Card Initialization failed");
  }
}

void AudioPlayer::playFile(const char* filename) {
  playWav.play(filename);
  delay(20);

  Serial.print("Playing file: ");
  Serial.println(filename);

  while (playWav.isPlaying()) {
    delay(100);
  }
}