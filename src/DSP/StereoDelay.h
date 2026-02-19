#pragma once

#include "Constants.h"
#include <vector>
#include <cmath>

class StereoDelay {
public:
    static constexpr int MAX_BUFFER_SIZE = 192000;
    
    StereoDelay() : time_(0.25f), feedback_(0.3f), crossFeedback_(0.1f),
                    sampleRate_(48000.0f), writePos_(0) {
        bufferMid_.resize(MAX_BUFFER_SIZE, 0.0f);
        bufferSide_.resize(MAX_BUFFER_SIZE, 0.0f);
    }
    
    void setTime(float time) { time_ = clampf(time, 0.001f, 2.0f); }
    void setFeedback(float fb) { feedback_ = clampf(fb, 0.0f, 0.9f); }
    void setCrossFeedback(float cross) { crossFeedback_ = clampf(cross, 0.0f, 0.3f); }
    void setSampleRate(float sr) { sampleRate_ = sr; }
    
    void process(float input, float& outLeft, float& outRight) {
        int delaySamples = static_cast<int>(time_ * sampleRate_);
        if (delaySamples >= MAX_BUFFER_SIZE) delaySamples = MAX_BUFFER_SIZE - 1;
        if (delaySamples < 1) delaySamples = 1;
        
        float mid = input;
        float side = input;
        
        int readPosMid = writePos_ - delaySamples;
        if (readPosMid < 0) readPosMid += MAX_BUFFER_SIZE;
        
        float delayedMid = bufferMid_[readPosMid];
        float delayedSide = bufferSide_[readPosMid];
        
        bufferMid_[writePos_] = mid + delayedMid * feedback_ + delayedSide * crossFeedback_;
        bufferSide_[writePos_] = side + delayedSide * feedback_ + delayedMid * crossFeedback_;
        
        writePos_ = (writePos_ + 1) % MAX_BUFFER_SIZE;
        
        outLeft = (mid + delayedMid + side + delayedSide) * 0.5f;
        outRight = (mid + delayedMid - side - delayedSide) * 0.5f;
    }
    
    void reset() {
        std::fill(bufferMid_.begin(), bufferMid_.end(), 0.0f);
        std::fill(bufferSide_.begin(), bufferSide_.end(), 0.0f);
        writePos_ = 0;
    }
    
private:
    std::vector<float> bufferMid_;
    std::vector<float> bufferSide_;
    int writePos_;
    float time_;
    float feedback_;
    float crossFeedback_;
    float sampleRate_;
};
