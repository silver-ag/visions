#include "plugin.hpp"
#include "widgets/switches.hpp"

#define PI 3.14159265358979323846

float mod(float a, float b);
float lerp_cf(float a, float b, float t);

struct CoordinateFolder : Module {
	enum ParamId {
		MODE_PARAM,
		AMOUNT_PARAM,
		AMOUNT_MOD_PARAM,
		PARAMS_LEN
	};
	enum InputId {
		POSITION_INPUT,
		X_INPUT,
		Y_INPUT,
		AMOUNT_INPUT,
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

	CoordinateFolder() {
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
		configSwitch(MODE_PARAM, 0, 1, 0, "Mode", {"Square", "Radial"});
                configParam(AMOUNT_PARAM, 1, 11, 1, "Amount");
                configParam(AMOUNT_MOD_PARAM, 0, 1, 0, "Amount modulation");
                configInput(AMOUNT_INPUT, "Amount CV");
                configInput(POSITION_INPUT, "Polyphonic position");
                configInput(X_INPUT, "X position");
                configInput(Y_INPUT, "Y position");
                configOutput(POSITION_OUTPUT, "Polyphonic position");
                configOutput(X_OUTPUT, "X position");
                configOutput(Y_OUTPUT, "Y position");
	}

	void process(const ProcessArgs& args) override {
		float amount = params[AMOUNT_PARAM].getValue();
		amount = lerp_cf(amount, inputs[AMOUNT_INPUT].getVoltage()+1, params[AMOUNT_MOD_PARAM].getValue());
		float x_in = 0;
		float y_in = 0;
		float x_out = 0;
		float y_out = 0;
		if (inputs[POSITION_INPUT].isConnected()) {
			x_in = inputs[POSITION_INPUT].getPolyVoltage(0);
			y_in = inputs[POSITION_INPUT].getPolyVoltage(1);
		} else {
			x_in = inputs[X_INPUT].getVoltage();
			y_in = inputs[Y_INPUT].getVoltage();
		}

		if (params[MODE_PARAM].getValue() == 0) {
			// square
			x_out = mod(x_in, 10.0/amount);
			y_out = mod(y_in, 10.0/amount);
		} else {
			// radial
			float r = std::sqrt((x_in*x_in)+(y_in*y_in));
			float theta = std::atan2(x_in, y_in);
			theta = mod(theta, 2*PI/float(amount));
			x_out = r * std::sin(theta);
			y_out = r * std::cos(theta);
		}
		outputs[X_OUTPUT].setVoltage(x_out);
		outputs[Y_OUTPUT].setVoltage(y_out);
		outputs[POSITION_OUTPUT].setVoltage(x_out, 0);
		outputs[POSITION_OUTPUT].setVoltage(y_out, 1);
		outputs[POSITION_OUTPUT].setChannels(2);
	}
};

float mod(float a, float b) {
        while (a > b) {
                a -= b;
        }
        while (a < 0) {
                a += b;
        }
        return a;
}

float lerp_cf(float a, float b, float t) { // ass opposed to lerp_at in AffineTransform.cpp - a truly hideous kludge to avoid multiple definitions
        return (a * (1.0 - t)) + (b * t);
}

struct CoordinateFolderWidget : ModuleWidget {
	CoordinateFolderWidget(CoordinateFolder* module) {
		setModule(module);
		setPanel(createPanel(asset::plugin(pluginInstance, "res/CoordinateFolder.svg")));

		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));


		addChild(createParam<HorizontalSwitch>(mm2px(Vec(5.5, 40)), module, CoordinateFolder::MODE_PARAM));
                addChild(createParam<RoundBlackKnob>(mm2px(Vec(3.5, 67)), module, CoordinateFolder::AMOUNT_PARAM));
                addChild(createParam<Trimpot>(mm2px(Vec(13, 63)), module, CoordinateFolder::AMOUNT_MOD_PARAM));
                addInput(createInputCentered<PJ301MPort>(mm2px(Vec(8, 59)), module, CoordinateFolder::AMOUNT_INPUT));

                addInput(createInputCentered<PJ301MPort>(mm2px(Vec(8.5, 12.5)), module, CoordinateFolder::POSITION_INPUT));
                addInput(createInputCentered<PJ301MPort>(mm2px(Vec(8.5, 24)), module, CoordinateFolder::X_INPUT));
                addInput(createInputCentered<PJ301MPort>(mm2px(Vec(8.5, 32.5)), module, CoordinateFolder::Y_INPUT));
                addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(8.5, 111)), module, CoordinateFolder::POSITION_OUTPUT));
                addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(8.5, 91)), module, CoordinateFolder::X_OUTPUT));
                addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(8.5, 99.5)), module, CoordinateFolder::Y_OUTPUT));
	}
};


Model* modelCoordinateFolder = createModel<CoordinateFolder, CoordinateFolderWidget>("CoordinateFolder");
