#ifndef FM_ENGINE_H
#define FM_ENGINE_H

#include "Operator.h"
#include "Envelope.h"
#include "Filter.h"
#include "LFO.h"
#include "Effects.h"

class FMEngine {
public:
    static const int NUM_OPERATORS = 6;
    static const int NUM_ALGORITHMS = 8;
    static const int NUM_VOICES = 16;
    static const int NUM_LFOS = 2;

    FMEngine() : sampleRate_(48000.0f), masterVolume_(0.7f) {
        for (int i = 0; i < NUM_VOICES; ++i) {
            voices_[i].active = false;
            voices_[i].note = -1;
        }
        
        for (int i = 0; i < NUM_OPERATORS; ++i) {
            operators_[i].setSampleRate(sampleRate_);
        }
        
        envelope_.setSampleRate(sampleRate_);
        filter_.setSampleRate(sampleRate_);
        
        for (int i = 0; i < NUM_LFOS; ++i) {
            lfos_[i].setSampleRate(sampleRate_);
        }
        
        chorus_.setSampleRate(sampleRate_);
        delay_.setSampleRate(sampleRate_);
        
        setAlgorithm(0);
    }

    void setSampleRate(float sr) {
        sampleRate_ = sr;
        for (int i = 0; i < NUM_OPERATORS; ++i) {
            operators_[i].setSampleRate(sr);
        }
        envelope_.setSampleRate(sr);
        filter_.setSampleRate(sr);
        for (int i = 0; i < NUM_LFOS; ++i) {
            lfos_[i].setSampleRate(sr);
        }
        chorus_.setSampleRate(sr);
        delay_.setSampleRate(sr);
    }

    void noteOn(int note, int velocity) {
        int voiceIndex = findFreeVoice();
        if (voiceIndex < 0) voiceIndex = stealVoice(note);
        
        Voice& voice = voices_[voiceIndex];
        voice.active = true;
        voice.note = note;
        voice.velocity = velocity / 127.0f;
        voice.frequency = 440.0f * pow(2.0f, (note - 69) / 12.0f);
        
        for (int i = 0; i < NUM_OPERATORS; ++i) {
            voice.operators[i].setSampleRate(sampleRate_);
            voice.operators[i].setFrequency(voice.frequency, sampleRate_);
            voice.operators[i].reset();
        }
        
        voice.envelope.setSampleRate(sampleRate_);
        voice.envelope.trigger();
        
        voice.filter.setSampleRate(sampleRate_);
        voice.filter.setType(filter_.getType());
        voice.filter.setCutoff(filter_.getCutoff());
        voice.filter.setResonance(filter_.getResonance());
        
        for (int i = 0; i < NUM_LFOS; ++i) {
            voice.lfos[i].setSampleRate(sampleRate_);
            voice.lfos[i].reset();
        }
    }

    void noteOff(int note) {
        for (int i = 0; i < NUM_VOICES; ++i) {
            if (voices_[i].active && voices_[i].note == note) {
                voices_[i].envelope.release();
            }
        }
    }

    void process(float* outputLeft, float* outputRight, int numSamples) {
        float mix = 0.0f;
        
        for (int v = 0; v < NUM_VOICES; ++v) {
            Voice& voice = voices_[v];
            
            if (!voice.envelope.isActive()) {
                voice.active = false;
                continue;
            }
            
            for (int i = 0; i < NUM_LFOS; ++i) {
                voice.lfos[i].process();
            }
            
            voice.envelope.process();
            
            float amp = voice.envelope.getLevel() * voice.velocity * masterVolume_;
            
            float opOutput[NUM_OPERATORS];
            
            for (int op = 0; op < NUM_OPERATORS; ++op) {
                voice.operators[op].setFrequency(voice.frequency, sampleRate_);
                
                float modInput = 0.0f;
                if (algorithm_ == 0) {
                    if (op > 0) modInput = voice.operators[op - 1].getOutput() * 5.0f;
                } else if (algorithm_ == 1) {
                    if (op == 1) modInput = voice.operators[0].getOutput() * 5.0f;
                    else if (op > 1) modInput = voice.operators[op - 1].getOutput() * 5.0f;
                } else if (algorithm_ == 2) {
                    if (op == 0) modInput = 0.0f;
                    else modInput = (voice.operators[0].getOutput() + voice.operators[1].getOutput() + 
                                   voice.operators[2].getOutput() + voice.operators[3].getOutput() +
                                   voice.operators[4].getOutput()) * 5.0f;
                } else if (algorithm_ == 3) {
                    if (op == 0) modInput = 0.0f;
                    else if (op == 1) modInput = voice.operators[0].getOutput() * 5.0f;
                    else if (op == 2) modInput = voice.operators[1].getOutput() * 5.0f;
                    else if (op == 3) modInput = (voice.operators[0].getOutput() + voice.operators[1].getOutput()) * 5.0f;
                    else if (op == 4) modInput = (voice.operators[2].getOutput() + voice.operators[3].getOutput()) * 5.0f;
                    else if (op == 5) modInput = voice.operators[4].getOutput() * 5.0f;
                } else if (algorithm_ == 4) {
                    if (op == 0) modInput = voice.operators[1].getOutput() * 5.0f;
                    else if (op == 1) modInput = 0.0f;
                    else if (op == 2) modInput = voice.operators[3].getOutput() * 5.0f;
                    else if (op == 3) modInput = 0.0f;
                    else if (op == 4) modInput = voice.operators[5].getOutput() * 5.0f;
                    else if (op == 5) modInput = 0.0f;
                } else if (algorithm_ == 5) {
                    if (op < 3) modInput = voice.operators[op].getOutput() * 5.0f;
                    else modInput = voice.operators[op].getOutput() * 5.0f;
                } else if (algorithm_ == 6) {
                    if (op == 0) modInput = voice.operators[1].getOutput() * 5.0f;
                    else if (op == 1) modInput = 0.0f;
                    else if (op == 2) modInput = voice.operators[3].getOutput() * 5.0f;
                    else if (op == 3) modInput = voice.operators[4].getOutput() * 5.0f;
                    else if (op == 4) modInput = voice.operators[5].getOutput() * 5.0f;
                    else if (op == 5) modInput = 0.0f;
                } else {
                    modInput = 0.0f;
                }
                
                voice.operators[op].setModulatorInput(modInput);
                voice.operators[op].process();
                opOutput[op] = voice.operators[op].getOutput();
            }
            
            float voiceOut = 0.0f;
            for (int op = 0; op < NUM_OPERATORS; ++op) {
                voiceOut += opOutput[op];
            }
            
            voiceOut *= amp;
            
            voice.filter.process(voiceOut);
            float filtered = voice.filter.getOutput();
            
            mix += filtered;
        }
        
        mix /= 2.0f;
        
        chorus_.process(mix);
        float chorusOut = chorus_.getOutput();
        
        delay_.process(chorusOut);
        float delayOut = delay_.getOutput();
        
        *outputLeft = delayOut;
        *outputRight = delayOut;
    }

    void setOperatorRatio(int op, float ratio) {
        if (op >= 0 && op < NUM_OPERATORS) {
            operators_[op].setRatio(ratio);
        }
    }

    void setOperatorLevel(int op, float level) {
        if (op >= 0 && op < NUM_OPERATORS) {
            operators_[op].setLevel(level);
        }
    }

    void setOperatorFeedback(int op, float fb) {
        if (op >= 0 && op < NUM_OPERATORS) {
            operators_[op].setFeedback(fb);
        }
    }

    void setAlgorithm(int algo) {
        algorithm_ = (algo >= 0 && algo < NUM_ALGORITHMS) ? algo : 0;
    }

    void setFilterType(int type) { filter_.setType(type); }
    void setFilterCutoff(float cutoff) { filter_.setCutoff(cutoff); }
    void setFilterResonance(float res) { filter_.setResonance(res); }

    void setAttack(float attack) { envelope_.setAttack(attack); }
    void setDecay(float decay) { envelope_.setDecay(decay); }
    void setSustain(float sustain) { envelope_.setSustain(sustain); }
    void setRelease(float release) { envelope_.setRelease(release); }

    void setLFORate(int lfo, float rate) {
        if (lfo >= 0 && lfo < NUM_LFOS) lfos_[lfo].setRate(rate);
    }
    void setLFODepth(int lfo, float depth) {
        if (lfo >= 0 && lfo < NUM_LFOS) lfos_[lfo].setDepth(depth);
    }
    void setLFOWave(int lfo, int wave) {
        if (lfo >= 0 && lfo < NUM_LFOS) lfos_[lfo].setWave(wave);
    }

    void setChorusRate(float rate) { chorus_.setRate(rate); }
    void setChorusDepth(float depth) { chorus_.setDepth(depth); }
    void setDelayTime(float time) { delay_.setTime(time); }
    void setDelayFeedback(float fb) { delay_.setFeedback(fb); }

    void setMasterVolume(float vol) { masterVolume_ = vol; }

    float getOperatorRatio(int op) const {
        return (op >= 0 && op < NUM_OPERATORS) ? operators_[op].getRatio() : 0;
    }
    float getOperatorLevel(int op) const {
        return (op >= 0 && op < NUM_OPERATORS) ? operators_[op].getLevel() : 0;
    }
    float getOperatorFeedback(int op) const {
        return (op >= 0 && op < NUM_OPERATORS) ? operators_[op].getFeedback() : 0;
    }

    float getFilterCutoff() const { return filter_.getCutoff(); }
    float getFilterResonance() const { return filter_.getResonance(); }
    int getFilterType() const { return filter_.getType(); }

    float getAttack() const { return envelope_.getAttack(); }
    float getDecay() const { return envelope_.getDecay(); }
    float getSustain() const { return envelope_.getSustain(); }
    float getRelease() const { return envelope_.getRelease(); }

    float getLFORate(int lfo) const {
        return (lfo >= 0 && lfo < NUM_LFOS) ? lfos_[lfo].getRate() : 0;
    }
    float getLFODepth(int lfo) const {
        return (lfo >= 0 && lfo < NUM_LFOS) ? lfos_[lfo].getDepth() : 0;
    }

    float getAlgorithm() const { return algorithm_; }
    float getMasterVolume() const { return masterVolume_; }

private:
    struct Voice {
        bool active;
        int note;
        float velocity;
        float frequency;
        Operator operators[NUM_OPERATORS];
        Envelope envelope;
        Filter filter;
        LFO lfos[NUM_LFOS];
    };

    int findFreeVoice() {
        for (int i = 0; i < NUM_VOICES; ++i) {
            if (!voices_[i].active) return i;
        }
        return -1;
    }

    int stealVoice(int note) {
        int oldest = 0;
        for (int i = 1; i < NUM_VOICES; ++i) {
            if (voices_[i].note < voices_[oldest].note) {
                oldest = i;
            }
        }
        return oldest;
    }

    Operator operators_[NUM_OPERATORS];
    Envelope envelope_;
    Filter filter_;
    LFO lfos_[NUM_LFOS];
    Chorus chorus_;
    Delay delay_;

    Voice voices_[NUM_VOICES];

    int algorithm_;
    float sampleRate_;
    float masterVolume_;
};

#endif
