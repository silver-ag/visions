#include "plugin.hpp"
#include "widgets/switches.hpp"

struct PathGenerator : Module {
	enum ParamId {
		MODE_PARAM,
		SPEED_PARAM,
		RESOLUTION_PARAM,
		XY_POLARITY_PARAM,
		PARAMS_LEN
	};
	enum InputId {
		INPUTS_LEN
	};
	enum OutputId {
		POSITION_OUTPUT,
		X_OUTPUT,
		Y_OUTPUT,
		OUTPUTS_LEN
	};
	enum LightId {
		LIGHTS_LEN
	};

	float x_position = 0; // in terms of resolution
	float y_position = 0;

	int direction = 0; // for spiral mode

	PathGenerator() {
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
		configSwitch(MODE_PARAM, 0.f, 3.f, 0.f, "Mode", {"Scanning", "Boustrophedon", "Spiral", "Random"});
		configSwitch(XY_POLARITY_PARAM, 0.f, 1.f, 0.f, "Bi/Unipolar Position", {"Bipolar", "Unipolar"});
		configParam(SPEED_PARAM, 0, 1, 1, "Speed");
		configParam(RESOLUTION_PARAM, 50, 200, 100, "Resolution");
		paramQuantities[RESOLUTION_PARAM]->snapEnabled = true;
		configOutput(POSITION_OUTPUT, "Polyphonic position");
		configOutput(X_OUTPUT, "X position");
		configOutput(Y_OUTPUT, "Y position");
	}

	void process(const ProcessArgs& args) override {
		int mode = params[MODE_PARAM].getValue();
		float speed = params[SPEED_PARAM].getValue();
		int res = params[RESOLUTION_PARAM].getValue();
		if (mode == 0) { // scanning
			x_position += speed;
			if (x_position >= res) {
				x_position = 0;
				y_position += speed;
				if (y_position >= res) {
					y_position -= res;
				}
			}
		} else if (mode == 1) { // boustro
			if (int(y_position) % 2 == 0) {
				x_position += speed;
				if (x_position >= res) {
					y_position += speed;
					if (y_position >= res) {
						y_position -= res;
					}
				}
			} else {
				x_position -= speed;
				if (x_position < 0) {
					y_position += speed;
					if (y_position >= res) {
						y_position -= res;
					}
				}
			}
		} else if (mode == 2) { // spiral
			if (x_position < 0 or x_position > res) {
				x_position = res/2;
			}
			if (y_position < 0 or y_position > res) {
				y_position = res/2;
			}
			if (direction == 0) { // left
				x_position += speed;
				if (x_position > (res - y_position)) {
					direction = 1;
				}
			} else if (direction == 1) { // down
				y_position += speed;
				if (y_position > x_position) {
					direction = 2;
				}
			} else if (direction == 2) { // right
				x_position -= speed;
				if (x_position < (res - y_position)) {
					direction = 3;
				}
			} else if (direction == 3) { // up
				y_position -= speed;
				if (y_position < x_position) {
					direction = 0;
				}
			}
		} else if (mode == 3) { // random
			if (random::uniform() < speed) {
				x_position = int(random::uniform() * res);
				y_position = int(random::uniform() * res);
			}
		}
		float x_voltage = (10 * float(x_position) / res);
		float y_voltage = (10 * float(y_position) / res);
		if (params[XY_POLARITY_PARAM].getValue() == 0) {
			x_voltage -= 5;
			y_voltage -= 5;
		}
		outputs[X_OUTPUT].setVoltage(x_voltage);
		outputs[Y_OUTPUT].setVoltage(y_voltage);
		outputs[POSITION_OUTPUT].setVoltage(x_voltage, 0);
		outputs[POSITION_OUTPUT].setVoltage(y_voltage, 1);
		outputs[POSITION_OUTPUT].setChannels(2);
	}
};

struct PathGeneratorWidget : ModuleWidget {
	PathGeneratorWidget(PathGenerator* module) {
		setModule(module);
		setPanel(createPanel(asset::plugin(pluginInstance, "res/PathGenerator.svg")));

		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		addChild(createParam<RoundBlackKnob>(mm2px(Vec(5.5, 15)), module, PathGenerator::MODE_PARAM));
		addChild(createParam<RoundBlackKnob>(mm2px(Vec(5.5, 34)), module, PathGenerator::SPEED_PARAM));
		addChild(createParam<Trimpot>(mm2px(Vec(7, 53)), module, PathGenerator::RESOLUTION_PARAM));
		addChild(createParam<PolaritySwitch>(mm2px(Vec(5.5, 107)), module, PathGenerator::XY_POLARITY_PARAM));

		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(8.5, 79.5)), module, PathGenerator::POSITION_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(8.5, 91)), module, PathGenerator::X_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(8.5, 100)), module, PathGenerator::Y_OUTPUT));

	}
};


Model* modelPathGenerator = createModel<PathGenerator, PathGeneratorWidget>("PathGenerator");
