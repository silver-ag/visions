#include "plugin.hpp"


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
		configParam(TRANSLATE_X_MOD_PARAM, 0.f, 1.f, 0.f, "");
		configParam(SCALE_X_MOD_PARAM, 0.f, 1.f, 0.f, "");
		configParam(TRANSLATE_X_PARAM, 0.f, 1.f, 0.f, "");
		configParam(SCALE_X_PARAM, 0.f, 1.f, 0.f, "");
		configParam(TRANSLATE_Y_MOD_PARAM, 0.f, 1.f, 0.f, "");
		configParam(SCALE_Y_MOD_PARAM, 0.f, 1.f, 0.f, "");
		configParam(TRANSLATE_Y_PARAM, 0.f, 1.f, 0.f, "");
		configParam(SCALE_Y_PARAM, 0.f, 1.f, 0.f, "");
		configParam(SKEW_X_MOD_PARAM, 0.f, 1.f, 0.f, "");
		configParam(SKEW_X_PARAM, 0.f, 1.f, 0.f, "");
		configParam(ROTATE_MOD_PARAM, 0.f, 1.f, 0.f, "");
		configParam(SKEW_Y_MOD_PARAM, 0.f, 1.f, 0.f, "");
		configParam(SKEW_Y_PARAM, 0.f, 1.f, 0.f, "");
		configParam(ROTATE_PARAM, 0.f, 1.f, 0.f, "");
		configInput(SCALE_VECTOR_INPUT, "");
		configInput(SCALE_X_INPUT, "");
		configInput(SCALE_Y_INPUT, "");
		configInput(TRANSLATE_VECTOR_INPUT, "");
		configInput(TRANSLATE_X_INPUT, "");
		configInput(TRANSLATE_Y_INPUT, "");
		configInput(SKEW_VECTOR_INPUT, "");
		configInput(SKEW_X_INPUT, "");
		configInput(SKEW_Y_INPUT, "");
		configInput(IN_POSITION_INPUT, "");
		configInput(IN_X_INPUT, "");
		configInput(IN_Y_INPUT, "");
		configInput(ROTATE_INPUT, "");
		configOutput(OUT_POSITION_OUTPUT, "");
		configOutput(OUT_Y_OUTPUT, "");
		configOutput(OUT_X_OUTPUT, "");
	}

	void process(const ProcessArgs& args) override {
	}
};


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
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(15.5, 72)), module, AffineTransform::SKEW_VECTOR_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(15.5, 84)), module, AffineTransform::SKEW_X_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(15.5, 93)), module, AffineTransform::SKEW_Y_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(10.418, 178.861)), module, AffineTransform::IN_POSITION_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(21.531, 178.851)), module, AffineTransform::IN_X_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(31.379, 178.926)), module, AffineTransform::IN_Y_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(63.616, 241.408)), module, AffineTransform::ROTATE_INPUT));

		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(-80.869, -286.917)), module, AffineTransform::OUT_POSITION_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(-69.757, -286.928)), module, AffineTransform::OUT_Y_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(-59.908, -286.853)), module, AffineTransform::OUT_X_OUTPUT));
	}
};


Model* modelAffineTransform = createModel<AffineTransform, AffineTransformWidget>("AffineTransform");
