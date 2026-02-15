// FreqmodGridProcessor.h - Minimal VST3 FM Synth Processor
#pragma once

#include "public.sdk/source/vst/vstsinglecomponenteffect.h"
#include "plugids.h"
#include "../DSP/FMEngine.h"

namespace Steinberg {
namespace Vst {

class FreqmodGridProcessor : public SingleComponentEffect {
public:
    FreqmodGridProcessor();
    virtual ~FreqmodGridProcessor();

    static FUnknown* create(void*) { return (IAudioProcessor*)new FreqmodGridProcessor(); }

    // IComponent
    tresult PLUGIN_API initialize(FUnknown* context) override;
    tresult PLUGIN_API terminate() override;
    tresult PLUGIN_API setActive(TBool state) override;
    tresult PLUGIN_API process(ProcessData& data) override;
    tresult PLUGIN_API canProcessSampleSize(int32 sampleSize) override;
    tresult PLUGIN_API setState(IBStream* state) override;
    tresult PLUGIN_API getState(IBStream* state) override;
    tresult PLUGIN_API setupProcessing(ProcessSetup& setup) override;

    // Parameters
    int32 PLUGIN_API getParameterCount() override;
    tresult PLUGIN_API getParameterInfo(int32 paramIndex, ParameterInfo& info) override;
    tresult PLUGIN_API getParamValueByString(ParamID id, TChar* string, ParamValue& value) override;
    tresult PLUGIN_API getParamStringByValue(ParamID id, TChar* string, ParamValue& valueNormalized) override;
    ParamValue PLUGIN_API normalizedParamToPlain(ParamID id, ParamValue valueNormalized) override;
    ParamValue PLUGIN_API plainParamToNormalized(ParamID id, ParamValue plainValue) override;
    tresult PLUGIN_API setParamNormalized(ParamID id, ParamValue value) override;

    // MIDI
    tresult PLUGIN_API getMidiControllerAssignment(int32 busIndex, int16 channel,
                                                   CtrlNumber midiControllerNumber, ParamID& id) override;

    // Unit support
    int32 PLUGIN_API getUnitCount() override;
    tresult PLUGIN_API getUnitInfo(int32 unitIndex, UnitInfo& info) override;

    OBJ_METHODS(FreqmodGridProcessor, SingleComponentEffect)
    REFCOUNT_METHODS(SingleComponentEffect)

private:
    void processMIDI(IEventList* events);
    void updateEngineParams();

    FMEngine engine_;
    float sampleRate_;
    int32 blockSize_;
    float params_[38];  // kNumParams
    float paramValues_[38];
};

} // namespace Vst
} // namespace Steinberg
