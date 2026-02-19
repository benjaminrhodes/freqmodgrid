#pragma once

#include "Constants.h"
#include <vector>
#include <cmath>

class StereoChorus {
public:
    static constexpr int MAX_BUFFER_SIZE = 8192;
    
    StereoChorus() : rate_(1.0f), depth_(0.3f), sampleRate_(48000.0f),
                     writePos_(0), lfoPhaseMid_(0.0f), lfoPhaseSide_(0.25f) {
        bufferMid_.resize(MAX_BUFFER_SIZE, 0.0f);
        bufferSide_.resize(MAX_BUFFER_SIZE, 0.0f);
    }
    
    void setRate(float rate) { rate_ = clampf(rate, 0.1f, 10.0f); }
    void setDepth(float depth) { depth_ = clampf(depth, 0.0f, 1.0f); }
    void setSampleRate(float sr) { sampleRate_ = sr; }
    
    void process(float input, float& outLeft, float& outRight) {
        float mid = input;
        float side = input;
        
        float lfoMid = std::sin(lfoPhaseMid_ * 6.28318530718f);
        lfoPhaseMid_ += rate_ / sampleRate_;
        if (lfoPhaseMid_ >= 1.0f) lfoPhaseMid_ -= 1.0f;
        
        float lfoSide = std::sin(lfoPhaseSide_ * 6.28318530718f);
        lfoPhaseSide_ += rate_ / sampleRate_;
        if (lfoPhaseSide_ >= 1.0f) lfoPhaseSide_ -= 1.0f;
        
        float delayedMid = processBuffer(bufferMid_, lfoMid);
        mid = mid + delayedMid * 0.5f;
        
        float delayedSide = processBuffer(bufferSide_, lfoSide);
        side = side + delayedSide * 0.5f;
        
        outLeft = (mid + side) * 0.707f;
        outRight = (mid - side) * 0.707f;
    }
    
    void reset() {
        std::fill(bufferMid_.begin(), bufferMid_.end(), 0.0f);
        std::fill(bufferSide_.begin(), bufferSide_.end(), 0.0f);
        writePos_ = 0;
        lfoPhaseMid_ = 0.0f;
        lfoPhaseSide_ = 0.25f;
    }
    
private:
    float processBuffer(std::vector<float>& buffer, float lfoValue) {
        float delayTime = 0.02f + depth_ * 0.01f * lfoValue;
        float delaySamplesF = delayTime * sampleRate_;
        int delaySamples = static_cast<int>(delaySamplesF);
        float frac = delaySamplesF - delaySamples;
        
        if (delaySamples >= MAX_BUFFER_SIZE - 1) delaySamples = MAX_BUFFER_SIZE - 2;
        if (delaySamples < 1) delaySamples = 1;
        
        int readPos0 = writePos_ - delaySamples;
        if (readPos0 < 0) readPos0 += MAX_BUFFER_SIZE;
        int readPos1 = readPos0 - 1;
        if (readPos1 < 0) readPos1 += MAX_BUFFER_SIZE;
        
        float delayed = buffer[readPos0] * (1.0f - frac) + buffer[readPos1] * frac;
        
        buffer[writePos_] = delayed;
        writePos_ = (writePos_ + 1) % MAX_BUFFER_SIZE;
        
        return delayed;
    }
    
    std::vector<float> bufferMid_;
    std::vector<float> bufferSide_;
    int writePos_;
    float lfoPhaseMid_;
    float lfoPhaseSide_;
    float rate_;
    float depth_;
    float sampleRate_;
};
