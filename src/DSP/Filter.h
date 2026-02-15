#ifndef FILTER_H
#define FILTER_H

#include <cmath>

// 2-pole biquad filter (12dB/oct) with resonance
class Filter {
public:
    enum Type { LOWPASS, HIGHPASS };

    Filter() : type_(LOWPASS), cutoff_(12000.0f), resonance_(0.0f),
               sampleRate_(48000.0f), output_(0.0f),
               b0_(1.0f), b1_(0.0f), b2_(0.0f),
               a1_(0.0f), a2_(0.0f), z1_(0.0f), z2_(0.0f) {
        calcCoefs();
    }

    void setType(Type type) { type_ = type; calcCoefs(); }
    void setType(int type) { type_ = (type == 0) ? LOWPASS : HIGHPASS; calcCoefs(); }

    void setCutoff(float cutoff) {
        cutoff_ = clampf(cutoff, 20.0f, 20000.0f);
        calcCoefs();
    }

    void setResonance(float res) {
        resonance_ = clampf(res, 0.0f, 1.0f);
        calcCoefs();
    }

    void setSampleRate(float sr) {
        sampleRate_ = sr;
        calcCoefs();
    }

    void process(float input) {
        // Direct Form II Transposed biquad
        output_ = b0_ * input + z1_;
        z1_ = b1_ * input - a1_ * output_ + z2_;
        z2_ = b2_ * input - a2_ * output_;
    }

    float getOutput() const { return output_; }
    Type getType() const { return type_; }
    float getCutoff() const { return cutoff_; }
    float getResonance() const { return resonance_; }

    void reset() {
        z1_ = z2_ = 0.0f;
        output_ = 0.0f;
    }

private:
    void calcCoefs() {
        float maxCutoff = sampleRate_ * 0.499f;
        float fc = clampf(cutoff_, 20.0f, maxCutoff);

        float wc = 2.0f * 3.14159265359f * fc / sampleRate_;
        float sinW = std::sin(wc);
        float cosW = std::cos(wc);

        // Q: resonance 0 = 0.707 (Butterworth), resonance 1 = Q of 12
        float Q = 0.707f + resonance_ * 11.293f;
        float alpha = sinW / (2.0f * Q);

        float a0;
        if (type_ == LOWPASS) {
            b0_ = (1.0f - cosW) * 0.5f;
            b1_ = 1.0f - cosW;
            b2_ = (1.0f - cosW) * 0.5f;
            a0 = 1.0f + alpha;
            a1_ = -2.0f * cosW;
            a2_ = 1.0f - alpha;
        } else {
            b0_ = (1.0f + cosW) * 0.5f;
            b1_ = -(1.0f + cosW);
            b2_ = (1.0f + cosW) * 0.5f;
            a0 = 1.0f + alpha;
            a1_ = -2.0f * cosW;
            a2_ = 1.0f - alpha;
        }

        // Normalize by a0
        b0_ /= a0;
        b1_ /= a0;
        b2_ /= a0;
        a1_ /= a0;
        a2_ /= a0;
    }

    static inline float clampf(float v, float lo, float hi) {
        return (v < lo) ? lo : (hi < v) ? hi : v;
    }

    Type type_;
    float cutoff_;
    float resonance_;
    float sampleRate_;
    float output_;

    // Biquad coefficients
    float b0_, b1_, b2_;
    float a1_, a2_;

    // State (Direct Form II Transposed)
    float z1_, z2_;
};

#endif
