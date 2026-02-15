#ifndef ENVELOPE_H
#define ENVELOPE_H

class Envelope {
public:
    Envelope() : attack_(0.01f), decay_(0.1f), sustain_(0.7f), release_(0.3f),
                 attackCoef_(0.0f), decayCoef_(0.0f), releaseCoef_(0.0f),
                 attackBase_(0.0f), decayBase_(0.0f), releaseBase_(0.0f),
                 level_(0.0f), state_(ENV_IDLE), sampleRate_(48000.0f) {
        calcCoefs();
    }

    enum State { ENV_IDLE, ENV_ATTACK, ENV_DECAY, ENV_SUSTAIN, ENV_RELEASE };

    void setAttack(float attack) {
        attack_ = clamp(attack, 0.001f, 5.0f);
        calcCoefs();
    }
    void setDecay(float decay) {
        decay_ = clamp(decay, 0.001f, 5.0f);
        calcCoefs();
    }
    void setSustain(float sustain) {
        sustain_ = clamp(sustain, 0.0f, 1.0f);
        calcCoefs();
    }
    void setRelease(float release) {
        release_ = clamp(release, 0.01f, 10.0f);
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
            releaseBase_ = level_;
        }
    }

    void process() {
        switch (state_) {
            case ENV_IDLE:
                level_ = 0.0f;
                break;
            case ENV_ATTACK:
                level_ = attackBase_ + attackCoef_ * level_;
                if (level_ >= 1.0f) {
                    level_ = 1.0f;
                    state_ = ENV_DECAY;
                }
                break;
            case ENV_DECAY:
                level_ = decayBase_ + decayCoef_ * level_;
                if (level_ <= sustain_) {
                    level_ = sustain_;
                    state_ = ENV_SUSTAIN;
                }
                break;
            case ENV_SUSTAIN:
                level_ = sustain_;
                break;
            case ENV_RELEASE:
                level_ = releaseBase_ + releaseCoef_ * level_;
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
        attackCoef_ = -1.0f / (attack_ * sampleRate_);
        attackBase_ = 1.0f + attackCoef_;
        decayCoef_ = -1.0f / (decay_ * sampleRate_);
        decayBase_ = sustain_ * (1.0f - decayCoef_);
        releaseCoef_ = -1.0f / (release_ * sampleRate_);
    }

    static inline float clamp(float v, float lo, float hi) {
        return (v < lo) ? lo : (hi < v) ? hi : v;
    }

    float attack_, decay_, sustain_, release_;
    float attackCoef_, decayCoef_, releaseCoef_;
    float attackBase_, decayBase_, releaseBase_;
    float level_;
    State state_;
    float sampleRate_;
};

#endif
