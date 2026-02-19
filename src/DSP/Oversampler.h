#pragma once

#include "Constants.h"
#include <vector>

enum class OversampleMode { Off, x2, x4 };

template<typename T>
class Oversampler {
public:
    Oversampler() : mode_(OversampleMode::Off), ratio_(1), upBuffer_(0), downBuffer_(0) {}
    
    void setMode(OversampleMode mode) {
        mode_ = mode;
        switch (mode) {
            case OversampleMode::x2: ratio_ = 2; break;
            case OversampleMode::x4: ratio_ = 4; break;
            default: ratio_ = 1; break;
        }
    }
    
    OversampleMode getMode() const { return mode_; }
    int getRatio() const { return ratio_; }
    bool isActive() const { return mode_ != OversampleMode::Off; }
    
    void process(T* input, T* output, int numSamples, void (*processFunc)(T*, T*, int)) {
        if (mode_ == OversampleMode::Off) {
            processFunc(input, output, numSamples);
            return;
        }
        
        int upsampledSize = numSamples * ratio_;
        upBuffer_.resize(upsampledSize);
        downBuffer_.resize(upsampledSize);
        
        upsample(input, upBuffer_.data(), numSamples);
        processFunc(upBuffer_.data(), downBuffer_.data(), upsampledSize);
        downsample(downBuffer_.data(), output, numSamples);
    }
    
private:
    void upsample(T* input, T* output, int numSamples) {
        if (ratio_ == 2) {
            for (int i = 0; i < numSamples; i++) {
                output[i * 2] = input[i];
                output[i * 2 + 1] = input[i] * 0.5f;
            }
        } else if (ratio_ == 4) {
            for (int i = 0; i < numSamples; i++) {
                output[i * 4] = input[i];
                output[i * 4 + 1] = input[i] * 0.66f;
                output[i * 4 + 2] = input[i] * 0.33f;
                output[i * 4 + 3] = 0.0f;
            }
        }
    }
    
    void downsample(T* input, T* output, int numSamples) {
        if (ratio_ == 2) {
            for (int i = 0; i < numSamples; i++) {
                output[i] = (input[i * 2] + input[i * 2 + 1]) * 0.5f;
            }
        } else if (ratio_ == 4) {
            for (int i = 0; i < numSamples; i++) {
                output[i] = (input[i * 4] + input[i * 4 + 1] + input[i * 4 + 2] + input[i * 4 + 3]) * 0.25f;
            }
        }
    }
    
    OversampleMode mode_;
    int ratio_;
    std::vector<T> upBuffer_;
    std::vector<T> downBuffer_;
};
