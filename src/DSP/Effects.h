#ifndef EFFECTS_H
#define EFFECTS_H

#include <cmath>
#include <vector>
#include <cstring>

class Chorus {
public:
    static constexpr int MAX_BUFFER_SIZE = 8192; // Enough for 96kHz+ with headroom

    Chorus() : rate_(1.0f), depth_(0.3f), sampleRate_(48000.0f),
               writePos_(0), lfoPhase_(0.0f), output_(0.0f) {
        buffer_.resize(MAX_BUFFER_SIZE, 0.0f);
    }

    void setRate(float rate) { rate_ = clampf(rate, 0.1f, 10.0f); }
    void setDepth(float depth) { depth_ = clampf(depth, 0.0f, 1.0f); }
    void setSampleRate(float sr) { sampleRate_ = sr; }

    void process(float input) {
        float lfo = std::sin(lfoPhase_ * 6.28318530718f);
        lfoPhase_ += rate_ / sampleRate_;
        if (lfoPhase_ >= 1.0f) lfoPhase_ -= 1.0f;

        // Delay time: 15-35ms range modulated by LFO
        float delayTime = 0.02f + depth_ * 0.01f * lfo;
        float delaySamplesF = delayTime * sampleRate_;
        int delaySamples = static_cast<int>(delaySamplesF);
        float frac = delaySamplesF - static_cast<float>(delaySamples);

        // Clamp to buffer bounds
        if (delaySamples >= MAX_BUFFER_SIZE - 1) delaySamples = MAX_BUFFER_SIZE - 2;
        if (delaySamples < 1) delaySamples = 1;

        // Linear interpolation for smooth modulation
        int readPos0 = writePos_ - delaySamples;
        if (readPos0 < 0) readPos0 += MAX_BUFFER_SIZE;
        int readPos1 = readPos0 - 1;
        if (readPos1 < 0) readPos1 += MAX_BUFFER_SIZE;

        float delayed = buffer_[readPos0] * (1.0f - frac) + buffer_[readPos1] * frac;

        buffer_[writePos_] = input;
        writePos_ = (writePos_ + 1) % MAX_BUFFER_SIZE;

        output_ = input + delayed * 0.5f;
    }

    float getOutput() const { return output_; }

    void reset() {
        std::fill(buffer_.begin(), buffer_.end(), 0.0f);
        writePos_ = 0;
        lfoPhase_ = 0.0f;
        output_ = 0.0f;
    }

private:
    static inline float clampf(float v, float lo, float hi) {
        return (v < lo) ? lo : (hi < v) ? hi : v;
    }

    std::vector<float> buffer_;
    int writePos_;
    float lfoPhase_;
    float rate_;
    float depth_;
    float sampleRate_;
    float output_;
};

class FMDelay {
public:
    static constexpr int MAX_BUFFER_SIZE = 192000; // 2s at 96kHz

    FMDelay() : time_(0.25f), feedback_(0.3f), sampleRate_(48000.0f),
              writePos_(0), output_(0.0f) {
        buffer_.resize(MAX_BUFFER_SIZE, 0.0f);
    }

    void setTime(float time) { time_ = clampf(time, 0.001f, 2.0f); }
    void setFeedback(float fb) { feedback_ = clampf(fb, 0.0f, 0.9f); }
    void setSampleRate(float sr) { sampleRate_ = sr; }

    void process(float input) {
        int delaySamples = static_cast<int>(time_ * sampleRate_);
        if (delaySamples >= MAX_BUFFER_SIZE) delaySamples = MAX_BUFFER_SIZE - 1;
        if (delaySamples < 1) delaySamples = 1;

        int readPos = writePos_ - delaySamples;
        if (readPos < 0) readPos += MAX_BUFFER_SIZE;

        float delayed = buffer_[readPos];
        buffer_[writePos_] = input + delayed * feedback_;

        writePos_ = (writePos_ + 1) % MAX_BUFFER_SIZE;

        output_ = input + delayed;
    }

    float getOutput() const { return output_; }

    void reset() {
        std::fill(buffer_.begin(), buffer_.end(), 0.0f);
        writePos_ = 0;
        output_ = 0.0f;
    }

private:
    static inline float clampf(float v, float lo, float hi) {
        return (v < lo) ? lo : (hi < v) ? hi : v;
    }

    std::vector<float> buffer_;
    int writePos_;
    float time_;
    float feedback_;
    float sampleRate_;
    float output_;
};

#endif
