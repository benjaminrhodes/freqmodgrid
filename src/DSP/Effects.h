#ifndef EFFECTS_H
#define EFFECTS_H

#include <cmath>

class Chorus {
public:
    Chorus() : rate_(1.0f), depth_(0.3f), sampleRate_(48000.0f) {
        for (int i = 0; i < 1024; ++i) {
            buffer_[i] = 0.0f;
        }
        writePos_ = 0;
        lfoPhase_ = 0.0f;
    }

    void setRate(float rate) { rate_ = clamp(rate, 0.1f, 10.0f); }
    void setDepth(float depth) { depth_ = clamp(depth, 0.0f, 1.0f); }
    void setSampleRate(float sr) { sampleRate_ = sr; }

    void process(float input) {
        float lfo = sin(lfoPhase_ * 6.28318530718f);
        lfoPhase_ += rate_ / sampleRate_;
        if (lfoPhase_ >= 1.0f) lfoPhase_ -= 1.0f;

        float delayTime = 0.02f + depth_ * 0.01f * lfo;
        int delaySamples = static_cast<int>(delayTime * sampleRate_);
        
        int readPos = writePos_ - delaySamples;
        if (readPos < 0) readPos += 1024;

        float delayed = buffer_[readPos];
        buffer_[writePos_] = input;
        
        writePos_++;
        if (writePos_ >= 1024) writePos_ = 0;

        output_ = input + delayed * 0.5f;
    }

    float getOutput() const { return output_; }

private:
    static inline float clamp(float v, float lo, float hi) {
        return (v < lo) ? lo : (hi < v) ? hi : v;
    }

    float buffer_[1024];
    int writePos_;
    float lfoPhase_;
    float rate_;
    float depth_;
    float sampleRate_;
    float output_;
};

class Delay {
public:
    Delay() : time_(0.25f), feedback_(0.3f), sampleRate_(48000.0f) {
        for (int i = 0; i < 96000; ++i) {
            buffer_[i] = 0.0f;
        }
        writePos_ = 0;
    }

    void setTime(float time) { time_ = clamp(time, 0.001f, 2.0f); }
    void setFeedback(float fb) { feedback_ = clamp(fb, 0.0f, 0.9f); }
    void setSampleRate(float sr) { sampleRate_ = sr; }

    void process(float input) {
        int delaySamples = static_cast<int>(time_ * sampleRate_);
        delaySamples = (delaySamples >= 96000) ? 95999 : delaySamples;
        
        int readPos = writePos_ - delaySamples;
        if (readPos < 0) readPos += 96000;

        float delayed = buffer_[readPos];
        buffer_[writePos_] = input + delayed * feedback_;
        
        writePos_++;
        if (writePos_ >= 96000) writePos_ = 0;

        output_ = input + delayed;
    }

    float getOutput() const { return output_; }
    void reset() {
        for (int i = 0; i < 96000; ++i) buffer_[i] = 0.0f;
        writePos_ = 0;
    }

private:
    static inline float clamp(float v, float lo, float hi) {
        return (v < lo) ? lo : (hi < v) ? hi : v;
    }

    float buffer_[96000];
    int writePos_;
    float time_;
    float feedback_;
    float sampleRate_;
    float output_;
};

#endif
