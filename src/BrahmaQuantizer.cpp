#include "plugin.hpp"
#include "quantize.hpp"
#include "Brahma.hpp"

struct BrahmaQuantizer : Module
{
	float rootNote = 0.f;
	float lastKnownRootNote = rootNote;
	float raga = 0.f;
	float lastKnownRaga = raga;
	float incomingCv = 0.f;
	float lastKnownIncomingCv = incomingCv;
	std::string debugValue = "Debug";
	std::string currentRagaName = "DHAVALAMBARI";
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

	BrahmaQuantizer()
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

		debugValue = std::to_string(c) + " " + getNoteName(int(rootNote)) + " " + getRagaName(int(raga));
		currentRagaName = getRagaName(int(raga));
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

		RagaObj r = getRagaByIdx(raga);
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
	BrahmaQuantizer *brahmaModule;

	RagaNameTextFieldWidget()
	{
		LedDisplayTextField();
	}
	void step() override
	{
		// this text field widget can access the module
		if (brahmaModule) // dont leave out this check
		{
			this->setText(brahmaModule->currentRagaName);
		}

		// call the inherited step method
		LedDisplayTextField::step();
	}
};

struct RootNoteNameTextFieldWidget : LedDisplayTextField
{
	BrahmaQuantizer *brahmaModule;

	RootNoteNameTextFieldWidget()
	{
		LedDisplayTextField();
	}
	void step() override
	{
		// this text field widget can access the module
		if (brahmaModule) // dont leave out this check
		{
			this->setText(brahmaModule->currentRootNoteName);
		}

		// call the inherited step method
		LedDisplayTextField::step();
	}
};

struct BrahmaQuantizerWidget : ModuleWidget
{
	RagaNameTextFieldWidget *ragaNameText;
	RootNoteNameTextFieldWidget *rootNoteNameText;
	BrahmaQuantizerWidget(BrahmaQuantizer *module)
	{
		setModule(module);
		setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/Brahma.svg")));

		addChild(createWidget<ScrewBlack>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewBlack>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewBlack>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<ScrewBlack>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		RoundBlackKnob *ragaKnob = createParamCentered<RoundBlackKnob>(mm2px(Vec(10, 28)), module, BrahmaQuantizer::RAGA_PARAM);
		ragaKnob->snap = true;
		addParam(ragaKnob);

		RoundBlackKnob *rootNoteKnob = createParamCentered<RoundBlackKnob>(mm2px(Vec(10, 50)), module, BrahmaQuantizer::ROOT_NOTE_PARAM);
		rootNoteKnob->snap = true;
		addParam(rootNoteKnob);

		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(10.24, 77.478)), module, BrahmaQuantizer::CV_INPUT));

		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(10.24, 108.713)), module, BrahmaQuantizer::CV_OUTPUT));

		ragaNameText = createWidget<RagaNameTextFieldWidget>(mm2px(Vec(-3.0, 32.0))); // x, y
		ragaNameText->brahmaModule = module;
		ragaNameText->box.size = mm2px(Vec(30.0, 10.0)); // w x h

		rootNoteNameText = createWidget<RootNoteNameTextFieldWidget>(mm2px(Vec(5.0, 55.0))); // x, y
		rootNoteNameText->brahmaModule = module;
		rootNoteNameText->box.size = mm2px(Vec(10.0, 10.0)); // w x h

		addChild(ragaNameText);
		addChild(rootNoteNameText);
	}
};

Model *modelBrahmaQuantizer = createModel<BrahmaQuantizer, BrahmaQuantizerWidget>("Brahma");