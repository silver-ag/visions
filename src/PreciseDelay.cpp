#include "plugin.hpp"

struct PreciseDelay : Module {
	enum ParamId {
		DELAY_1_PARAM,
		DELAY_2_PARAM,
		DELAY_3_PARAM,
		DELAY_4_PARAM,
		DELAY_5_PARAM,
		DELAY_6_PARAM,
		PARAMS_LEN
	};
	enum InputId {
		INPUT_1_INPUT,
		INPUT_2_INPUT,
		INPUT_3_INPUT,
		INPUT_4_INPUT,
		INPUT_5_INPUT,
		INPUT_6_INPUT,
		INPUTS_LEN
	};
	enum OutputId {
		OUTPUT_1_OUTPUT,
		OUTPUT_2_OUTPUT,
		OUTPUT_3_OUTPUT,
		OUTPUT_4_OUTPUT,
		OUTPUT_5_OUTPUT,
		OUTPUT_6_OUTPUT,
		OUTPUTS_LEN
	};
	enum LightId {
		LIGHTS_LEN
	};

	float delay_line_a[32][16];
	float delay_line_b[32][16];
	float delay_line_c[32][16];
	float delay_line_d[32][16];
	float delay_line_e[32][16];
	float delay_line_f[32][16];
	int write_pointer_a = 0;
	int write_pointer_b = 0;
	int write_pointer_c = 0;
	int write_pointer_d = 0;
	int write_pointer_e = 0;
	int write_pointer_f = 0;

	PreciseDelay() {
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
		configParam(DELAY_1_PARAM, 1, 32, 1, "Delay A");
		paramQuantities[DELAY_1_PARAM]->snapEnabled = true;
		configParam(DELAY_2_PARAM, 1, 32, 1, "Delay B");
		paramQuantities[DELAY_2_PARAM]->snapEnabled = true;
		configParam(DELAY_3_PARAM, 1, 32, 1, "Delay C");
		paramQuantities[DELAY_3_PARAM]->snapEnabled = true;
		configParam(DELAY_4_PARAM, 1, 32, 1, "Delay D");
		paramQuantities[DELAY_4_PARAM]->snapEnabled = true;
		configParam(DELAY_5_PARAM, 1, 32, 1, "Delay E");
		paramQuantities[DELAY_5_PARAM]->snapEnabled = true;
		configParam(DELAY_6_PARAM, 1, 32, 1, "Delay F");
		paramQuantities[DELAY_6_PARAM]->snapEnabled = true;
		configInput(INPUT_1_INPUT, "Input A");
		configInput(INPUT_2_INPUT, "Input B");
		configInput(INPUT_3_INPUT, "Input C");
		configInput(INPUT_4_INPUT, "Input D");
		configInput(INPUT_5_INPUT, "Input E");
		configInput(INPUT_6_INPUT, "Input F");
		configOutput(OUTPUT_1_OUTPUT, "Output A");
		configOutput(OUTPUT_2_OUTPUT, "Output B");
		configOutput(OUTPUT_3_OUTPUT, "Output C");
		configOutput(OUTPUT_4_OUTPUT, "Output D");
		configOutput(OUTPUT_5_OUTPUT, "Output E");
		configOutput(OUTPUT_6_OUTPUT, "Output F");
	}

	void process(const ProcessArgs& args) override {
		write_pointer_a = (write_pointer_a + 1) % 32;
		write_pointer_b = (write_pointer_b + 1) % 32;
		write_pointer_c = (write_pointer_c + 1) % 32;
		write_pointer_d = (write_pointer_d + 1) % 32;
		write_pointer_e = (write_pointer_e + 1) % 32;
		write_pointer_f = (write_pointer_f + 1) % 32;

		for (int c = 0; c < 16; c++) {
			delay_line_a[write_pointer_a][c] = inputs[INPUT_1_INPUT].getPolyVoltage(c);
			delay_line_b[write_pointer_b][c] = inputs[INPUT_2_INPUT].getPolyVoltage(c);
			delay_line_c[write_pointer_c][c] = inputs[INPUT_3_INPUT].getPolyVoltage(c);
			delay_line_d[write_pointer_d][c] = inputs[INPUT_4_INPUT].getPolyVoltage(c);
			delay_line_e[write_pointer_e][c] = inputs[INPUT_5_INPUT].getPolyVoltage(c);
			delay_line_f[write_pointer_f][c] = inputs[INPUT_6_INPUT].getPolyVoltage(c);
		}

		for (int c = 0; c < 16; c++) {
			outputs[OUTPUT_1_OUTPUT].setVoltage(delay_line_a[(write_pointer_a + 33 - int(params[DELAY_1_PARAM].getValue())) % 32][c], c);
			outputs[OUTPUT_2_OUTPUT].setVoltage(delay_line_b[(write_pointer_b + 33 - int(params[DELAY_2_PARAM].getValue())) % 32][c], c);
			outputs[OUTPUT_3_OUTPUT].setVoltage(delay_line_c[(write_pointer_c + 33 - int(params[DELAY_3_PARAM].getValue())) % 32][c], c);
			outputs[OUTPUT_4_OUTPUT].setVoltage(delay_line_d[(write_pointer_d + 33 - int(params[DELAY_4_PARAM].getValue())) % 32][c], c);
			outputs[OUTPUT_5_OUTPUT].setVoltage(delay_line_e[(write_pointer_e + 33 - int(params[DELAY_5_PARAM].getValue())) % 32][c], c);
			outputs[OUTPUT_6_OUTPUT].setVoltage(delay_line_f[(write_pointer_f + 33 - int(params[DELAY_6_PARAM].getValue())) % 32][c], c);
		}

		outputs[OUTPUT_1_OUTPUT].setChannels(inputs[INPUT_1_INPUT].getChannels());
		outputs[OUTPUT_2_OUTPUT].setChannels(inputs[INPUT_2_INPUT].getChannels());
		outputs[OUTPUT_3_OUTPUT].setChannels(inputs[INPUT_3_INPUT].getChannels());
		outputs[OUTPUT_4_OUTPUT].setChannels(inputs[INPUT_4_INPUT].getChannels());
		outputs[OUTPUT_5_OUTPUT].setChannels(inputs[INPUT_5_INPUT].getChannels());
		outputs[OUTPUT_6_OUTPUT].setChannels(inputs[INPUT_6_INPUT].getChannels());

	}
};


struct PreciseDelayWidget : ModuleWidget {
	PreciseDelayWidget(PreciseDelay* module) {
		setModule(module);
		setPanel(createPanel(asset::plugin(pluginInstance, "res/PreciseDelay.svg")));

		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(30.73, 25.25)), module, PreciseDelay::DELAY_1_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(30.73, 43.25)), module, PreciseDelay::DELAY_2_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(30.73, 61.25)), module, PreciseDelay::DELAY_3_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(30.73, 79.25)), module, PreciseDelay::DELAY_4_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(30.73, 97.25)), module, PreciseDelay::DELAY_5_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(30.73, 115.25)), module, PreciseDelay::DELAY_6_PARAM));

		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(10.75, 25.25)), module, PreciseDelay::INPUT_1_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(10.75, 43.25)), module, PreciseDelay::INPUT_2_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(10.75, 61.25)), module, PreciseDelay::INPUT_3_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(10.75, 79.25)), module, PreciseDelay::INPUT_4_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(10.75, 97.25)), module, PreciseDelay::INPUT_5_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(10.75, 115.25)), module, PreciseDelay::INPUT_6_INPUT));

		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(50.71, 25.25)), module, PreciseDelay::OUTPUT_1_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(50.71, 43.25)), module, PreciseDelay::OUTPUT_2_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(50.71, 61.25)), module, PreciseDelay::OUTPUT_3_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(50.71, 79.25)), module, PreciseDelay::OUTPUT_4_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(50.71, 97.25)), module, PreciseDelay::OUTPUT_5_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(50.71, 115.25)), module, PreciseDelay::OUTPUT_6_OUTPUT));
	}
};


Model* modelPreciseDelay = createModel<PreciseDelay, PreciseDelayWidget>("PreciseDelay");
