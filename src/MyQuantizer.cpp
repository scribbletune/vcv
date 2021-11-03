#include "plugin.hpp"
#include "quantize.hpp"

int intervals[] = {0, 1, 3, 6, 7, 8, 9, 12};

struct MyQuantizer : Module
{
	float incomingCv = 0.f;
	std::string debugValue;
	float outgoingCv = 0.f;

	enum ParamIds
	{
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
	}

	void process(const ProcessArgs &args) override
	{
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
		debugValue = std::to_string(quantizedCv);

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
			this->setText(std::to_string(myQuantizerModule->incomingCv) + "\n" + myQuantizerModule->debugValue);
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

		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(15.24, 77.478)), module, MyQuantizer::CV_INPUT));

		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(15.24, 108.713)), module, MyQuantizer::CV_OUTPUT));

		textField = createWidget<CustomTextFieldWidget>(mm2px(Vec(0.0, 50.0)));
		textField->myQuantizerModule = module;
		textField->box.size = mm2px(Vec(40.0, 15.0));
		addChild(textField);
	}
};

Model *modelMyQuantizer = createModel<MyQuantizer, MyQuantizerWidget>("MyQuantizer");