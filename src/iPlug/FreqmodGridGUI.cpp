#include "FreqmodGridGUI.h"

const IColor FreqmodGridGUI::sBackground = IColor(255, 26, 26, 38);       // #1a1a26
const IColor FreqmodGridGUI::sSectionBg = IColor(255, 35, 35, 52);        // #232334
const IColor FreqmodGridGUI::sAccent = IColor(255, 0, 212, 255);          // #00d4ff (cyan)
const IColor FreqmodGridGUI::sText = IColor(255, 200, 200, 210);         // #c8c8d2
const IColor FreqmodGridGUI::sKnobBg = IColor(255, 50, 50, 70);          // #323246
const IColor FreqmodGridGUI::sOperatorOn = IColor(255, 0, 212, 255);      // #00d4ff
const IColor FreqmodGridGUI::sOperatorOff = IColor(255, 60, 60, 80);     // #3c3c50
const IColor FreqmodGridGUI::sModulatorColor = IColor(255, 255, 140, 0);  // #ff8c00 (orange)
const IColor FreqmodGridGUI::sCarrierColor = IColor(255, 0, 200, 100);   // #00c864 (green)

FreqmodGridGUI::FreqmodGridGUI(void* pEditor, IPlug* pPlugin, int w, int h)
: IPlugGUI(pEditor, pPlugin, w, h)
{
}

void FreqmodGridGUI::OnAttached()
{
  auto pGraphics = mMakeGraphics(this);
  
  pGraphics->HandleMouseDown([](const IMouseMsg& msg) {});
  pGraphics->HandleDragDrop([this](const IMouseMsg& msg, const char* path) { OnDragDrop(msg.mX, msg.mY, path); });
  
  AttachBackground(pGraphics, IColor(255, 26, 26, 38));
  
  int y = 10;
  
  CreateAlgorithmSection(pGraphics, 10, y);
  
  y += 90;
  
  int opX = 10;
  for (int op = 0; op < 6; op++) {
    CreateOperatorSection(pGraphics, opX, y, op);
    opX += 145;
  }
  
  y += 140;
  
  CreateFilterSection(pGraphics, 10, y);
  CreateEnvelopeSection(pGraphics, 300, y);
  CreateLFOSection(pGraphics, 500, y, 0);
  CreateLFOSection(pGraphics, 650, y, 1);
  
  y += 130;
  
  CreateEffectsSection(pGraphics, 10, y);
  CreateMasterSection(pGraphics, 500, y);
  
  pGraphics->SetScaleConstraints(0.75, 2.0, 900, 600);
  
  AttachControl(pGraphics);
}

void FreqmodGridGUI::CreateAlgorithmSection(IGraphics* pGraphics, int x, int y)
{
  pGraphics->AttachControl(new ITextControl(pGraphics, IRECT(x, y, x + 120, y + 20), "ALGORITHM", IText(14, sText)));
  
  const char* algoNames[] = {"1", "2", "3", "4", "5", "6", "7", "8"};
  
  for (int i = 0; i < 8; i++) {
    int btnX = x + 15 + (i * 50);
    auto btn = new IVButtonControl(pGraphics, IRECT(btnX, y + 25, btnX + 40, y + 60),
      [this, i](IVButtonControl* pCaller) {
        GetPlugin()->GetParam(kParamAlgorithm)->Set(i);
        GetUI()->SetParameterFromUI(kParamAlgorithm, i);
      },
      "", btnNames[i]);
    btn->SetValueFn([this, i](IVButtonControl* pCaller) {
      return (GetPlugin()->GetParam(kParamAlgorithm)->Value() == i) ? 1.0 : 0.0;
    });
    pGraphics->AttachControl(btn);
  }
}

void FreqmodGridGUI::CreateOperatorSection(IGraphics* pGraphics, int x, int y, int opIndex)
{
  char opName[16];
  sprintf(opName, "OP %d", opIndex + 1);
  pGraphics->AttachControl(new ITextControl(pGraphics, IRECT(x, y, x + 130, y + 16), opName, IText(12, sAccent, EAlign::Center)));
  
  int ratioParam = kParamOp1Ratio + (opIndex * 3);
  int levelParam = kParamOp1Level + (opIndex * 3);
  int fbParam = kParamOp1Feedback + (opIndex * 3);
  
  int knobY = y + 20;
  auto ratioKnob = new IKnobMultiControl(pGraphics, IRECT(x + 30, knobY, x + 100, knobY + 50), ratioParam,
    IText(10, sText, EAlign::Center), "Ratio");
  pGraphics->AttachControl(ratioKnob);
  
  knobY += 55;
  auto levelKnob = new IKnobMultiControl(pGraphics, IRECT(x + 30, knobY, x + 100, knobY + 50), levelParam,
    IText(10, sText, EAlign::Center), "Level");
  pGraphics->AttachControl(levelKnob);
  
  knobY += 55;
  auto fbKnob = new IKnobMultiControl(pGraphics, IRECT(x + 30, knobY, x + 100, knobY + 50), fbParam,
    IText(10, sText, EAlign::Center), "Feedback");
  pGraphics->AttachControl(fbKnob);
}

void FreqmodGridGUI::CreateFilterSection(IGraphics* pGraphics, int x, int y)
{
  pGraphics->AttachControl(new ITextControl(pGraphics, IRECT(x, y, x + 150, y + 16), "FILTER", IText(12, sAccent)));
  
  auto typeSelect = new IVSwitchControl(pGraphics, IRECT(x + 10, y + 20, x + 80, y + 40), kParamFilterType,
    [this](IControl* pCaller) {}, "LP", "HP", "OFF");
  pGraphics->AttachControl(typeSelect);
  
  auto cutoffKnob = new IKnobMultiControl(pGraphics, IRECT(x + 30, y + 50, x + 80, y + 95), kParamFilterCutoff,
    IText(10, sText, EAlign::Center), "Cutoff");
  pGraphics->AttachControl(cutoffKnob);
  
  auto resKnob = new IKnobMultiControl(pGraphics, IRECT(x + 100, y + 50, x + 150, y + 95), kParamFilterRes,
    IText(10, sText, EAlign::Center), "Res");
  pGraphics->AttachControl(resKnob);
}

void FreqmodGridGUI::CreateEnvelopeSection(IGraphics* pGraphics, int x, int y)
{
  pGraphics->AttachControl(new ITextControl(pGraphics, IRECT(x, y, x + 200, y + 16), "ENVELOPE (ADSR)", IText(12, sAccent)));
  
  int knobX = x + 20;
  int knobY = y + 20;
  
  auto attackKnob = new IKnobMultiControl(pGraphics, IRECT(knobX, knobY, knobX + 40, knobY + 45), kParamAttack,
    IText(9, sText, EAlign::Center), "A");
  pGraphics->AttachControl(attackKnob);
  
  knobX += 45;
  auto decayKnob = new IKnobMultiControl(pGraphics, IRECT(knobX, knobY, knobX + 40, knobY + 45), kParamDecay,
    IText(9, sText, EAlign::Center), "D");
  pGraphics->AttachControl(decayKnob);
  
  knobX += 45;
  auto sustainKnob = new IKnobMultiControl(pGraphics, IRECT(knobX, knobY, knobX + 40, knobY + 45), kParamSustain,
    IText(9, sText, EAlign::Center), "S");
  pGraphics->AttachControl(sustainKnob);
  
  knobX += 45;
  auto releaseKnob = new IKnobMultiControl(pGraphics, IRECT(knobX, knobY, knobX + 40, knobY + 45), kParamRelease,
    IText(9, sText, EAlign::Center), "R");
  pGraphics->AttachControl(releaseKnob);
}

void FreqmodGridGUI::CreateLFOSection(IGraphics* pGraphics, int x, int y, int lfoIndex)
{
  char lfoName[16];
  sprintf(lfoName, "LFO %d", lfoIndex + 1);
  pGraphics->AttachControl(new ITextControl(pGraphics, IRECT(x, y, x + 120, y + 16), lfoName, IText(12, sAccent)));
  
  int rateParam = (lfoIndex == 0) ? kParamLFO1Rate : kParamLFO2Rate;
  int depthParam = (lfoIndex == 0) ? kParamLFO1Depth : kParamLFO2Depth;
  
  auto rateKnob = new IKnobMultiControl(pGraphics, IRECT(x + 20, y + 20, x + 60, y + 60), rateParam,
    IText(10, sText, EAlign::Center), "Rate");
  pGraphics->AttachControl(rateKnob);
  
  auto depthKnob = new IKnobMultiControl(pGraphics, IRECT(x + 70, y + 20, x + 110, y + 60), depthParam,
    IText(10, sText, EAlign::Center), "Depth");
  pGraphics->AttachControl(depthKnob);
}

void FreqmodGridGUI::CreateEffectsSection(IGraphics* pGraphics, int x, int y)
{
  pGraphics->AttachControl(new ITextControl(pGraphics, IRECT(x, y, x + 280, y + 16), "EFFECTS", IText(12, sAccent)));
  
  int knobX = x + 20;
  
  auto chorusRateKnob = new IKnobMultiControl(pGraphics, IRECT(knobX, y + 20, knobX + 45, y + 60), kParamChorusRate,
    IText(9, sText, EAlign::Center), "Ch Rate");
  pGraphics->AttachControl(chorusRateKnob);
  
  knobX += 50;
  auto chorusDepthKnob = new IKnobMultiControl(pGraphics, IRECT(knobX, y + 20, knobX + 45, y + 60), kParamChorusDepth,
    IText(9, sText, EAlign::Center), "Ch Depth");
  pGraphics->AttachControl(chorusDepthKnob);
  
  knobX += 50;
  auto delayTimeKnob = new IKnobMultiControl(pGraphics, IRECT(knobX, y + 20, knobX + 45, y + 60), kParamDelayTime,
    IText(9, sText, EAlign::Center), "Del Time");
  pGraphics->AttachControl(delayTimeKnob);
  
  knobX += 50;
  auto delayFbKnob = new IKnobMultiControl(pGraphics, IRECT(knobX, y + 20, knobX + 45, y + 60), kParamDelayFeedback,
    IText(9, sText, EAlign::Center), "Del Fb");
  pGraphics->AttachControl(delayFbKnob);
}

void FreqmodGridGUI::CreateMasterSection(IGraphics* pGraphics, int x, int y)
{
  pGraphics->AttachControl(new ITextControl(pGraphics, IRECT(x, y, x + 150, y + 16), "MASTER", IText(12, sAccent)));
  
  auto volKnob = new IKnobMultiControl(pGraphics, IRECT(x + 40, y + 20, x + 110, y + 80), kParamMasterVolume,
    IText(11, sText, EAlign::Center), "Volume");
  pGraphics->AttachControl(volKnob);
  
  auto randomizeBtn = new IVButtonControl(pGraphics, IRECT(x + 30, y + 90, x + 120, y + 115),
    [this](IVButtonControl* pCaller) {
      for (int i = 0; i < kNumParams; i++) {
        auto param = GetPlugin()->GetParam(i);
        if (param) {
          float val = param->GetMin() + (float)rand() / RAND_MAX * (param->GetMax() - param->GetMin());
          GetUI()->SetParameterFromUI(i, val);
        }
      }
    }, "", "Randomize");
  pGraphics->AttachControl(randomizeBtn);
}

void FreqmodGridGUI::Draw(IGraphics* pGraphics)
{
  pGraphics->FillRect(sBackground, mRECT);
}

void FreqmodGridGUI::OnDragDrop(int x, int y, const char* path)
{
}
