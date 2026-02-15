#ifndef LFO_H
#define LFO_H

#include <cmath>

class LFO {
public:
    enum Wave { WAVE_SINE, WAVE_SAW, WAVE_SQUARE, WAVE_TRIANGLE };

    LFO() : rate_(1.0f), depth_(0.0f), phase_(0.0f), output_(0.0f),
             wave_(WAVE_SINE), sync_(true), sampleRate_(48000.0f) {
        updateIncrement();
    }

    void setRate(float rate) {
        rate_ = clamp(rate, 0.01f, 20.0f);
        updateIncrement();
    }

    void setDepth(float depth) {
        depth_ = clamp(depth, 0.0f, 1.0f);
    }

    void setWave(Wave wave) { wave_ = wave; }
    void setWave(int wave) { wave_ = static_cast<Wave>(clamp(wave, 0, 3)); }

    void setSync(bool sync) { sync_ = sync; }

    void setSampleRate(float sr) {
        sampleRate_ = sr;
        updateIncrement();
    }

    void setPhase(float phase) {
        phase_ = fmod(phase, 1.0f);
        if (phase_ < 0) phase_ += 1.0f;
    }

    void process() {
        switch (wave_) {
            case WAVE_SINE:
                output_ = sin(phase_ * 6.28318530718f);
                break;
            case WAVE_SAW:
                output_ = 2.0f * phase_ - 1.0f;
                break;
            case WAVE_SQUARE:
                output_ = (phase_ < 0.5f) ? 1.0f : -1.0f;
                break;
            case WAVE_TRIANGLE:
                output_ = (phase_ < 0.5f) ? (4.0f * phase_ - 1.0f) : (3.0f - 4.0f * phase_);
                break;
        }

        phase_ += increment_;
        if (phase_ >= 1.0f) phase_ -= 1.0f;
    }

    float getOutput() const { return depth_ * output_; }
    float getRawOutput() const { return output_; }
    float getRate() const { return rate_; }
    float getDepth() const { return depth_; }
    Wave getWave() const { return wave_; }

    void reset() {
        phase_ = 0.0f;
        output_ = 0.0f;
    }

private:
    void updateIncrement() {
        increment_ = rate_ / sampleRate_;
    }

    static inline float clamp(float v, float lo, float hi) {
        return (v < lo) ? lo : (hi < v) ? hi : v;
    }

    float rate_;
    float depth_;
    float phase_;
    float output_;
    float increment_;
    Wave wave_;
    bool sync_;
    float sampleRate_;
};

#endif
