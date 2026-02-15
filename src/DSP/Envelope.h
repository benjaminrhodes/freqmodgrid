#ifndef ENVELOPE_H
#define ENVELOPE_H

#include <cmath>

class Envelope {
public:
    Envelope() : attack_(0.01f), decay_(0.1f), sustain_(0.7f), release_(0.3f),
                 level_(0.0f), state_(ENV_IDLE), sampleRate_(48000.0f) {
        calcCoefs();
    }

    enum State { ENV_IDLE, ENV_ATTACK, ENV_DECAY, ENV_SUSTAIN, ENV_RELEASE };

    void setAttack(float attack) {
        attack_ = clampf(attack, 0.001f, 5.0f);
        calcCoefs();
    }
    void setDecay(float decay) {
        decay_ = clampf(decay, 0.001f, 5.0f);
        calcCoefs();
    }
    void setSustain(float sustain) {
        sustain_ = clampf(sustain, 0.0f, 1.0f);
        calcCoefs();
    }
    void setRelease(float release) {
        release_ = clampf(release, 0.01f, 10.0f);
        calcCoefs();
    }

    float getAttack() const { return attack_; }
    float getDecay() const { return decay_; }
    float getSustain() const { return sustain_; }
    float getRelease() const { return release_; }

    void setSampleRate(float sr) {
        sampleRate_ = sr;
        calcCoefs();
    }

    void trigger() {
        state_ = ENV_ATTACK;
    }

    void release() {
        if (state_ != ENV_IDLE) {
            state_ = ENV_RELEASE;
        }
    }

    void process() {
        switch (state_) {
            case ENV_IDLE:
                level_ = 0.0f;
                break;
            case ENV_ATTACK:
                level_ += attackRate_;
                if (level_ >= 1.0f) {
                    level_ = 1.0f;
                    state_ = ENV_DECAY;
                }
                break;
            case ENV_DECAY:
                level_ *= decayCoef_;
                if (level_ <= sustain_ + 0.0001f) {
                    level_ = sustain_;
                    state_ = ENV_SUSTAIN;
                }
                break;
            case ENV_SUSTAIN:
                level_ = sustain_;
                break;
            case ENV_RELEASE:
                level_ *= releaseCoef_;
                if (level_ <= 0.001f) {
                    level_ = 0.0f;
                    state_ = ENV_IDLE;
                }
                break;
        }
    }

    float getLevel() const { return level_; }
    State getState() const { return state_; }
    bool isActive() const { return state_ != ENV_IDLE; }

    void reset() {
        state_ = ENV_IDLE;
        level_ = 0.0f;
    }

private:
    void calcCoefs() {
        // Attack: linear ramp from 0 to 1 over attack_ seconds
        float attackSamples = attack_ * sampleRate_;
        attackRate_ = (attackSamples > 0.0f) ? (1.0f / attackSamples) : 1.0f;

        // Decay: exponential decay from 1.0 toward sustain_
        // We want to reach ~sustain_ in decay_ seconds.
        // Use exp(-k) per sample where k = ln(0.001) / (decay * sampleRate)
        // This gives ~60dB of decay over the decay time.
        float decaySamples = decay_ * sampleRate_;
        decayCoef_ = (decaySamples > 0.0f) ?
            std::pow(0.001f, 1.0f / decaySamples) : 0.0f;
        // Decay goes from 1.0 toward 0; we clamp at sustain_ in process()

        // Release: exponential decay from current level toward 0
        float releaseSamples = release_ * sampleRate_;
        releaseCoef_ = (releaseSamples > 0.0f) ?
            std::pow(0.001f, 1.0f / releaseSamples) : 0.0f;
    }

    static inline float clampf(float v, float lo, float hi) {
        return (v < lo) ? lo : (hi < v) ? hi : v;
    }

    float attack_, decay_, sustain_, release_;
    float attackRate_;
    float decayCoef_;
    float releaseCoef_;
    float level_;
    State state_;
    float sampleRate_;
};

#endif
