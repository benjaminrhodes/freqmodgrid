#ifndef FM_ENGINE_H
#define FM_ENGINE_H

#include "Operator.h"
#include "Envelope.h"
#include "Filter.h"
#include "LFO.h"
#include "Effects.h"

// Each algorithm defines: which operators modulate which, and which are carriers (go to output).
// Operators are processed in the order specified by processOrder so modulators run before carriers.
struct AlgorithmDef {
    // For each operator, list of operator indices that modulate it (phase mod sources)
    int modulators[6][6]; // modulators[op][i] = source op index, -1 = end
    // Which operators are carriers (contribute to audio output)
    bool isCarrier[6];
    // Processing order: modulators first, then carriers
    int processOrder[6];
};

// 8 algorithms matching the spec:
// 1: 1>2>3>4>5>6 (serial chain, op6 is carrier)
// 2: (1+2)>3>4>5>6 (ops 1,2 parallel into 3, chain to 6)
// 3: 1>(2+3+4+5+6) (op1 modulates all, ops 2-6 are carriers)
// 4: ((1+2)+(3+4))>5>6 (two pairs summed, chain to 6)
// 5: 1>2, 3>4, 5>6 (3 parallel pairs; ops 2,4,6 are carriers)
// 6: (1+2+3)>(4+5+6) (group 1-3 mods group 4-6; ops 4,5,6 are carriers)
// 7: 1>2>3, 4>5>6 (2 parallel chains; ops 3,6 are carriers)
// 8: All summed to output (no modulation, all carriers)
static const AlgorithmDef kAlgorithms[8] = {
    // Algo 1: 1>2>3>4>5>6 (serial). Carrier: 6 only.
    // Op0 has no mod. Op1 modulated by op0. Op2 by op1. Op3 by op2. Op4 by op3. Op5 by op4.
    {
        {{-1},{0,-1},{1,-1},{2,-1},{3,-1},{4,-1}},
        {false, false, false, false, false, true},
        {0, 1, 2, 3, 4, 5}
    },
    // Algo 2: (1+2)>3>4>5>6. Carriers: 6.
    // Op2 modulated by op0+op1. Op3 by op2. Op4 by op3. Op5 by op4.
    {
        {{-1},{-1},{0,1,-1},{2,-1},{3,-1},{4,-1}},
        {false, false, false, false, false, true},
        {0, 1, 2, 3, 4, 5}
    },
    // Algo 3: 1>(2+3+4+5+6). Op0 modulates ops 1-5. Carriers: 1,2,3,4,5.
    {
        {{-1},{0,-1},{0,-1},{0,-1},{0,-1},{0,-1}},
        {false, true, true, true, true, true},
        {0, 1, 2, 3, 4, 5}
    },
    // Algo 4: ((1+2)+(3+4))>5>6. Carriers: 6.
    // Op4 modulated by op0+op1+op2+op3. Op5 by op4.
    {
        {{-1},{-1},{-1},{-1},{0,1,2,3,-1},{4,-1}},
        {false, false, false, false, false, true},
        {0, 1, 2, 3, 4, 5}
    },
    // Algo 5: 1>2, 3>4, 5>6 (3 parallel pairs). Carriers: 2,4,6 (indices 1,3,5).
    {
        {{-1},{0,-1},{-1},{2,-1},{-1},{4,-1}},
        {false, true, false, true, false, true},
        {0, 1, 2, 3, 4, 5}
    },
    // Algo 6: (1+2+3)>(4+5+6). Ops 0,1,2 modulate ops 3,4,5. Carriers: 4,5,6 (indices 3,4,5).
    {
        {{-1},{-1},{-1},{0,1,2,-1},{0,1,2,-1},{0,1,2,-1}},
        {false, false, false, true, true, true},
        {0, 1, 2, 3, 4, 5}
    },
    // Algo 7: 1>2>3, 4>5>6 (2 parallel chains). Carriers: 3,6 (indices 2,5).
    {
        {{-1},{0,-1},{1,-1},{-1},{3,-1},{4,-1}},
        {false, false, true, false, false, true},
        {0, 1, 2, 3, 4, 5}
    },
    // Algo 8: All summed to output (no modulation). All carriers.
    {
        {{-1},{-1},{-1},{-1},{-1},{-1}},
        {true, true, true, true, true, true},
        {0, 1, 2, 3, 4, 5}
    }
};

class FMEngine {
public:
    static const int NUM_OPERATORS = 6;
    static const int NUM_ALGORITHMS = 8;
    static const int NUM_VOICES = 16;
    static const int NUM_LFOS = 2;

    FMEngine() : sampleRate_(48000.0f), masterVolume_(0.7f), algorithm_(0),
                 voiceAge_(0) {
        for (int i = 0; i < NUM_VOICES; ++i) {
            voices_[i].active = false;
            voices_[i].note = -1;
            voices_[i].age = 0;
        }

        // Default operator settings
        opRatio_[0] = 1.0f;  opLevel_[0] = 0.5f;  opFeedback_[0] = 0.0f;
        opRatio_[1] = 2.0f;  opLevel_[1] = 0.5f;  opFeedback_[1] = 0.0f;
        opRatio_[2] = 3.0f;  opLevel_[2] = 0.5f;  opFeedback_[2] = 0.0f;
        opRatio_[3] = 1.0f;  opLevel_[3] = 0.0f;  opFeedback_[3] = 0.0f;
        opRatio_[4] = 0.5f;  opLevel_[4] = 0.0f;  opFeedback_[4] = 0.0f;
        opRatio_[5] = 0.25f; opLevel_[5] = 0.0f;  opFeedback_[5] = 0.0f;

        envAttack_ = 0.01f;
        envDecay_ = 0.1f;
        envSustain_ = 0.7f;
        envRelease_ = 0.3f;

        filterType_ = 0;
        filterCutoff_ = 12000.0f;
        filterResonance_ = 0.0f;

        for (int i = 0; i < NUM_LFOS; ++i) {
            lfoRate_[i] = (i == 0) ? 1.0f : 2.0f;
            lfoDepth_[i] = 0.0f;
            lfoWave_[i] = 0;
        }

        chorus_.setSampleRate(sampleRate_);
        delay_.setSampleRate(sampleRate_);
    }

    void setSampleRate(float sr) {
        sampleRate_ = sr;
        chorus_.setSampleRate(sr);
        delay_.setSampleRate(sr);
        // Update all active voices
        for (int v = 0; v < NUM_VOICES; ++v) {
            if (voices_[v].active) {
                applyParamsToVoice(voices_[v]);
            }
        }
    }

    void noteOn(int note, float velocity) {
        int voiceIndex = findFreeVoice();
        if (voiceIndex < 0) voiceIndex = stealVoice();

        Voice& voice = voices_[voiceIndex];
        voice.active = true;
        voice.note = note;
        voice.velocity = velocity;
        voice.frequency = 440.0f * std::pow(2.0f, (note - 69) / 12.0f);
        voice.age = ++voiceAge_;

        for (int i = 0; i < NUM_OPERATORS; ++i) {
            voice.operators[i].reset();
        }
        voice.envelope.reset();
        voice.filter.reset();
        for (int i = 0; i < NUM_LFOS; ++i) {
            voice.lfos[i].reset();
        }

        applyParamsToVoice(voice);
        voice.envelope.trigger();
    }

    void noteOff(int note) {
        for (int i = 0; i < NUM_VOICES; ++i) {
            if (voices_[i].active && voices_[i].note == note) {
                voices_[i].envelope.release();
            }
        }
    }

    void process(float* outputLeft, float* outputRight, int numSamples) {
        for (int s = 0; s < numSamples; ++s) {
            float mix = 0.0f;

            for (int v = 0; v < NUM_VOICES; ++v) {
                Voice& voice = voices_[v];

                if (!voice.active) continue;
                if (!voice.envelope.isActive()) {
                    voice.active = false;
                    continue;
                }

                // Process LFOs
                for (int i = 0; i < NUM_LFOS; ++i) {
                    voice.lfos[i].process();
                }

                // Apply LFO1 to operator ratios (vibrato)
                float lfo1Out = voice.lfos[0].getOutput();
                // Apply LFO2 to filter cutoff
                float lfo2Out = voice.lfos[1].getOutput();

                // Update operator frequencies with LFO1 vibrato
                for (int op = 0; op < NUM_OPERATORS; ++op) {
                    float freqMod = 1.0f + lfo1Out * 0.05f; // +/- 5% pitch modulation
                    voice.operators[op].setFrequency(
                        voice.frequency * freqMod, sampleRate_);
                }

                // Process envelope
                voice.envelope.process();
                float amp = voice.envelope.getLevel() * voice.velocity * masterVolume_;

                // Process operators in algorithm order
                const AlgorithmDef& algo = kAlgorithms[algorithm_];
                float opOutput[NUM_OPERATORS] = {0.0f};

                for (int idx = 0; idx < NUM_OPERATORS; ++idx) {
                    int op = algo.processOrder[idx];

                    // Sum modulation inputs from this op's modulators
                    float modInput = 0.0f;
                    for (int m = 0; m < NUM_OPERATORS; ++m) {
                        int modSrc = algo.modulators[op][m];
                        if (modSrc < 0) break;
                        modInput += opOutput[modSrc] * 5.0f;
                    }

                    voice.operators[op].setModulatorInput(modInput);
                    voice.operators[op].process();
                    opOutput[op] = voice.operators[op].getOutput();
                }

                // Sum only carrier operators
                float voiceOut = 0.0f;
                int numCarriers = 0;
                for (int op = 0; op < NUM_OPERATORS; ++op) {
                    if (algo.isCarrier[op]) {
                        voiceOut += opOutput[op];
                        numCarriers++;
                    }
                }
                // Normalize by number of carriers to keep consistent level
                if (numCarriers > 1) {
                    voiceOut /= std::sqrt(static_cast<float>(numCarriers));
                }

                voiceOut *= amp;

                // Apply filter with LFO2 modulation on cutoff
                float modCutoff = filterCutoff_ * (1.0f + lfo2Out * 0.5f);
                if (modCutoff < 20.0f) modCutoff = 20.0f;
                if (modCutoff > 20000.0f) modCutoff = 20000.0f;
                voice.filter.setCutoff(modCutoff);
                voice.filter.process(voiceOut);
                float filtered = voice.filter.getOutput();

                mix += filtered;
            }

            // Soft clip to prevent harsh distortion from stacked voices
            mix *= 0.5f;

            // Effects chain (global, not per-voice)
            chorus_.process(mix);
            float chorusOut = chorus_.getOutput();

            delay_.process(chorusOut);
            float delayOut = delay_.getOutput();

            outputLeft[s] = delayOut;
            outputRight[s] = delayOut;
        }
    }

    // Parameter setters — update stored values AND propagate to all active voices
    void setOperatorRatio(int op, float ratio) {
        if (op >= 0 && op < NUM_OPERATORS) {
            opRatio_[op] = ratio;
            propagateOperatorParams();
        }
    }

    void setOperatorLevel(int op, float level) {
        if (op >= 0 && op < NUM_OPERATORS) {
            opLevel_[op] = level;
            propagateOperatorParams();
        }
    }

    void setOperatorFeedback(int op, float fb) {
        if (op >= 0 && op < NUM_OPERATORS) {
            opFeedback_[op] = fb;
            propagateOperatorParams();
        }
    }

    void setAlgorithm(int algo) {
        algorithm_ = (algo >= 0 && algo < NUM_ALGORITHMS) ? algo : 0;
    }

    void setFilterType(int type) {
        filterType_ = type;
        for (int v = 0; v < NUM_VOICES; ++v) {
            if (voices_[v].active) voices_[v].filter.setType(type);
        }
    }
    void setFilterCutoff(float cutoff) {
        filterCutoff_ = cutoff;
        for (int v = 0; v < NUM_VOICES; ++v) {
            if (voices_[v].active) voices_[v].filter.setCutoff(cutoff);
        }
    }
    void setFilterResonance(float res) {
        filterResonance_ = res;
        for (int v = 0; v < NUM_VOICES; ++v) {
            if (voices_[v].active) voices_[v].filter.setResonance(res);
        }
    }

    void setAttack(float attack) {
        envAttack_ = attack;
        for (int v = 0; v < NUM_VOICES; ++v) {
            if (voices_[v].active) voices_[v].envelope.setAttack(attack);
        }
    }
    void setDecay(float decay) {
        envDecay_ = decay;
        for (int v = 0; v < NUM_VOICES; ++v) {
            if (voices_[v].active) voices_[v].envelope.setDecay(decay);
        }
    }
    void setSustain(float sustain) {
        envSustain_ = sustain;
        for (int v = 0; v < NUM_VOICES; ++v) {
            if (voices_[v].active) voices_[v].envelope.setSustain(sustain);
        }
    }
    void setRelease(float release) {
        envRelease_ = release;
        for (int v = 0; v < NUM_VOICES; ++v) {
            if (voices_[v].active) voices_[v].envelope.setRelease(release);
        }
    }

    void setLFORate(int lfo, float rate) {
        if (lfo >= 0 && lfo < NUM_LFOS) {
            lfoRate_[lfo] = rate;
            for (int v = 0; v < NUM_VOICES; ++v) {
                if (voices_[v].active) voices_[v].lfos[lfo].setRate(rate);
            }
        }
    }
    void setLFODepth(int lfo, float depth) {
        if (lfo >= 0 && lfo < NUM_LFOS) {
            lfoDepth_[lfo] = depth;
            for (int v = 0; v < NUM_VOICES; ++v) {
                if (voices_[v].active) voices_[v].lfos[lfo].setDepth(depth);
            }
        }
    }
    void setLFOWave(int lfo, int wave) {
        if (lfo >= 0 && lfo < NUM_LFOS) {
            lfoWave_[lfo] = wave;
            for (int v = 0; v < NUM_VOICES; ++v) {
                if (voices_[v].active) voices_[v].lfos[lfo].setWave(wave);
            }
        }
    }

    void setChorusRate(float rate) { chorus_.setRate(rate); }
    void setChorusDepth(float depth) { chorus_.setDepth(depth); }
    void setDelayTime(float time) { delay_.setTime(time); }
    void setDelayFeedback(float fb) { delay_.setFeedback(fb); }

    void setMasterVolume(float vol) { masterVolume_ = vol; }

    // Getters
    float getOperatorRatio(int op) const {
        return (op >= 0 && op < NUM_OPERATORS) ? opRatio_[op] : 0;
    }
    float getOperatorLevel(int op) const {
        return (op >= 0 && op < NUM_OPERATORS) ? opLevel_[op] : 0;
    }
    float getOperatorFeedback(int op) const {
        return (op >= 0 && op < NUM_OPERATORS) ? opFeedback_[op] : 0;
    }

    float getFilterCutoff() const { return filterCutoff_; }
    float getFilterResonance() const { return filterResonance_; }
    int getFilterType() const { return filterType_; }

    float getAttack() const { return envAttack_; }
    float getDecay() const { return envDecay_; }
    float getSustain() const { return envSustain_; }
    float getRelease() const { return envRelease_; }

    float getLFORate(int lfo) const {
        return (lfo >= 0 && lfo < NUM_LFOS) ? lfoRate_[lfo] : 0;
    }
    float getLFODepth(int lfo) const {
        return (lfo >= 0 && lfo < NUM_LFOS) ? lfoDepth_[lfo] : 0;
    }

    int getAlgorithm() const { return algorithm_; }
    float getMasterVolume() const { return masterVolume_; }

private:
    struct Voice {
        bool active;
        int note;
        float velocity;
        float frequency;
        unsigned long age;
        Operator operators[NUM_OPERATORS];
        Envelope envelope;
        Filter filter;
        LFO lfos[NUM_LFOS];
    };

    // Apply all stored parameters to a voice (used on noteOn and setSampleRate)
    void applyParamsToVoice(Voice& voice) {
        for (int i = 0; i < NUM_OPERATORS; ++i) {
            voice.operators[i].setSampleRate(sampleRate_);
            voice.operators[i].setRatio(opRatio_[i]);
            voice.operators[i].setLevel(opLevel_[i]);
            voice.operators[i].setFeedback(opFeedback_[i]);
            voice.operators[i].setFrequency(voice.frequency, sampleRate_);
        }
        voice.envelope.setSampleRate(sampleRate_);
        voice.envelope.setAttack(envAttack_);
        voice.envelope.setDecay(envDecay_);
        voice.envelope.setSustain(envSustain_);
        voice.envelope.setRelease(envRelease_);

        voice.filter.setSampleRate(sampleRate_);
        voice.filter.setType(filterType_);
        voice.filter.setCutoff(filterCutoff_);
        voice.filter.setResonance(filterResonance_);

        for (int i = 0; i < NUM_LFOS; ++i) {
            voice.lfos[i].setSampleRate(sampleRate_);
            voice.lfos[i].setRate(lfoRate_[i]);
            voice.lfos[i].setDepth(lfoDepth_[i]);
            voice.lfos[i].setWave(lfoWave_[i]);
        }
    }

    // Propagate operator params to all active voices
    void propagateOperatorParams() {
        for (int v = 0; v < NUM_VOICES; ++v) {
            if (voices_[v].active) {
                for (int i = 0; i < NUM_OPERATORS; ++i) {
                    voices_[v].operators[i].setRatio(opRatio_[i]);
                    voices_[v].operators[i].setLevel(opLevel_[i]);
                    voices_[v].operators[i].setFeedback(opFeedback_[i]);
                    voices_[v].operators[i].setFrequency(
                        voices_[v].frequency, sampleRate_);
                }
            }
        }
    }

    int findFreeVoice() {
        for (int i = 0; i < NUM_VOICES; ++i) {
            if (!voices_[i].active) return i;
        }
        return -1;
    }

    // Steal the oldest voice (lowest age value)
    int stealVoice() {
        int oldest = 0;
        for (int i = 1; i < NUM_VOICES; ++i) {
            if (voices_[i].age < voices_[oldest].age) {
                oldest = i;
            }
        }
        return oldest;
    }

    // Stored parameter values (source of truth)
    float opRatio_[NUM_OPERATORS];
    float opLevel_[NUM_OPERATORS];
    float opFeedback_[NUM_OPERATORS];

    float envAttack_, envDecay_, envSustain_, envRelease_;
    int filterType_;
    float filterCutoff_, filterResonance_;
    float lfoRate_[NUM_LFOS], lfoDepth_[NUM_LFOS];
    int lfoWave_[NUM_LFOS];

    // Global effects (post voice mixing)
    Chorus chorus_;
    FMDelay delay_;

    Voice voices_[NUM_VOICES];

    int algorithm_;
    float sampleRate_;
    float masterVolume_;
    unsigned long voiceAge_;
};

#endif
