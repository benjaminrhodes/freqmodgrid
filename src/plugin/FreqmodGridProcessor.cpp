// FreqmodGridProcessor.cpp - Minimal VST3 FM Synth Processor
#include "FreqmodGridProcessor.h"
#include "../../DSP/FMEngine.h"
#include <cstdio>
#include <cmath>
#include <cstring>

namespace Steinberg {
namespace Vst {

// Parameter definitions
struct ParamDef {
    int32 id;
    const char* name;
    const char* units;
    float min;
    float max;
    float default_;
    int32 steps;
};

static const ParamDef kParamDefs[] = {
    {0, "Op1 Ratio", "ratio", 0.25f, 32.0f, 1.0f, 0},
    {1, "Op1 Level", "%", 0.0f, 1.0f, 0.5f, 127},
    {2, "Op1 Feedback", "%", 0.0f, 1.0f, 0.0f, 127},
    {3, "Op2 Ratio", "ratio", 0.25f, 32.0f, 2.0f, 0},
    {4, "Op2 Level", "%", 0.0f, 1.0f, 0.5f, 127},
    {5, "Op2 Feedback", "%", 0.0f, 1.0f, 0.0f, 127},
    {6, "Op3 Ratio", "ratio", 0.25f, 32.0f, 3.0f, 0},
    {7, "Op3 Level", "%", 0.0f, 1.0f, 0.5f, 127},
    {8, "Op3 Feedback", "%", 0.0f, 1.0f, 0.0f, 127},
    {9, "Op4 Ratio", "ratio", 0.25f, 32.0f, 1.0f, 0},
    {10, "Op4 Level", "%", 0.0f, 1.0f, 0.0f, 127},
    {11, "Op4 Feedback", "%", 0.0f, 1.0f, 0.0f, 127},
    {12, "Op5 Ratio", "ratio", 0.25f, 32.0f, 0.5f, 0},
    {13, "Op5 Level", "%", 0.0f, 1.0f, 0.0f, 127},
    {14, "Op5 Feedback", "%", 0.0f, 1.0f, 0.0f, 127},
    {15, "Op6 Ratio", "ratio", 0.25f, 32.0f, 0.25f, 0},
    {16, "Op6 Level", "%", 0.0f, 1.0f, 0.0f, 127},
    {17, "Op6 Feedback", "%", 0.0f, 1.0f, 0.0f, 127},
    {18, "Algorithm", "", 1.0f, 8.0f, 1.0f, 7},
    {19, "Filter Type", "", 0.0f, 1.0f, 0.0f, 1},
    {20, "Filter Cutoff", "Hz", 20.0f, 20000.0f, 12000.0f, 0},
    {21, "Filter Resonance", "%", 0.0f, 1.0f, 0.0f, 127},
    {22, "Env Attack", "s", 0.001f, 5.0f, 0.01f, 0},
    {23, "Env Decay", "s", 0.001f, 5.0f, 0.1f, 0},
    {24, "Env Sustain", "%", 0.0f, 1.0f, 0.7f, 127},
    {25, "Env Release", "s", 0.01f, 10.0f, 0.3f, 0},
    {26, "LFO1 Rate", "Hz", 0.01f, 20.0f, 1.0f, 0},
    {27, "LFO1 Depth", "%", 0.0f, 1.0f, 0.0f, 127},
    {28, "LFO2 Rate", "Hz", 0.01f, 20.0f, 2.0f, 0},
    {29, "LFO2 Depth", "%", 0.0f, 1.0f, 0.0f, 127},
    {30, "Chorus Rate", "Hz", 0.1f, 10.0f, 1.0f, 0},
    {31, "Chorus Depth", "%", 0.0f, 1.0f, 0.3f, 127},
    {32, "Delay Time", "s", 0.001f, 2.0f, 0.25f, 0},
    {33, "Delay Feedback", "%", 0.0f, 0.9f, 0.3f, 127},
    {34, "Master Volume", "", 0.0f, 1.0f, 0.7f, 127},
    {35, "Polyphony", "voices", 1.0f, 16.0f, 8.0f, 15},
};

static constexpr int32 kNumParams = 36;

FreqmodGridProcessor::FreqmodGridProcessor() : sampleRate_(48000.0f), blockSize_(256) {
    for (int i = 0; i < kNumParams; i++) {
        params_[i] = (kParamDefs[i].default_ - kParamDefs[i].min) / (kParamDefs[i].max - kParamDefs[i].min);
        paramValues_[i] = kParamDefs[i].default_;
    }
}

FreqmodGridProcessor::~FreqmodGridProcessor() {
}

tresult PLUGIN_API FreqmodGridProcessor::initialize(FUnknown* context) {
    tresult result = SingleComponentEffect::initialize(context);
    if (result != kResultOk) return result;

    addEventInput(STR("MIDI"), 1);
    addAudioOutput(STR("Output"), SpeakerArr::kStereo);

    return kResultOk;
}

tresult PLUGIN_API FreqmodGridProcessor::terminate() {
    return kResultOk;
}

tresult PLUGIN_API FreqmodGridProcessor::setActive(TBool state) {
    if (state) engine_.setSampleRate(sampleRate_);
    return kResultOk;
}

tresult PLUGIN_API FreqmodGridProcessor::setupProcessing(ProcessSetup& setup) {
    sampleRate_ = setup.sampleRate;
    blockSize_ = setup.maxSamplesPerBlock;
    engine_.setSampleRate(sampleRate_);
    updateEngineParams();
    return kResultOk;
}

tresult PLUGIN_API FreqmodGridProcessor::canProcessSampleSize(int32 sampleSize) {
    return kResultOk;
}

tresult PLUGIN_API FreqmodGridProcessor::process(ProcessData& data) {
    // Handle parameter changes
    if (data.inputParameterChanges) {
        int32 count = 0;
        data.inputParameterChanges->getParameterChanges(count);
        for (int32 i = 0; i < count; i++) {
            IParamQueue* queue = data.inputParameterChanges->getParameterChanges(i);
            if (!queue) continue;
            
            int32 numChanges = queue->getParameterCount();
            for (int32 j = 0; j < numChanges; j++) {
                ParamValue value = 0;
                int32 offset = 0;
                if (queue->getParameterByIndex(j, value, offset) == kResultOk) {
                    ParamID id = queue->getParameterId();
                    if (id >= 0 && id < kNumParams) {
                        params_[id] = value;
                        paramValues_[id] = normalizedParamToPlain(id, value);
                        updateEngineParams();
                    }
                }
            }
        }
    }

    // Handle MIDI
    if (data.inputEvents) {
        processMIDI(data.inputEvents);
    }

    // Process audio
    if (data.numOutputs > 0 && data.outputs[0].numChannels > 0) {
        float** outputs = data.outputs[0].buffers;
        int32 sampleCount = data.numSamples;

        for (int32 i = 0; i < sampleCount; i++) {
            float outL = 0.0f, outR = 0.0f;
            engine_.process(&outL, &outR, 1);
            outputs[0][i] = outL;
            if (data.outputs[0].numChannels > 1) outputs[1][i] = outR;
        }
    }

    return kResultOk;
}

void FreqmodGridProcessor::processMIDI(IEventList* events) {
    int32 count = 0;
    events->getEventCount(count);
    for (int32 i = 0; i < count; i++) {
        Event event;
        if (events->getEvent(i, event) == kResultOk) {
            if (event.type == Event::kNoteOnEvent && event.noteOn.velocity > 0) {
                engine_.noteOn(event.noteOn.pitch, event.noteOn.velocity);
            } else if (event.type == Event::kNoteOffEvent) {
                engine_.noteOff(event.noteOff.pitch);
            }
        }
    }
}

void FreqmodGridProcessor::updateEngineParams() {
    engine_.setOperatorRatio(0, paramValues_[0]);
    engine_.setOperatorLevel(0, paramValues_[1]);
    engine_.setOperatorFeedback(0, paramValues_[2]);
    engine_.setOperatorRatio(1, paramValues_[3]);
    engine_.setOperatorLevel(1, paramValues_[4]);
    engine_.setOperatorFeedback(1, paramValues_[5]);
    engine_.setOperatorRatio(2, paramValues_[6]);
    engine_.setOperatorLevel(2, paramValues_[7]);
    engine_.setOperatorFeedback(2, paramValues_[8]);
    engine_.setOperatorRatio(3, paramValues_[9]);
    engine_.setOperatorLevel(3, paramValues_[10]);
    engine_.setOperatorFeedback(3, paramValues_[11]);
    engine_.setOperatorRatio(4, paramValues_[12]);
    engine_.setOperatorLevel(4, paramValues_[13]);
    engine_.setOperatorFeedback(4, paramValues_[14]);
    engine_.setOperatorRatio(5, paramValues_[15]);
    engine_.setOperatorLevel(5, paramValues_[16]);
    engine_.setOperatorFeedback(5, paramValues_[17]);
    engine_.setAlgorithm(static_cast<int>(paramValues_[18]) - 1);
    engine_.setFilterType(static_cast<int>(paramValues_[19]));
    engine_.setFilterCutoff(paramValues_[20]);
    engine_.setFilterResonance(paramValues_[21]);
    engine_.setAttack(paramValues_[22]);
    engine_.setDecay(paramValues_[23]);
    engine_.setSustain(paramValues_[24]);
    engine_.setRelease(paramValues_[25]);
    engine_.setLFORate(0, paramValues_[26]);
    engine_.setLFODepth(0, paramValues_[27]);
    engine_.setLFORate(1, paramValues_[28]);
    engine_.setLFODepth(1, paramValues_[29]);
    engine_.setChorusRate(paramValues_[30]);
    engine_.setChorusDepth(paramValues_[31]);
    engine_.setDelayTime(paramValues_[32]);
    engine_.setDelayFeedback(paramValues_[33]);
    engine_.setMasterVolume(paramValues_[34]);
}

tresult PLUGIN_API FreqmodGridProcessor::setState(IBStream* state) {
    for (int i = 0; i < kNumParams; i++) {
        state->read(&params_[i], sizeof(params_[i]));
        paramValues_[i] = normalizedParamToPlain(i, params_[i]);
    }
    updateEngineParams();
    return kResultOk;
}

tresult PLUGIN_API FreqmodGridProcessor::getState(IBStream* state) {
    for (int i = 0; i < kNumParams; i++) {
        state->write(&params_[i], sizeof(params_[i]));
    }
    return kResultOk;
}

int32 PLUGIN_API FreqmodGridProcessor::getParameterCount() {
    return kNumParams;
}

tresult PLUGIN_API FreqmodGridProcessor::getParameterInfo(int32 paramIndex, ParameterInfo& info) {
    if (paramIndex < 0 || paramIndex >= kNumParams) return kResultInvalidArgument;
    
    info.id = kParamDefs[paramIndex].id;
    info.unitId = kRootUnitId;
    info.flags = ParameterInfo::kCanAutomate;
    info.stepCount = kParamDefs[paramIndex].steps;
    info.defaultNormalizedValue = (kParamDefs[paramIndex].default_ - kParamDefs[paramIndex].min) / 
                                   (kParamDefs[paramIndex].max - kParamDefs[paramIndex].min);
    
    Steinberg::UString(info.title, 64).fromAscii(kParamDefs[paramIndex].name);
    Steinberg::UString(info.units, 64).fromAscii(kParamDefs[paramIndex].units);
    
    return kResultOk;
}

ParamValue PLUGIN_API FreqmodGridProcessor::normalizedParamToPlain(ParamID id, ParamValue value) {
    if (id < 0 || id >= kNumParams) return 0;
    return kParamDefs[id].min + value * (kParamDefs[id].max - kParamDefs[id].min);
}

ParamValue PLUGIN_API FreqmodGridProcessor::plainParamToNormalized(ParamID id, ParamValue plainValue) {
    if (id < 0 || id >= kNumParams) return 0;
    return (plainValue - kParamDefs[id].min) / (kParamDefs[id].max - kParamDefs[id].min);
}

tresult PLUGIN_API FreqmodGridProcessor::setParamNormalized(ParamID id, ParamValue value) {
    if (id < 0 || id >= kNumParams) return kResultInvalidArgument;
    params_[id] = value;
    paramValues_[id] = normalizedParamToPlain(id, value);
    updateEngineParams();
    return kResultOk;
}

tresult PLUGIN_API FreqmodGridProcessor::getParamValueByString(ParamID id, TChar* string, ParamValue& value) {
    return kResultFalse;  // Not implemented
}

tresult PLUGIN_API FreqmodGridProcessor::getParamStringByValue(ParamID id, TChar* string, ParamValue& valueNormalized) {
    if (id < 0 || id >= kNumParams) return kResultInvalidArgument;
    
    float value = paramValues_[id];
    char text[64] = {0};
    
    if (strcmp(kParamDefs[id].units, "ratio") == 0 || strcmp(kParamDefs[id].units, "Hz") == 0) {
        snprintf(text, sizeof(text), "%.2f", value);
    } else if (strcmp(kParamDefs[id].units, "s") == 0) {
        snprintf(text, sizeof(text), "%.3f", value);
    } else if (strcmp(kParamDefs[id].units, "%") == 0) {
        snprintf(text, sizeof(text), "%.0f", value * 100.0f);
    } else {
        snprintf(text, sizeof(text), "%.2f", value);
    }
    
    Steinberg::UString(string, 64).fromAscii(text);
    return kResultOk;
}

tresult PLUGIN_API FreqmodGridProcessor::getMidiControllerAssignment(int32 busIndex, int16 channel,
                                                   CtrlNumber midiControllerNumber, ParamID& id) {
    // Map mod wheel to LFO1 depth
    if (midiControllerNumber == kCtrlModWheel) {
        id = 27;  // LFO1 Depth
        return kResultOk;
    }
    return kResultFalse;
}

int32 PLUGIN_API FreqmodGridProcessor::getUnitCount() {
    return 1;
}

tresult PLUGIN_API FreqmodGridProcessor::getUnitInfo(int32 unitIndex, UnitInfo& info) {
    if (unitIndex == 0) {
        info.id = kRootUnitId;
        info.parentUnitId = -1;
        info.programListId = -1;
        Steinberg::UString(info.name, 64).fromAscii("FreqmodGrid");
        return kResultOk;
    }
    return kResultInvalidArgument;
}

} // namespace Vst
} // namespace Steinberg
