#ifndef PLUGIDS_H
#define PLUGIDS_H

#include <cstdint>

namespace FrequadMod {

static const int32_t kNumParams = 35;
static const int32_t kNumInputs = 2;
static const int32_t kNumOutputs = 2;
static const int32_t kNumPrograms = 128;

static const uint8_t PluginUID[16] = {
    0x5A, 0x7D, 0x3F, 0x2E, 0x8C, 0x4B, 0x9A, 0x12,
    0x6D, 0x5E, 0x4F, 0x3A, 0x1B, 0x8C, 0x7D, 0x9E
};

static const uint8_t ControllerUID[16] = {
    0x5A, 0x7D, 0x3F, 0x2E, 0x8C, 0x4B, 0x9A, 0x12,
    0x6D, 0x5E, 0x4F, 0x3A, 0x1B, 0x8C, 0x7D, 0x9F
};

enum ParamIDs {
    kMasterVolume = 0,
    kOp1Ratio = 1,
    kOp1Level = 2,
    kOp1Feedback = 3,
    kOp2Ratio = 4,
    kOp2Level = 5,
    kOp2Feedback = 6,
    kOp3Ratio = 7,
    kOp3Level = 8,
    kOp3Feedback = 9,
    kOp4Ratio = 10,
    kOp4Level = 11,
    kOp4Feedback = 12,
    kOp5Ratio = 13,
    kOp5Level = 14,
    kOp5Feedback = 15,
    kOp6Ratio = 16,
    kOp6Level = 17,
    kOp6Feedback = 18,
    kAlgorithm = 19,
    kFilterType = 20,
    kFilterCutoff = 21,
    kFilterResonance = 22,
    kEnvAttack = 23,
    kEnvDecay = 24,
    kEnvSustain = 25,
    kEnvRelease = 26,
    kLFO1Rate = 27,
    kLFO1Depth = 28,
    kLFO2Rate = 29,
    kLFO2Depth = 30,
    kChorusRate = 31,
    kChorusDepth = 32,
    kDelayTime = 33,
    kDelayFeedback = 34
};

} // namespace FrequadMod

#endif // PLUGIDS_H
