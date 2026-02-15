// FreqmodGridEntry.cpp - VST3 Plugin Entry Point
#include "public.sdk/source/main/pluginfactory.h"
#include "FreqmodGridProcessor.h"
#include "plugids.h"

#define PLUGIN_NAME "FreqmodGrid"
#define PLUGIN_VENDOR "FreqMod"
#define PLUGIN_VERSION "1.0.0"

BEGIN_FACTORY_DEF(PLUGIN_VENDOR, "https://freqmod.com", "support@freqmod.com")

    // Main processor
    DEF_CLASS2(
        INLINE_UID_FROM_FUID(FreqmodGridProcessorUID),
        PClassInfo::kManyInstances,
        kVstAudioEffectClass,
        PLUGIN_NAME,
        Vst::kDistributable,
        "Synth",
        PLUGIN_VERSION,
        kVstVersionString,
        Steinberg::Vst::FreqmodGridProcessor::createInstance
    )

END_FACTORY
