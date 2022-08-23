#include <iostream>
#include <vector>
#include "plugin.hpp"
#include "quantize.hpp"

struct RagaObj
{
  int len;
  int indices[18];
};

std::string vedaRagaNames[] = {"JHANKARADHWANI", "NATABHAIRAVI", "KEERAVANI", "KHARAHARAPRIYA", "GOURIMANOHARI", "VARUNAPRIYA"};
std::string getVedaRagaName(int idx)
{
  return vedaRagaNames[idx];
}

struct RagaObj Jhankaradhwani = {7, {0, 1, 3, 5, 7, 8, 9}};
struct RagaObj Natabhairavi = {7, {0, 1, 3, 5, 7, 8, 10}};
struct RagaObj Keeravani = {7, {0, 1, 3, 5, 7, 8, 11}};
struct RagaObj Kharaharapriya = {7, {0, 1, 3, 5, 7, 9, 10}};
struct RagaObj Gourimanohari = {7, {0, 1, 3, 5, 7, 9, 11}};
struct RagaObj Varunapriya = {7, {0, 1, 3, 5, 7, 10, 11}};

RagaObj getVedaRagaByIdx(int idx)
{
  switch (idx)
  {
  case 0:
    return Jhankaradhwani;
    break;

  case 1:
    return Natabhairavi;
    break;

  case 2:
    return Keeravani;
    break;

  case 3:
    return Kharaharapriya;
    break;

  case 4:
    return Gourimanohari;
    break;

  case 5:
    return Varunapriya;
    break;

  default:
    return Jhankaradhwani;
    break;
  }
}

struct VedaQuantizer : Module
{
  float rootNote = 0.f;
  float lastKnownRootNote = rootNote;
  float raga = 0.f;
  float lastKnownRaga = raga;
  float incomingCv = 0.f;
  float lastKnownIncomingCv = incomingCv;
  std::string currentRagaName = "JHANKARADHWANI";
  std::string currentRootNoteName = "C";
  float outgoingCv = incomingCv;
  int c = 0;

  enum ParamIds
  {
    RAGA_PARAM,
    ROOT_NOTE_PARAM,
    NUM_PARAMS // consider adding a param to select the scale/raga
  };
  enum InputIds
  {
    CV_INPUT,
    NUM_INPUTS
  };
  enum OutputIds
  {
    CV_OUTPUT,
    NUM_OUTPUTS
  };

  VedaQuantizer()
  {
    config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS);
    configParam(RAGA_PARAM, 0.f, 5.f, 0.f, "Raga"); // paramId, minValue, maxValue, defaultValue, label
    configParam(ROOT_NOTE_PARAM, 0.f, 11.f, 0.f, "Root");
  }

  void process(const ProcessArgs &args) override
  {
    rootNote = params[ROOT_NOTE_PARAM].getValue();
    raga = params[RAGA_PARAM].getValue();
    incomingCv = inputs[CV_INPUT].getVoltage();

    currentRagaName = getVedaRagaName(int(raga));
    currentRootNoteName = getNoteName(int(rootNote));

    // Quantize output only if it s new
    if (rootNote == lastKnownRootNote && raga == lastKnownRaga && incomingCv == lastKnownIncomingCv)
    {
      return;
    }

    lastKnownRootNote = rootNote;
    lastKnownRaga = raga;
    lastKnownIncomingCv = incomingCv;

    double absIncomingCv = abs(incomingCv);
    double oct = floor(absIncomingCv);
    double decimal = absIncomingCv - oct;

    RagaObj r = getVedaRagaByIdx(raga);
    int *p;
    p = r.indices;
    // int len = *(&intervals + 1) - intervals;
    // construct scale to use at run time
    std::vector<double> scale;
    for (int i = 0; i < r.len; i++)
    {
      scale.push_back(*(p + i) / 12.0);
    }
    double quantizedCv = oct + getNoteValue(rootNote) + quantize(scale, decimal, 0, r.len - 1);
    outgoingCv = incomingCv < 0 ? -quantizedCv : quantizedCv;
    c++;

    outputs[CV_OUTPUT].setVoltage(outgoingCv);
  }
};

struct RagaNameTextFieldWidget : LedDisplayTextField
{
  VedaQuantizer *module;

  RagaNameTextFieldWidget()
  {
    LedDisplayTextField();
  }
  void step() override
  {
    // this text field widget can access the module
    if (module) // dont leave out this check
    {
      this->setText(module->currentRagaName);
    }

    // call the inherited step method
    LedDisplayTextField::step();
  }
};

struct RootNoteNameTextFieldWidget : LedDisplayTextField
{
  VedaQuantizer *module;

  RootNoteNameTextFieldWidget()
  {
    LedDisplayTextField();
  }
  void step() override
  {
    // this text field widget can access the module
    if (module) // dont leave out this check
    {
      this->setText(module->currentRootNoteName);
    }

    // call the inherited step method
    LedDisplayTextField::step();
  }
};

struct VedaQuantizerWidget : ModuleWidget
{
  RagaNameTextFieldWidget *ragaNameText;
  RootNoteNameTextFieldWidget *rootNoteNameText;
  VedaQuantizerWidget(VedaQuantizer *module)
  {
    setModule(module);
    setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/Veda.svg")));

    addChild(createWidget<ScrewBlack>(Vec(RACK_GRID_WIDTH, 0)));
    addChild(createWidget<ScrewBlack>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
    addChild(createWidget<ScrewBlack>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
    addChild(createWidget<ScrewBlack>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

    RoundBlackKnob *ragaKnob = createParamCentered<RoundBlackKnob>(mm2px(Vec(10, 28)), module, VedaQuantizer::RAGA_PARAM);
    ragaKnob->snap = true;
    addParam(ragaKnob);

    RoundBlackKnob *rootNoteKnob = createParamCentered<RoundBlackKnob>(mm2px(Vec(10, 50)), module, VedaQuantizer::ROOT_NOTE_PARAM);
    rootNoteKnob->snap = true;
    addParam(rootNoteKnob);

    addInput(createInputCentered<PJ301MPort>(mm2px(Vec(10.24, 77.478)), module, VedaQuantizer::CV_INPUT));

    addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(10.24, 108.713)), module, VedaQuantizer::CV_OUTPUT));

    ragaNameText = createWidget<RagaNameTextFieldWidget>(mm2px(Vec(-3.0, 32.0))); // x, y
    ragaNameText->module = module;
    ragaNameText->box.size = mm2px(Vec(30.0, 10.0)); // w x h

    rootNoteNameText = createWidget<RootNoteNameTextFieldWidget>(mm2px(Vec(5.0, 55.0))); // x, y
    rootNoteNameText->module = module;
    rootNoteNameText->box.size = mm2px(Vec(10.0, 10.0)); // w x h

    addChild(ragaNameText);
    addChild(rootNoteNameText);
  }
};

Model *modelVedaQuantizer = createModel<VedaQuantizer, VedaQuantizerWidget>("Veda");