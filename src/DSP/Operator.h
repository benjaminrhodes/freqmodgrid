#ifndef OPERATOR_H
#define OPERATOR_H

#include <cmath>
#include <algorithm>
#include <limits>

template<typename T>
constexpr const T& clamp(const T& v, const T& lo, const T& hi) {
    return (v < lo) ? lo : (hi < v) ? hi : v;
}

class Operator {
public:
    Operator() : ratio_(1.0f), level_(0.5f), feedback_(0.0f), detune_(0.0f),
                 phase_(0.0f), output_(0.0f), feedbackSample_(0.0f) {}

    void setRatio(float ratio) { ratio_ = clamp(ratio, 0.25f, 32.0f); }
    void setLevel(float level) { level_ = clamp(level, 0.0f, 1.0f); }
    void setFeedback(float fb) { feedback_ = clamp(fb, 0.0f, 1.0f); }
    void setDetune(float cents) { detune_ = clamp(cents, -100.0f, 100.0f); }

    float getRatio() const { return ratio_; }
    float getLevel() const { return level_; }
    float getFeedback() const { return feedback_; }

    void setSampleRate(float sampleRate) { sampleRate_ = sampleRate; }

    void setFrequency(float freq, float sampleRate) {
        baseFreq_ = freq;
        sampleRate_ = sampleRate;
        increment_ = (ratio_ * freq * std::pow(2.0f, detune_ / 1200.0f)) / sampleRate;
    }

    void setModulatorInput(float mod) {
        modulatorInput_ = mod;
    }

    void process() {
        phase_ += increment_;
        if (phase_ >= 1.0f) phase_ -= 1.0f;

        float fb = feedback_ * feedbackSample_ * 5.0f;
        float phasePos = phase_ + fb;
        
        output_ = level_ * fastSin(phasePos * 6.28318530718f + modulatorInput_);
        feedbackSample_ = output_;
    }

    float getOutput() const { return output_; }

    void reset() {
        phase_ = 0.0f;
        output_ = 0.0f;
        feedbackSample_ = 0.0f;
        modulatorInput_ = 0.0f;
    }

private:
    static inline float fastSin(float x) {
        x = fmodf(x, 6.28318530718f);
        if (x < 0) x += 6.28318530718f;
        
        float x2 = x * x;
        float x3 = x2 * x;
        float x5 = x3 * x2;
        float x7 = x5 * x2;
        
        return x - (x3 / 6.0f) + (x5 / 120.0f) - (x7 / 5040.0f);
    }

    float ratio_;
    float level_;
    float feedback_;
    float detune_;
    float phase_;
    float output_;
    float feedbackSample_;
    float modulatorInput_;
    float baseFreq_;
    float sampleRate_;
    float increment_;
};

#endif
