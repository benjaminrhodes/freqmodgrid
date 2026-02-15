#pragma once

#include "IPlug/IPlugGUI.h"
#include "FreqmodGrid_DSP.h"
#include <functional>

using namespace iplug;
using namespace igraphics;

class FreqmodGridGUI : public IPlugGUI
{
public:
  FreqmodGridGUI(void* pEditor, IPlug* pPlugin, int w, int h);
  
  void Draw(IGraphics* pGraphics) override;
  void OnAttached() override;
  void OnDragDrop(int x, int y, const char* path) override;
  
private:
  void CreateAlgorithmSection(IGraphics* pGraphics, int x, int y);
  void CreateOperatorSection(IGraphics* pGraphics, int x, int y, int opIndex);
  void CreateFilterSection(IGraphics* pGraphics, int x, int y);
  void CreateEnvelopeSection(IGraphics* pGraphics, int x, int y);
  void CreateLFOSection(IGraphics* pGraphics, int x, int y, int lfoIndex);
  void CreateEffectsSection(IGraphics* pGraphics, int x, int y);
  void CreateMasterSection(IGraphics* pGraphics, int x, int y);
  
  void DrawAlgorithmVisualization(IGraphics* pGraphics, int x, int y, int algo);
  
  static const IColor sBackground;
  static const IColor sSectionBg;
  static const IColor sAccent;
  static const IColor sText;
  static const IColor sKnobBg;
  static const IColor sOperatorOn;
  static const IColor sOperatorOff;
  static const IColor sModulatorColor;
  static const IColor sCarrierColor;
};
