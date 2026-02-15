#pragma once

#include "MidiSynth.h"
#include "ADSREnvelope.h"
#include "Oscillator.h"
#include "Smoothers.h"

using namespace iplug;

enum EParams
{
  kParamGain = 0,
  kParamAttack,
  kParamDecay,
  kParamSustain,
  kParamRelease,
  kParamOp1Ratio,
  kParamOp1Level,
  kParamOp2Ratio,
  kParamOp2Level,
  kParamAlgorithm,
  kParamFilterCutoff,
  kParamFilterRes,
  kParamMasterVolume,
  kNumParams
};

template<typename T>
class FreqmodGridDSP
{
public:
#pragma mark - Voice
  class Voice : public SynthVoice
  {
  public:
    Voice()
    : mAMPEnv("amp", [&](){ mOSC.Reset(); })
    {
    }

    bool GetBusy() const override
    {
      return mAMPEnv.GetBusy();
    }

    void Trigger(double level, bool isRetrigger) override
    {
      mOSC.Reset();
      if (isRetrigger)
        mAMPEnv.Retrigger(level);
      else
        mAMPEnv.Start(level);
    }

    void Release() override
    {
      mAMPEnv.Release();
    }

    void ProcessSamplesAccumulating(T** inputs, T** outputs, int nInputs, int nOutputs, int startIdx, int nFrames) override
    {
      double pitch = mInputs[kVoiceControlPitch].endValue;
      double pitchBend = mInputs[kVoiceControlPitchBend].endValue;

      // Convert pitch to frequency (1V/oct)
      double carrierFreq = 440. * pow(2., pitch + pitchBend);
      double modulatorFreq = carrierFreq * mOp2Ratio;

      for (auto i = startIdx; i < startIdx + nFrames; i++)
      {
        // Simple 2-op FM: modulator -> carrier
        double modOut = mModOSC.Process(modulatorFreq) * mOp2Level;
        double carrierPhase = carrierFreq + (modOut * carrierFreq * 4.0); // FM index
        double sample = mOSC.Process(carrierPhase) * mAMPEnv.Process(inputs[1][i]) * mGain;

        outputs[0][i] += sample;
        outputs[1][i] += sample;
      }
    }

    void SetSampleRateAndBlockSize(double sampleRate, int blockSize) override
    {
      mOSC.SetSampleRate(sampleRate);
      mModOSC.SetSampleRate(sampleRate);
      mAMPEnv.SetSampleRate(sampleRate);
    }

    void SetProgramNumber(int pgm) override {}
    void SetControl(int controlNumber, float value) override {}

  public:
    FastSinOscillator<T> mOSC;
    FastSinOscillator<T> mModOSC;
    ADSREnvelope<T> mAMPEnv;
    double mOp2Ratio = 2.0;
    double mOp2Level = 0.5;
  };

public:
#pragma mark -
  FreqmodGridDSP(int nVoices)
  {
    for (auto i = 0; i < nVoices; i++)
    {
      mSynth.AddVoice(new Voice(), 0);
    }
  }

  void ProcessBlock(T** inputs, T** outputs, int nOutputs, int nFrames, double qnPos = 0., bool transportIsRunning = false, double tempo = 120.)
  {
    // Clear outputs
    for (auto i = 0; i < nOutputs; i++)
      memset(outputs[i], 0, nFrames * sizeof(T));

    mParamSmoother.ProcessBlock(mParamsToSmooth, mModulations.GetList(), nFrames);
    mSynth.ProcessBlock(mModulations.GetList(), outputs, 0, nOutputs, nFrames);

    for (int s = 0; s < nFrames; s++)
    {
      T gain = mModulations.GetList()[0][s];
      outputs[0][s] *= gain;
      outputs[1][s] *= gain;
    }
  }

  void Reset(double sampleRate, int blockSize)
  {
    mSynth.SetSampleRateAndBlockSize(sampleRate, blockSize);
    mSynth.Reset();
    mModulationsData.Resize(blockSize * 2);
    mModulations.Empty();
    for (int i = 0; i < 2; i++)
      mModulations.Add(mModulationsData.Get() + (blockSize * i));
  }

  void ProcessMidiMsg(const IMidiMsg& msg)
  {
    mSynth.AddMidiMsgToQueue(msg);
  }

  void SetParam(int paramIdx, double value)
  {
    using EEnvStage = ADSREnvelope<sample>::EStage;

    switch (paramIdx)
    {
      case kParamGain:
      case kParamMasterVolume:
        mParamsToSmooth[0] = (T)value / 100.;
        break;
      case kParamSustain:
        mParamsToSmooth[1] = (T)value / 100.;
        break;
      case kParamAttack:
      case kParamDecay:
      case kParamRelease:
      {
        EEnvStage stage = static_cast<EEnvStage>(EEnvStage::kAttack + (paramIdx - kParamAttack));
        mSynth.ForEachVoice([stage, value](SynthVoice& voice) {
          dynamic_cast<FreqmodGridDSP::Voice&>(voice).mAMPEnv.SetStageTime(stage, value);
        });
        break;
      }
      case kParamOp2Ratio:
        mSynth.ForEachVoice([value](SynthVoice& voice) {
          dynamic_cast<FreqmodGridDSP::Voice&>(voice).mOp2Ratio = value;
        });
        break;
      case kParamOp2Level:
        mSynth.ForEachVoice([value](SynthVoice& voice) {
          dynamic_cast<FreqmodGridDSP::Voice&>(voice).mOp2Level = value / 100.;
        });
        break;
      default:
        break;
    }
  }

public:
  MidiSynth mSynth { VoiceAllocator::kPolyModePoly, MidiSynth::kDefaultBlockSize };
  WDL_TypedBuf<T> mModulationsData;
  WDL_PtrList<T> mModulations;
  LogParamSmooth<T, 2> mParamSmoother;
  sample mParamsToSmooth[2] = {1., 1.};
};
