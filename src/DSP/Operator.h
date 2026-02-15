#ifndef OPERATOR_H
#define OPERATOR_H

#include <cmath>

class Operator {
public:
    Operator() : ratio_(1.0f), level_(0.5f), feedback_(0.0f), detune_(0.0f),
                 phase_(0.0f), output_(0.0f), feedbackSample_(0.0f),
                 modulatorInput_(0.0f), baseFreq_(0.0f), sampleRate_(48000.0f),
                 increment_(0.0f) {}

    void setRatio(float ratio) { ratio_ = clampf(ratio, 0.25f, 32.0f); }
    void setLevel(float level) { level_ = clampf(level, 0.0f, 1.0f); }
    void setFeedback(float fb) { feedback_ = clampf(fb, 0.0f, 1.0f); }
    void setDetune(float cents) { detune_ = clampf(cents, -100.0f, 100.0f); }

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
        if (phase_ < 0.0f) phase_ += 1.0f;

        float fb = feedback_ * feedbackSample_ * 5.0f;
        float totalPhase = phase_ * 6.28318530718f + fb + modulatorInput_;

        output_ = level_ * fastSin(totalPhase);
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
    // Bhaskara I approximation — accurate across full [0, 2pi] range
    static inline float fastSin(float x) {
        constexpr float TWO_PI = 6.28318530718f;
        constexpr float PI = 3.14159265359f;

        x = std::fmod(x, TWO_PI);
        if (x < 0.0f) x += TWO_PI;

        float sign = 1.0f;
        if (x > PI) {
            x -= PI;
            sign = -1.0f;
        }

        // sin(x) ~ 16x(pi - x) / (5pi^2 - 4x(pi - x))  for x in [0, pi]
        float xpi = x * (PI - x);
        return sign * (16.0f * xpi) / (5.0f * PI * PI - 4.0f * xpi);
    }

    static inline float clampf(float v, float lo, float hi) {
        return (v < lo) ? lo : (hi < v) ? hi : v;
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
