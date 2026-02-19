// FMEngine.cpp - Minimal implementation for template methods
#include "FMEngine.h"
#include <cmath>

void FMEngine::setVoiceBend(int note, float bendCents) {
    for (int v = 0; v < NUM_VOICES; ++v) {
        if (voices_[v].active && voices_[v].note == note) {
            voices_[v].bendCents = bendCents;
            float bendMultiplier = std::pow(2.0f, bendCents / 1200.0f);
            for (int op = 0; op < NUM_OPERATORS; ++op) {
                voices_[v].operators[op].setFrequency(
                    voices_[v].frequency * bendMultiplier, sampleRate_);
            }
        }
    }
}

void FMEngine::setVoicePressure(int note, float pressure) {
    for (int v = 0; v < NUM_VOICES; ++v) {
        if (voices_[v].active && voices_[v].note == note) {
            voices_[v].pressure = pressure;
        }
    }
}

void FMEngine::setVoiceSlide(int note, float slideRate) {
    for (int v = 0; v < NUM_VOICES; ++v) {
        if (voices_[v].active && voices_[v].note == note) {
            voices_[v].slideRate = slideRate;
        }
    }
}
