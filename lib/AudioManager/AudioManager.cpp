#include "AudioManager.h"

AudioManager::AudioManager() :
    playWav1(), playWav2(), playWav3(), audioOutput(),
    mixer1(), mixer2(), mixer3() {}

void AudioManager::initialize() {
    AudioMemory(12);
    patchCords[0] = new AudioConnection(playWav1, 0, mixer1, 0);
    patchCords[1] = new AudioConnection(playWav1, 1, mixer1, 1);
    patchCords[2] = new AudioConnection(playWav2, 0, mixer2, 0);
    patchCords[3] = new AudioConnection(playWav2, 1, mixer2, 1);
    patchCords[4] = new AudioConnection(playWav3, 0, mixer3, 0);
    patchCords[5] = new AudioConnection(playWav3, 1, mixer3, 1);
    patchCords[6] = new AudioConnection(mixer1, audioOutput);
    patchCords[7] = new AudioConnection(mixer2, audioOutput);
    patchCords[8] = new AudioConnection(mixer3, audioOutput);
}

void AudioManager::playStartGameSound() {
    playWav1.play("startGame.wav");
}

void AudioManager::playRollerGameSound() {
    playWav2.play("rollerGame.wav");
}

void AudioManager::playBallHitSound() {
    if (!playWav3.isPlaying()) {
        playWav3.play("ballHit.wav");
    }
}