#include "FreqmodGrid.h"
#include "IPlug_include_in_plug_src.h"

FreqmodGrid::FreqmodGrid(const InstanceInfo& info)
: Plugin(info, MakeConfig(kNumParams, kNumPresets))
{
  GetParam(kParamGain)->InitDouble("Gain", 100., 0., 100.0, 0.01, "%");
  GetParam(kParamAttack)->InitDouble("Attack", 10., 1., 1000., 0.1, "ms", IParam::kFlagsNone, "ADSR", IParam::ShapePowCurve(3.));
  GetParam(kParamDecay)->InitDouble("Decay", 10., 1., 1000., 0.1, "ms", IParam::kFlagsNone, "ADSR", IParam::ShapePowCurve(3.));
  GetParam(kParamSustain)->InitDouble("Sustain", 50., 0., 100., 1, "%", IParam::kFlagsNone, "ADSR");
  GetParam(kParamRelease)->InitDouble("Release", 10., 2., 1000., 0.1, "ms", IParam::kFlagsNone, "ADSR");
  GetParam(kParamOp1Ratio)->InitDouble("Op1 Ratio", 1.0, 0.25, 32.0, 0.01, "");
  GetParam(kParamOp1Level)->InitDouble("Op1 Level", 100., 0., 100., 1., "%");
  GetParam(kParamOp2Ratio)->InitDouble("Op2 Ratio", 2.0, 0.25, 32.0, 0.01, "");
  GetParam(kParamOp2Level)->InitDouble("Op2 Level", 50., 0., 100., 1., "%");
  GetParam(kParamAlgorithm)->InitEnum("Algorithm", 0, {"Serial", "Parallel", "Mixed"});
  GetParam(kParamFilterCutoff)->InitFrequency("Filter Cutoff", 12000., 20., 20000.);
  GetParam(kParamFilterRes)->InitDouble("Filter Resonance", 0., 0., 100., 1., "%");
  GetParam(kParamMasterVolume)->InitDouble("Master Volume", 70., 0., 100., 1., "%");
}

#if IPLUG_DSP
void FreqmodGrid::ProcessBlock(sample** inputs, sample** outputs, int nFrames)
{
  mDSP.ProcessBlock(inputs, outputs, NOutChansConnected(), nFrames);
}

void FreqmodGrid::ProcessMidiMsg(const IMidiMsg& msg)
{
  TRACE;
  mDSP.ProcessMidiMsg(msg);
}

void FreqmodGrid::OnReset()
{
  mDSP.Reset(GetSampleRate(), GetBlockSize());
}

void FreqmodGrid::OnParamChange(int paramIdx)
{
  mDSP.SetParam(paramIdx, GetParam(paramIdx)->Value());
}

void FreqmodGrid::OnIdle()
{
}
#endif
