#ifndef AUDIO_MANAGER_H
#define AUDIO_MANAGER_H

#include <Audio.h>

class AudioManager {
public:
    AudioManager();
    void initialize();
    void playStartGameSound();
    void playRollerGameSound();
    void playBallHitSound();

private:
    AudioPlaySdWav playWav1;
    AudioPlaySdWav playWav2;
    AudioPlaySdWav playWav3;
    AudioOutputI2SOct audioOutput;
    AudioMixer4 mixer1;
    AudioMixer4 mixer2;
    AudioMixer4 mixer3;
    AudioConnection* patchCords[9];
};

#endif