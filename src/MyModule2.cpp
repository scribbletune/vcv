#include "plugin.hpp"

struct MyModule2 : Module {
	float phase = 0.f;
	float blinkPhase = 0.f;
	float phase2 = 0.f;
	float blinkPhase2 = 0.f;
	enum ParamIds {
		PITCH_PARAM,
		PITCH_PARAM2,
		NUM_PARAMS
	};
	enum InputIds {
		PITCH_INPUT,
		PITCH_INPUT2,
		NUM_INPUTS
	};
	enum OutputIds {
		SINE_OUTPUT,
		SINE_OUTPUT2,
		NUM_OUTPUTS
	};
	enum LightIds {
		BLINK_LIGHT,
		BLINK_LIGHT2,
		NUM_LIGHTS
	};

	MyModule2() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		configParam(PITCH_PARAM, 0.f, 1.f, 0.f, "");
		configParam(PITCH_PARAM2, 0.f, 1.f, 0.f, "");
	}

	void process(const ProcessArgs& args) override {
		// Compute the frequency from the pitch parameter and input
		float pitch = params[PITCH_PARAM].getValue();
		pitch += inputs[PITCH_INPUT].getVoltage();
		pitch = clamp(pitch, -4.f, 4.f);

		float pitch2 = params[PITCH_PARAM2].getValue();
		pitch2 += inputs[PITCH_INPUT2].getVoltage();
		pitch2 = clamp(pitch2, -4.f, 4.f);

		// The default pitch is C4 = 261.6256f
		float freq = dsp::FREQ_C4 * std::pow(2.f, pitch);
		float freq2 = dsp::FREQ_C4 * std::pow(2.f, pitch2);

		// Accumulate the phase
		phase += freq * args.sampleTime;
		if (phase >= 0.5f)
			phase -= 1.f;

		phase2 += freq2 * args.sampleTime;
		if (phase2 >= 0.5f)
			phase2 -= 1.f;

		// Compute the sine output
		float sine = std::sin(2.f * M_PI * phase);
		// Audio signals are typically +/-5V
		// https://vcvrack.com/manual/VoltageStandards.html
		outputs[SINE_OUTPUT].setVoltage(5.f * sine);

		float sine2 = std::sin(2.f * M_PI * phase2);
		outputs[SINE_OUTPUT2].setVoltage(5.f * sine2);

		// Blink light at 1Hz
		blinkPhase += args.sampleTime;
		if (blinkPhase >= 1.f)
			blinkPhase -= 1.f;
		lights[BLINK_LIGHT].setBrightness(blinkPhase < 0.5f ? 1.f : 0.f);

		blinkPhase2 += args.sampleTime;
		if (blinkPhase2 >= 1.f)
			blinkPhase2 -= 1.f;
		lights[BLINK_LIGHT2].setBrightness(blinkPhase2 < 0.5f ? 1.f : 0.f);
	}
};


struct MyModule2Widget : ModuleWidget {
	MyModule2Widget(MyModule2* module) {
		setModule(module);
		setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/MyModule2.svg")));

		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(45.113, 46.054)), module, MyModule2::PITCH_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(15.24, 46.063)), module, MyModule2::PITCH_PARAM2));

		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(45.113, 77.469)), module, MyModule2::PITCH_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(15.24, 77.478)), module, MyModule2::PITCH_INPUT2));

		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(45.113, 108.703)), module, MyModule2::SINE_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(15.24, 108.713)), module, MyModule2::SINE_OUTPUT2));

		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(45.113, 25.8)), module, MyModule2::BLINK_LIGHT));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(15.24, 25.81)), module, MyModule2::BLINK_LIGHT2));
	}
};


Model* modelMyModule2 = createModel<MyModule2, MyModule2Widget>("MyModule2");