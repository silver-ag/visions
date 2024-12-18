#include "plugin.hpp"


float lerp(float a, float b, float t);

struct AffineTransform : Module {
	enum ParamId {
		TRANSLATE_X_MOD_PARAM,
		SCALE_X_MOD_PARAM,
		TRANSLATE_X_PARAM,
		SCALE_X_PARAM,
		TRANSLATE_Y_MOD_PARAM,
		SCALE_Y_MOD_PARAM,
		TRANSLATE_Y_PARAM,
		SCALE_Y_PARAM,
		SKEW_X_MOD_PARAM,
		SKEW_X_PARAM,
		ROTATE_MOD_PARAM,
		SKEW_Y_MOD_PARAM,
		SKEW_Y_PARAM,
		ROTATE_PARAM,
		PARAMS_LEN
	};
	enum InputId {
		SCALE_VECTOR_INPUT,
		SCALE_X_INPUT,
		SCALE_Y_INPUT,
		TRANSLATE_VECTOR_INPUT,
		TRANSLATE_X_INPUT,
		TRANSLATE_Y_INPUT,
		SKEW_VECTOR_INPUT,
		SKEW_X_INPUT,
		SKEW_Y_INPUT,
		IN_POSITION_INPUT,
		IN_X_INPUT,
		IN_Y_INPUT,
		ROTATE_INPUT,
		INPUTS_LEN
	};
	enum OutputId {
		OUT_POSITION_OUTPUT,
		OUT_Y_OUTPUT,
		OUT_X_OUTPUT,
		OUTPUTS_LEN
	};
	enum LightId {
		LIGHTS_LEN
	};

	AffineTransform() {
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
		configParam(TRANSLATE_X_MOD_PARAM, 0.f, 1.f, 0.f, "Translate X modulation");
		configParam(SCALE_X_MOD_PARAM, 0.f, 1.f, 0.f, "Scale X modulation");
		configParam(TRANSLATE_X_PARAM, -1.f, 1.f, 0.f, "Translate X");
		configParam(SCALE_X_PARAM, -1.f, 1.f, 0.f, "Scale X");
		configParam(TRANSLATE_Y_MOD_PARAM, 0.f, 1.f, 0.f, "Translate Y modulation");
		configParam(SCALE_Y_MOD_PARAM, 0.f, 1.f, 0.f, "Scale Y modulation");
		configParam(TRANSLATE_Y_PARAM, -1.f, 1.f, 0.f, "Translate Y");
		configParam(SCALE_Y_PARAM, -1.f, 1.f, 0.f, "Scale Y");
		configParam(SKEW_X_MOD_PARAM, 0.f, 1.f, 0.f, "Skew X modulation");
		configParam(SKEW_X_PARAM, -1.f, 1.f, 0.f, "Skew X");
		configParam(ROTATE_MOD_PARAM, 0.f, 1.f, 0.f, "Rotate modulation");
		configParam(SKEW_Y_MOD_PARAM, 0.f, 1.f, 0.f, "Skew Y modulation");
		configParam(SKEW_Y_PARAM, -1.f, 1.f, 0.f, "Skew Y");
		configParam(ROTATE_PARAM, 0.f, 1.f, 0.f, "Rotate");
		configInput(SCALE_VECTOR_INPUT, "Scale Polyphonic");
		configInput(SCALE_X_INPUT, "Scale X");
		configInput(SCALE_Y_INPUT, "Scale Y");
		configInput(TRANSLATE_VECTOR_INPUT, "Translate Polyphonic");
		configInput(TRANSLATE_X_INPUT, "Translate X");
		configInput(TRANSLATE_Y_INPUT, "Translate Y");
		configInput(SKEW_VECTOR_INPUT, "Skew Polyphonic");
		configInput(SKEW_X_INPUT, "Skew X");
		configInput(SKEW_Y_INPUT, "Skew Y");
		configInput(IN_POSITION_INPUT, "Polyphonic");
		configInput(IN_X_INPUT, "X");
		configInput(IN_Y_INPUT, "Y");
		configInput(ROTATE_INPUT, "Rotate");
		configOutput(OUT_POSITION_OUTPUT, "Polyphonic");
		configOutput(OUT_Y_OUTPUT, "Y");
		configOutput(OUT_X_OUTPUT, "X");
	}

	void process(const ProcessArgs& args) override {
		float x = 0;
		float y = 0;
		if (inputs[IN_POSITION_INPUT].isConnected()) {
			x = inputs[IN_POSITION_INPUT].getPolyVoltage(0);
			y = inputs[IN_POSITION_INPUT].getPolyVoltage(1);
		} else {
			x = inputs[IN_X_INPUT].getVoltage();
			y = inputs[IN_Y_INPUT].getVoltage();
		}

		// SCALE
		float scale_factor_x = lerp(params[SCALE_X_PARAM].getValue() * 5, inputs[SCALE_X_INPUT].getVoltage(), params[SCALE_X_MOD_PARAM].getValue());
		float scale_factor_y = lerp(params[SCALE_Y_PARAM].getValue() * 5, inputs[SCALE_Y_INPUT].getVoltage(), params[SCALE_Y_MOD_PARAM].getValue());
		x *= scale_factor_x;
		y *= scale_factor_y;

		outputs[OUT_X_OUTPUT].setVoltage(x);
		outputs[OUT_Y_OUTPUT].setVoltage(y);
		outputs[OUT_POSITION_OUTPUT].setVoltage(x,0);
		outputs[OUT_POSITION_OUTPUT].setVoltage(y,1);
		outputs[OUT_POSITION_OUTPUT].setChannels(2);
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

float lerp(float a, float b, float t) {
        return (a * (1.0 - t)) + (b * t);
}


struct AffineTransformWidget : ModuleWidget {
	AffineTransformWidget(AffineTransform* module) {
		setModule(module);
		setPanel(createPanel(asset::plugin(pluginInstance, "res/AffineTransform.svg")));

		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		addParam(createParamCentered<Trimpot>(mm2px(Vec(27.791, 47)), module, AffineTransform::TRANSLATE_X_MOD_PARAM));
		addParam(createParamCentered<Trimpot>(mm2px(Vec(68.252, 47)), module, AffineTransform::SCALE_X_MOD_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(39.728, 45)), module, AffineTransform::TRANSLATE_X_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(80.19, 45)), module, AffineTransform::SCALE_X_PARAM));
		addParam(createParamCentered<Trimpot>(mm2px(Vec(27.733, 55)), module, AffineTransform::TRANSLATE_Y_MOD_PARAM));
		addParam(createParamCentered<Trimpot>(mm2px(Vec(68.194, 55)), module, AffineTransform::SCALE_Y_MOD_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(39.671, 57)), module, AffineTransform::TRANSLATE_Y_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(80.132, 57)), module, AffineTransform::SCALE_Y_PARAM));
		addParam(createParamCentered<Trimpot>(mm2px(Vec(27.693, 84)), module, AffineTransform::SKEW_X_MOD_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(39.631, 82)), module, AffineTransform::SKEW_X_PARAM));
		addParam(createParamCentered<Trimpot>(mm2px(Vec(63.363, 85)), module, AffineTransform::ROTATE_MOD_PARAM));
		addParam(createParamCentered<Trimpot>(mm2px(Vec(27.635, 92)), module, AffineTransform::SKEW_Y_MOD_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(39.573, 94)), module, AffineTransform::SKEW_Y_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(63.363, 97)), module, AffineTransform::ROTATE_PARAM));

		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(56, 35)), module, AffineTransform::SCALE_VECTOR_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(56, 47)), module, AffineTransform::SCALE_X_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(56, 56)), module, AffineTransform::SCALE_Y_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(15.5, 35)), module, AffineTransform::TRANSLATE_VECTOR_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(15.5, 47)), module, AffineTransform::TRANSLATE_X_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(15.5, 56)), module, AffineTransform::TRANSLATE_Y_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(15.5, 72.3)), module, AffineTransform::SKEW_VECTOR_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(15.5, 84.3)), module, AffineTransform::SKEW_X_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(15.5, 93.3)), module, AffineTransform::SKEW_Y_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(10.4, 10.1)), module, AffineTransform::IN_POSITION_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(21.6, 10.1)), module, AffineTransform::IN_X_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(30.6, 10.1)), module, AffineTransform::IN_Y_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(63.616, 72.5)), module, AffineTransform::ROTATE_INPUT));

		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(80.4, 118.3)), module, AffineTransform::OUT_POSITION_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(69.4, 118.3)), module, AffineTransform::OUT_Y_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(60.4, 118.3)), module, AffineTransform::OUT_X_OUTPUT));
	}
};


Model* modelAffineTransform = createModel<AffineTransform, AffineTransformWidget>("AffineTransform");
