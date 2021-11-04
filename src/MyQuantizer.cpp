#include "plugin.hpp"
#include "quantize.hpp"

int intervals[] = {0, 3, 4, 6, 7, 10, 11, 12};

struct MyQuantizer : Module
{
	float incomingCv = 0.f;
	std::string debugValue = "Debug";
	float outgoingCv = 0.f;

	enum ParamIds
	{
		PITCH_PARAM,
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
		configParam(PITCH_PARAM, 0.f, 11.f, 0.f, "");
	}

	void process(const ProcessArgs &args) override
	{
		float pitch = params[PITCH_PARAM].getValue();
		debugValue = getNoteName(int(pitch));

		incomingCv = inputs[CV_INPUT].getVoltage();
		double absIncomingCv = abs(incomingCv);
		double oct = floor(absIncomingCv);
		double decimal = absIncomingCv - oct;

		// construct scale
		std::vector<double> scale;
		for (int i = 0; i < 7; i++)
		{
			scale.push_back(intervals[i] / 12.0);
		}
		double quantizedCv = oct + quantize(scale, decimal);
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

		RoundBlackKnob *rootNoteKnob = createParamCentered<RoundBlackKnob>(mm2px(Vec(15, 45)), module, MyQuantizer::PITCH_PARAM);
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