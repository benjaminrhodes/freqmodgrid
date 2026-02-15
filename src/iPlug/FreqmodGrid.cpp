#include "FreqmodGrid.h"
#include "IPlug_include_in_plug_src.h"

FreqmodGrid::FreqmodGrid(const InstanceInfo& info)
: Plugin(info, MakeConfig(kNumParams, kNumPresets))
{
#if IPLUG_EDITOR
  mMakeEditorFunc = [this]() {
    return MakeEditor(this, PLUG_WIDTH, PLUG_HEIGHT);
  };
#endif
  // Operator 1
  GetParam(kParamOp1Ratio)->InitDouble("Op1 Ratio", 1.0, 0.25, 32.0, 0.01, "");
  GetParam(kParamOp1Level)->InitDouble("Op1 Level", 50., 0., 100., 1., "%");
  GetParam(kParamOp1Feedback)->InitDouble("Op1 Feedback", 0., 0., 100., 1., "%");
  // Operator 2
  GetParam(kParamOp2Ratio)->InitDouble("Op2 Ratio", 2.0, 0.25, 32.0, 0.01, "");
  GetParam(kParamOp2Level)->InitDouble("Op2 Level", 50., 0., 100., 1., "%");
  GetParam(kParamOp2Feedback)->InitDouble("Op2 Feedback", 0., 0., 100., 1., "%");
  // Operator 3
  GetParam(kParamOp3Ratio)->InitDouble("Op3 Ratio", 3.0, 0.25, 32.0, 0.01, "");
  GetParam(kParamOp3Level)->InitDouble("Op3 Level", 50., 0., 100., 1., "%");
  GetParam(kParamOp3Feedback)->InitDouble("Op3 Feedback", 0., 0., 100., 1., "%");
  // Operator 4
  GetParam(kParamOp4Ratio)->InitDouble("Op4 Ratio", 1.0, 0.25, 32.0, 0.01, "");
  GetParam(kParamOp4Level)->InitDouble("Op4 Level", 0., 0., 100., 1., "%");
  GetParam(kParamOp4Feedback)->InitDouble("Op4 Feedback", 0., 0., 100., 1., "%");
  // Operator 5
  GetParam(kParamOp5Ratio)->InitDouble("Op5 Ratio", 0.5, 0.25, 32.0, 0.01, "");
  GetParam(kParamOp5Level)->InitDouble("Op5 Level", 0., 0., 100., 1., "%");
  GetParam(kParamOp5Feedback)->InitDouble("Op5 Feedback", 0., 0., 100., 1., "%");
  // Operator 6
  GetParam(kParamOp6Ratio)->InitDouble("Op6 Ratio", 0.25, 0.25, 32.0, 0.01, "");
  GetParam(kParamOp6Level)->InitDouble("Op6 Level", 0., 0., 100., 1., "%");
  GetParam(kParamOp6Feedback)->InitDouble("Op6 Feedback", 0., 0., 100., 1., "%");

  // Algorithm (0-7 internally, displayed 1-8)
  GetParam(kParamAlgorithm)->InitEnum("Algorithm", 0, 8, "",
    IParam::kFlagsNone, "",
    "1: Serial", "2: (1+2)>Chain", "3: 1>All", "4: Pairs>Chain",
    "5: 3 Pairs", "6: Groups", "7: 2 Chains", "8: All Parallel");

  // Filter
  GetParam(kParamFilterType)->InitEnum("Filter Type", 0, 2, "",
    IParam::kFlagsNone, "", "Low Pass", "High Pass");
  GetParam(kParamFilterCutoff)->InitFrequency("Filter Cutoff", 12000., 20., 20000.);
  GetParam(kParamFilterRes)->InitDouble("Filter Resonance", 0., 0., 100., 1., "%");

  // Envelope (times in ms for UI, converted to seconds in SetParam)
  GetParam(kParamAttack)->InitDouble("Attack", 10., 1., 5000., 0.1, "ms",
    IParam::kFlagsNone, "ADSR", IParam::ShapePowCurve(3.));
  GetParam(kParamDecay)->InitDouble("Decay", 100., 1., 5000., 0.1, "ms",
    IParam::kFlagsNone, "ADSR", IParam::ShapePowCurve(3.));
  GetParam(kParamSustain)->InitDouble("Sustain", 70., 0., 100., 1., "%",
    IParam::kFlagsNone, "ADSR");
  GetParam(kParamRelease)->InitDouble("Release", 300., 10., 10000., 0.1, "ms",
    IParam::kFlagsNone, "ADSR", IParam::ShapePowCurve(3.));

  // LFOs
  GetParam(kParamLFO1Rate)->InitDouble("LFO1 Rate", 1.0, 0.01, 20.0, 0.01, "Hz");
  GetParam(kParamLFO1Depth)->InitDouble("LFO1 Depth", 0., 0., 100., 1., "%");
  GetParam(kParamLFO2Rate)->InitDouble("LFO2 Rate", 2.0, 0.01, 20.0, 0.01, "Hz");
  GetParam(kParamLFO2Depth)->InitDouble("LFO2 Depth", 0., 0., 100., 1., "%");

  // Effects
  GetParam(kParamChorusRate)->InitDouble("Chorus Rate", 1.0, 0.1, 10.0, 0.1, "Hz");
  GetParam(kParamChorusDepth)->InitDouble("Chorus Depth", 30., 0., 100., 1., "%");
  GetParam(kParamDelayTime)->InitDouble("Delay Time", 250., 1., 2000., 1., "ms");
  GetParam(kParamDelayFeedback)->InitDouble("Delay Feedback", 30., 0., 90., 1., "%");

  // Master
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

#if IPLUG_EDITOR
void FreqmodGrid::CreateEditor(IGraphics** pGraphics)
{
  mMakeGraphicsFunc = [this]() {
    return MakeGraphics(this, PLUG_WIDTH, PLUG_HEIGHT, PLUG_FPS, 1.);
  };
  
  *pGraphics = new FreqmodGridGUI(mEditor, this, PLUG_WIDTH, PLUG_HEIGHT);
}
#endif
#endif
