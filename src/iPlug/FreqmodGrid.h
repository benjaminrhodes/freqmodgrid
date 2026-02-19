#pragma once

#include "IPlug_include_in_plug_hdr.h"

const int kNumPresets = 1;

#if IPLUG_DSP
#include "FreqmodGrid_DSP.h"
#include "PresetManager.h"
#endif

using namespace iplug;
using namespace igraphics;

class FreqmodGrid final : public Plugin
{
public:
  FreqmodGrid(const InstanceInfo& info);

#if IPLUG_DSP
public:
  void ProcessBlock(sample** inputs, sample** outputs, int nFrames) override;
  void ProcessMidiMsg(const IMidiMsg& msg) override;
  void OnReset() override;
  void OnParamChange(int paramIdx) override;

private:
  FreqmodGridDSP<sample> mDSP {16};
  PresetManager mPresetManager;
#endif
};
