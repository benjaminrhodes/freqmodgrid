#pragma once

#include "MidiSynth.h"
#include "../DSP/FMEngine.h"

using namespace iplug;

// Parameter IDs for the full 6-op FM synth
enum EParams
{
  // Per-operator params: ratio, level, feedback (x6 = 18 params)
  kParamOp1Ratio = 0,
  kParamOp1Level,
  kParamOp1Feedback,
  kParamOp2Ratio,
  kParamOp2Level,
  kParamOp2Feedback,
  kParamOp3Ratio,
  kParamOp3Level,
  kParamOp3Feedback,
  kParamOp4Ratio,
  kParamOp4Level,
  kParamOp4Feedback,
  kParamOp5Ratio,
  kParamOp5Level,
  kParamOp5Feedback,
  kParamOp6Ratio,
  kParamOp6Level,
  kParamOp6Feedback,
  // Algorithm
  kParamAlgorithm,
  // Filter
  kParamFilterType,
  kParamFilterCutoff,
  kParamFilterRes,
  // Envelope
  kParamAttack,
  kParamDecay,
  kParamSustain,
  kParamRelease,
  // LFOs
  kParamLFO1Rate,
  kParamLFO1Depth,
  kParamLFO2Rate,
  kParamLFO2Depth,
  // Effects
  kParamChorusRate,
  kParamChorusDepth,
  kParamDelayTime,
  kParamDelayFeedback,
  // Master
  kParamMasterVolume,
  kNumParams
};

// Thin wrapper that uses FMEngine for DSP and iPlug2's MidiSynth for MIDI routing.
// FMEngine handles its own voice allocation, so we bypass iPlug2's voice system
// and just forward MIDI events directly.
template<typename T>
class FreqmodGridDSP
{
public:
  FreqmodGridDSP(int /*nVoices*/)
  {
  }

  void ProcessBlock(T** inputs, T** outputs, int nOutputs, int nFrames,
                    double qnPos = 0., bool transportIsRunning = false, double tempo = 120.)
  {
    // Clear outputs
    for (int i = 0; i < nOutputs; i++)
      memset(outputs[i], 0, nFrames * sizeof(T));

    // Process any queued MIDI messages
    while (!mMidiQueue.Empty())
    {
      IMidiMsg msg = mMidiQueue.Peek();
      if (msg.mOffset > nFrames) break;
      mMidiQueue.Remove();

      if (msg.StatusMsg() == IMidiMsg::kNoteOn && msg.Velocity() > 0)
        mEngine.noteOn(msg.NoteNumber(), msg.Velocity() / 127.0f);
      else if (msg.StatusMsg() == IMidiMsg::kNoteOff ||
               (msg.StatusMsg() == IMidiMsg::kNoteOn && msg.Velocity() == 0))
        mEngine.noteOff(msg.NoteNumber());
    }

    // FMEngine processes into float buffers
    // We need temporary float buffers if T is double
    if constexpr (std::is_same_v<T, float>)
    {
      mEngine.process(outputs[0], (nOutputs > 1) ? outputs[1] : outputs[0], nFrames);
    }
    else
    {
      // For double precision, use intermediate float buffers
      mTempL.resize(nFrames);
      mTempR.resize(nFrames);
      mEngine.process(mTempL.data(), mTempR.data(), nFrames);
      for (int s = 0; s < nFrames; s++)
      {
        outputs[0][s] = static_cast<T>(mTempL[s]);
        if (nOutputs > 1)
          outputs[1][s] = static_cast<T>(mTempR[s]);
      }
    }
  }

  void Reset(double sampleRate, int blockSize)
  {
    mEngine.setSampleRate(static_cast<float>(sampleRate));
    mMidiQueue.Resize(blockSize);
  }

  void ProcessMidiMsg(const IMidiMsg& msg)
  {
    mMidiQueue.Add(msg);
  }

  void SetParam(int paramIdx, double value)
  {
    switch (paramIdx)
    {
      // Operator params: each group of 3 is ratio, level, feedback
      case kParamOp1Ratio:    mEngine.setOperatorRatio(0, (float)value); break;
      case kParamOp1Level:    mEngine.setOperatorLevel(0, (float)value / 100.0); break;
      case kParamOp1Feedback: mEngine.setOperatorFeedback(0, (float)value / 100.0); break;
      case kParamOp2Ratio:    mEngine.setOperatorRatio(1, (float)value); break;
      case kParamOp2Level:    mEngine.setOperatorLevel(1, (float)value / 100.0); break;
      case kParamOp2Feedback: mEngine.setOperatorFeedback(1, (float)value / 100.0); break;
      case kParamOp3Ratio:    mEngine.setOperatorRatio(2, (float)value); break;
      case kParamOp3Level:    mEngine.setOperatorLevel(2, (float)value / 100.0); break;
      case kParamOp3Feedback: mEngine.setOperatorFeedback(2, (float)value / 100.0); break;
      case kParamOp4Ratio:    mEngine.setOperatorRatio(3, (float)value); break;
      case kParamOp4Level:    mEngine.setOperatorLevel(3, (float)value / 100.0); break;
      case kParamOp4Feedback: mEngine.setOperatorFeedback(3, (float)value / 100.0); break;
      case kParamOp5Ratio:    mEngine.setOperatorRatio(4, (float)value); break;
      case kParamOp5Level:    mEngine.setOperatorLevel(4, (float)value / 100.0); break;
      case kParamOp5Feedback: mEngine.setOperatorFeedback(4, (float)value / 100.0); break;
      case kParamOp6Ratio:    mEngine.setOperatorRatio(5, (float)value); break;
      case kParamOp6Level:    mEngine.setOperatorLevel(5, (float)value / 100.0); break;
      case kParamOp6Feedback: mEngine.setOperatorFeedback(5, (float)value / 100.0); break;

      case kParamAlgorithm:   mEngine.setAlgorithm((int)value); break;

      case kParamFilterType:    mEngine.setFilterType((int)value); break;
      case kParamFilterCutoff:  mEngine.setFilterCutoff((float)value); break;
      case kParamFilterRes:     mEngine.setFilterResonance((float)value / 100.0); break;

      case kParamAttack:  mEngine.setAttack((float)value / 1000.0); break;  // ms -> seconds
      case kParamDecay:   mEngine.setDecay((float)value / 1000.0); break;
      case kParamSustain: mEngine.setSustain((float)value / 100.0); break;
      case kParamRelease: mEngine.setRelease((float)value / 1000.0); break;

      case kParamLFO1Rate:  mEngine.setLFORate(0, (float)value); break;
      case kParamLFO1Depth: mEngine.setLFODepth(0, (float)value / 100.0); break;
      case kParamLFO2Rate:  mEngine.setLFORate(1, (float)value); break;
      case kParamLFO2Depth: mEngine.setLFODepth(1, (float)value / 100.0); break;

      case kParamChorusRate:     mEngine.setChorusRate((float)value); break;
      case kParamChorusDepth:    mEngine.setChorusDepth((float)value / 100.0); break;
      case kParamDelayTime:      mEngine.setDelayTime((float)value / 1000.0); break;  // ms -> s
      case kParamDelayFeedback:  mEngine.setDelayFeedback((float)value / 100.0); break;

      case kParamMasterVolume: mEngine.setMasterVolume((float)value / 100.0); break;
      default: break;
    }
  }

public:
  FMEngine mEngine;
  IMidiQueue mMidiQueue;
  std::vector<float> mTempL, mTempR;
};
