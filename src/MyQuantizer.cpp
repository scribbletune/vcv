#include "plugin.hpp"
#include "quantize.hpp"

struct MyQuantizer : Module
{
	float incomingCv = 0.f;
	std::string debugValue = "Debug";
	float outgoingCv = 0.f;

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

	MyQuantizer()
	{
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS);
		configParam(RAGA_PARAM, 0.f, 5.f, 0.f, "");
		configParam(ROOT_NOTE_PARAM, 0.f, 11.f, 0.f, "");
	}

	void process(const ProcessArgs &args) override
	{
		float rootNote = params[ROOT_NOTE_PARAM].getValue();
		float raga = params[RAGA_PARAM].getValue();
		debugValue = getNoteName(int(rootNote)) + " " + getRagaName(int(raga));

		incomingCv = inputs[CV_INPUT].getVoltage();
		double absIncomingCv = abs(incomingCv);
		double oct = floor(absIncomingCv);
		double decimal = absIncomingCv - oct;

		// construct scale
		RagaObj r = getRagaByIdx(0);
		int *p;
		p = r.indices;
		// int len = *(&intervals + 1) - intervals;
		std::vector<double> scale;
		for (int i = 0; i < r.len; i++)
		{
			scale.push_back(*(p + i) / 12.0);
		}
		double quantizedCv = oct + getNoteValue(rootNote) + quantize(scale, decimal, 0, r.len - 1);
		outgoingCv = incomingCv < 0 ? -quantizedCv : quantizedCv;
		// debugValue = std::to_string(quantizedCv);

		outputs[CV_OUTPUT]
			.setVoltage(outgoingCv);
	}
};

struct CustomTextFieldWidget : LedDisplayTextField
{
	MyQuantizer *myQuantizerModule;

	CustomTextFieldWidget()
	{
		LedDisplayTextField();
	}
	void step() override
	{
		// this text field widget can access the module eg:

		if (myQuantizerModule)
		{ // dont leave out this check
			// myQuantizerModule->someModuleMethod();
			this->setText(myQuantizerModule->debugValue);
		}

		// call the inherited step method
		LedDisplayTextField::step();
	}
};

struct MyQuantizerWidget : ModuleWidget
{
	CustomTextFieldWidget *textField;
	MyQuantizerWidget(MyQuantizer *module)
	{
		setModule(module);
		setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/MyQuantizer.svg")));

		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		RoundBlackKnob *ragaKnob = createParamCentered<RoundBlackKnob>(mm2px(Vec(15, 21)), module, MyQuantizer::RAGA_PARAM);
		ragaKnob->snap = true;
		addParam(ragaKnob);

		RoundBlackKnob *rootNoteKnob = createParamCentered<RoundBlackKnob>(mm2px(Vec(15, 45)), module, MyQuantizer::ROOT_NOTE_PARAM);
		rootNoteKnob->snap = true;
		addParam(rootNoteKnob);

		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(15.24, 77.478)), module, MyQuantizer::CV_INPUT));

		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(15.24, 108.713)), module, MyQuantizer::CV_OUTPUT));

		textField = createWidget<CustomTextFieldWidget>(mm2px(Vec(0.0, 60.0)));
		textField->myQuantizerModule = module;
		textField->box.size = mm2px(Vec(40.0, 10.0));
		addChild(textField);
	}
};

Model *modelMyQuantizer = createModel<MyQuantizer, MyQuantizerWidget>("MyQuantizer");