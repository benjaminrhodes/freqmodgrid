#include "FreqmodGrid.h"
#include "IPlug_include_in_plug_src.h"
#include "IControls.h"

FreqmodGrid::FreqmodGrid(const InstanceInfo& info)
: Plugin(info, MakeConfig(kNumParams, kNumPresets))
{
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
    "1", "2", "3", "4", "5", "6", "7", "8");

  // Filter
  GetParam(kParamFilterType)->InitEnum("Filter Type", 0, 2, "",
    IParam::kFlagsNone, "", "LP", "HP");
  GetParam(kParamFilterCutoff)->InitFrequency("Filter Cutoff", 12000., 20., 20000.);
  GetParam(kParamFilterRes)->InitDouble("Filter Resonance", 0., 0., 100., 1., "%");

  // Envelope
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
  GetParam(kParamOversample)->InitEnum("Oversample", 0, 3, "", IParam::kFlagsNone, "", "Off,2x,4x");
  
  // Initialize preset manager
  mPresetManager.loadFactoryPresets("resources/presets/factory_presets.json");
  mPresetManager.loadUserPresets();

#if IPLUG_EDITOR
  mMakeGraphicsFunc = [&]() {
    return MakeGraphics(*this, PLUG_WIDTH, PLUG_HEIGHT, PLUG_FPS, 1.0);
  };
  
  mLayoutFunc = [&](IGraphics* pGraphics) {
    pGraphics->AttachPanelBackground(IColor(255, 26, 26, 38));
    
    // Title
    auto title = new ITextControl(IRECT(10, 5, 200, 25), "FREQMOD GRID", 
      IText(14, EAlign::Center, IColor(255, 0, 212, 255)));
    pGraphics->AttachControl(title);
    
    const IVStyle style = DEFAULT_STYLE;
    
    int y = 35;
    
    // Algorithm buttons
    auto algoLabel = new ITextControl(IRECT(10, y, 100, y + 15), "ALGORITHM", 
      IText(10, IColor(255, 200, 200, 210)));
    pGraphics->AttachControl(algoLabel);
    y += 18;
    
    for (int i = 0; i < 8; i++) {
      int x = 10 + i * 42;
      auto btn = new IVButtonControl(IRECT(x, y, x + 38, y + 25),
        SplashClickActionFunc, std::to_string(i + 1).c_str(), style);
      btn->SetParamIdx(i);
      pGraphics->AttachControl(btn);
    }
    
    y += 35;
    
    // Operators
    int opX = 10;
    const char* opLabels[] = {"Op1", "Op2", "Op3", "Op4", "Op5", "Op6"};
    for (int op = 0; op < 6; op++) {
      auto opLabel = new ITextControl(IRECT(opX, y, opX + 70, y + 15), opLabels[op], 
        IText(10, EAlign::Center, IColor(255, 0, 212, 255)));
      pGraphics->AttachControl(opLabel);
      
      int ratioParam = kParamOp1Ratio + (op * 3);
      int levelParam = kParamOp1Level + (op * 3);
      int feedbackParam = kParamOp1Feedback + (op * 3);
      
      char ratioLabel[16];
      sprintf(ratioLabel, "Op%d Ratio", op + 1);
      char levelLabel[16];
      sprintf(levelLabel, "Op%d Level", op + 1);
      char feedbackLabel[16];
      sprintf(feedbackLabel, "Op%d Fdbk", op + 1);
      
      pGraphics->AttachControl(new IVKnobControl(IRECT(opX + 5, y + 18, opX + 35, y + 48), ratioParam, ratioLabel, style));
      pGraphics->AttachControl(new IVKnobControl(IRECT(opX + 40, y + 18, opX + 70, y + 48), levelParam, levelLabel, style));
      pGraphics->AttachControl(new IVKnobControl(IRECT(opX + 75, y + 18, opX + 105, y + 48), feedbackParam, feedbackLabel, style));
      
      opX += 110;
    }
    
    y += 60;
    
    // Filter
    auto filterLabel = new ITextControl(IRECT(10, y, 80, y + 15), "FILTER", 
      IText(10, IColor(255, 0, 212, 255)));
    pGraphics->AttachControl(filterLabel);
    pGraphics->AttachControl(new IVSwitchControl(IRECT(10, y + 18, 60, y + 38), kParamFilterType, "", style));
    pGraphics->AttachControl(new IVKnobControl(IRECT(70, y + 18, 120, y + 58), kParamFilterCutoff, "Cutoff", style));
    pGraphics->AttachControl(new IVKnobControl(IRECT(125, y + 18, 175, y + 58), kParamFilterRes, "Res", style));
    
    // Envelope
    auto envLabel = new ITextControl(IRECT(200, y, 280, y + 15), "ENVELOPE", 
      IText(10, IColor(255, 0, 212, 255)));
    pGraphics->AttachControl(envLabel);
    pGraphics->AttachControl(new IVKnobControl(IRECT(200, y + 18, 235, y + 50), kParamAttack, "Attack", style));
    pGraphics->AttachControl(new IVKnobControl(IRECT(240, y + 18, 275, y + 50), kParamDecay, "Decay", style));
    pGraphics->AttachControl(new IVKnobControl(IRECT(280, y + 18, 315, y + 50), kParamSustain, "Sustain", style));
    pGraphics->AttachControl(new IVKnobControl(IRECT(320, y + 18, 355, y + 50), kParamRelease, "Release", style));
    
    // LFO 1
    auto lfo1Label = new ITextControl(IRECT(380, y, 440, y + 15), "LFO 1", 
      IText(10, IColor(255, 0, 212, 255)));
    pGraphics->AttachControl(lfo1Label);
    pGraphics->AttachControl(new IVKnobControl(IRECT(380, y + 18, 420, y + 50), kParamLFO1Rate, "LFO1 Rate", style));
    pGraphics->AttachControl(new IVKnobControl(IRECT(425, y + 18, 465, y + 50), kParamLFO1Depth, "LFO1 Depth", style));
    
    // LFO 2
    auto lfo2Label = new ITextControl(IRECT(490, y, 550, y + 15), "LFO 2", 
      IText(10, IColor(255, 0, 212, 255)));
    pGraphics->AttachControl(lfo2Label);
    pGraphics->AttachControl(new IVKnobControl(IRECT(490, y + 18, 530, y + 50), kParamLFO2Rate, "LFO2 Rate", style));
    pGraphics->AttachControl(new IVKnobControl(IRECT(535, y + 18, 575, y + 50), kParamLFO2Depth, "LFO2 Depth", style));
    
    y += 65;
    
    // Effects
    auto fxLabel = new ITextControl(IRECT(10, y, 100, y + 15), "EFFECTS", 
      IText(10, IColor(255, 0, 212, 255)));
    pGraphics->AttachControl(fxLabel);
    pGraphics->AttachControl(new IVKnobControl(IRECT(10, y + 18, 55, y + 55), kParamChorusRate, "Ch Rate", style));
    pGraphics->AttachControl(new IVKnobControl(IRECT(60, y + 18, 105, y + 55), kParamChorusDepth, "Ch Depth", style));
    pGraphics->AttachControl(new IVKnobControl(IRECT(110, y + 18, 155, y + 55), kParamDelayTime, "Delay Time", style));
    pGraphics->AttachControl(new IVKnobControl(IRECT(160, y + 18, 205, y + 55), kParamDelayFeedback, "Delay Fdbk", style));
    
    // Master
    auto masterLabel = new ITextControl(IRECT(250, y, 320, y + 15), "MASTER", 
      IText(10, IColor(255, 0, 212, 255)));
    pGraphics->AttachControl(masterLabel);
    pGraphics->AttachControl(new IVKnobControl(IRECT(250, y + 18, 320, y + 65), kParamMasterVolume, "Vol", style));
    
    // Oversampling toggle buttons (Off/2x/4x)
    auto osLabel = new ITextControl(IRECT(340, y, 400, y + 15), "OS", 
      IText(10, IColor(255, 0, 212, 255)));
    pGraphics->AttachControl(osLabel);
    for (int i = 0; i < 3; i++) {
      int x = 340 + i * 35;
      auto btn = new IVButtonControl(IRECT(x, y + 18, x + 32, y + 42),
        SplashClickActionFunc, (i == 0) ? "Off" : (i == 1) ? "2x" : "4x", style);
      btn->SetParamIdx(kParamOversample);
      btn->SetValue(i);
      pGraphics->AttachControl(btn);
    }
    
    // Randomize button
    pGraphics->AttachControl(new IVButtonControl(IRECT(340, y + 25, 430, y + 50),
      SplashClickActionFunc, "RND", style));
    
    pGraphics->SetScaleConstraints(0.75, 2.0);
  };
#endif
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
#endif
