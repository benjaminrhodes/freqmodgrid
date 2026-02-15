#ifndef FILTER_H
#define FILTER_H

#include <cmath>

class Filter {
public:
    enum Type { LOWPASS, HIGHPASS };

    Filter() : type_(LOWPASS), cutoff_(12000.0f), resonance_(0.0f),
               sampleRate_(48000.0f), lpState_(0.0f), hpState_(0.0f) {
        calcCoefs();
    }

    void setType(Type type) { type_ = type; }
    void setType(int type) { type_ = (type == 0) ? LOWPASS : HIGHPASS; }
    
    void setCutoff(float cutoff) {
        cutoff_ = clamp(cutoff, 20.0f, 20000.0f);
        calcCoefs();
    }
    
    void setResonance(float res) {
        resonance_ = clamp(res, 0.0f, 1.0f);
        calcCoefs();
    }

    void setSampleRate(float sr) {
        sampleRate_ = sr;
        calcCoefs();
    }

    void process(float input) {
        float wc = cutoff_ * 6.28318530718f / sampleRate_;
        float q = 0.5f + resonance_ * 10.0f;
        
        float alpha = sin(wc) / (2.0f * q);
        float a0 = 1.0f + alpha;
        
        if (type_ == LOWPASS) {
            lpState_ += alpha * (input - lpState_);
            output_ = lpState_;
        } else {
            hpState_ += alpha * (input - hpState_);
            output_ = input - hpState_;
        }
    }

    float getOutput() const { return output_; }
    Type getType() const { return type_; }
    float getCutoff() const { return cutoff_; }
    float getResonance() const { return resonance_; }

    void reset() {
        lpState_ = 0.0f;
        hpState_ = 0.0f;
        output_ = 0.0f;
    }

private:
    void calcCoefs() {
        // Simplified one-pole filter coefficients
        float wc = cutoff_ * 6.28318530718f / sampleRate_;
        float alpha = wc / (1.0f + wc);
        
        if (type_ == LOWPASS) {
            lpAlpha_ = alpha;
        } else {
            hpAlpha_ = alpha;
        }
    }

    static inline float clamp(float v, float lo, float hi) {
        return (v < lo) ? lo : (hi < v) ? hi : v;
    }

    Type type_;
    float cutoff_;
    float resonance_;
    float sampleRate_;
    float lpAlpha_, hpAlpha_;
    float lpState_, hpState_;
    float output_;
};

#endif
