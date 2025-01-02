#include "plugin.hpp"
#include "nanovg.h"
#include "widgets/switches.hpp"

std::vector<int> hsv_to_rgb(int h, int s, int v);

struct VideoOut : Module {
	enum ParamId {
		RGB_HSV_PARAM,
		CLEAR_PARAM,
		RESOLUTION_PARAM,
		PARAMS_LEN
	};
	enum InputId {
		XY_POLY_INPUT,
		X_INPUT,
		Y_INPUT,
		COLOUR_POLY_INPUT,
		R_H_INPUT,
		G_S_INPUT,
		B_V_INPUT,
		INPUTS_LEN
	};
	enum OutputId {
		OUTPUTS_LEN
	};
	enum LightId {
		LIGHTS_LEN
	};

	unsigned char screen_data[160000] = {};
	int width = 100;
	int height = 100;

	bool already_cleared = false;
	bool resolution_changed = false;

	VideoOut() {
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
		configSwitch(RGB_HSV_PARAM, 0.f, 1.f, 0.f, "RGB or HSV", {"RGB", "HSV"});
		configButton(CLEAR_PARAM, "Clear");
		configParam(RESOLUTION_PARAM, 50, 200, 100, "Resolution");
		paramQuantities[RESOLUTION_PARAM]->snapEnabled = true;
		configInput(XY_POLY_INPUT, "Polyphonic XY");
		configInput(X_INPUT, "X coordinate");
		configInput(Y_INPUT, "Y coordinate");
		configInput(COLOUR_POLY_INPUT, "Polyphonic colour");
		configInput(R_H_INPUT, "Red or Hue");
		configInput(G_S_INPUT, "Green or Saturation");
		configInput(B_V_INPUT, "Blue or Value");
	}

	void process(const ProcessArgs &args) override {
		float x_voltage = 0;
		float y_voltage = 0;
		if (inputs[XY_POLY_INPUT].isConnected()) {
			x_voltage = inputs[XY_POLY_INPUT].getPolyVoltage(0);
			y_voltage = inputs[XY_POLY_INPUT].getPolyVoltage(1);
		} else {
			x_voltage = inputs[X_INPUT].getVoltage();
			y_voltage = inputs[Y_INPUT].getVoltage();
		}
		int x = (int) (std::round((((x_voltage / 10) + 0.5f) * width)));
		int y = (int) (std::round((((y_voltage / 10) + 0.5f) * height)));

		// write pixel only if it's onscreen
		if (x >= 0 && x < width && y >= 0 && y < height) {
			float rh_voltage = 0;
			float gs_voltage = 0;
			float bv_voltage = 0;
			if (inputs[COLOUR_POLY_INPUT].isConnected()) {
				rh_voltage = inputs[COLOUR_POLY_INPUT].getPolyVoltage(0);
				gs_voltage = inputs[COLOUR_POLY_INPUT].getPolyVoltage(1);
				bv_voltage = inputs[COLOUR_POLY_INPUT].getPolyVoltage(2);
			} else {
				rh_voltage = inputs[R_H_INPUT].getVoltage();
				gs_voltage = inputs[G_S_INPUT].getVoltage();
				bv_voltage = inputs[B_V_INPUT].getVoltage();
			}
			int rh = int(clamp((rh_voltage / 10) + 0.5f, 0.0, 0.999) * 256);
			int gs = int(clamp((gs_voltage / 10) + 0.5f, 0.0, 0.999) * 256);
			int bv = int(clamp((bv_voltage / 10) + 0.5f, 0.0, 0.999) * 256);

			int offset = 4 * (x + (y * height));
			if (params[RGB_HSV_PARAM].getValue() == 0) {
				screen_data[offset] = rh;
				screen_data[offset+1] = gs;
				screen_data[offset+2] = bv;
			} else {
				std::vector<int> rgb = hsv_to_rgb(rh, gs, bv);
				screen_data[offset] = rgb[0];
				screen_data[offset+1] = rgb[1];
				screen_data[offset+2] = rgb[2];
			}
		}

		if (params[CLEAR_PARAM].getValue() == 1) {
			clear();
			already_cleared = true;
		} else if (already_cleared) { // note the 'else if' here means this is only evaluated when the button isn't being held down
			already_cleared = false;
		}

		int resolution = params[RESOLUTION_PARAM].getValue();
		if (resolution != width) {
			width = resolution;
			height = resolution;
			resolution_changed = true;
		}
	}

	void clear() {
		for (int x = 0; x < width; x++) {
			for (int y = 0; y < height; y++) {
				int offset = 4 * (x + (height * y));
				screen_data[offset] = 128;
				screen_data[offset+1] = 128;
				screen_data[offset+2] = 128;
				screen_data[offset+3] = 255;
			}
		}
	}

};

struct VideoDisplay : TransparentWidget {
	VideoOut* module = nullptr;
	ModuleWidget* mw = nullptr;
	int image = -1;
	float real_width = -1;
	float real_height = -1;
	void drawLayer(const DrawArgs& args, int layer) override {
		NVGcontext* vg = args.vg;
		//nvgShapeAntiAlias(vg, false); no apprent effect?
		if (module && (layer == 1)) {
			if (image == -1 or module->resolution_changed) {
				module->clear(); // initialise
				image = nvgCreateImageRGBA(vg, module->width, module->height, 0, module->screen_data);
				module->resolution_changed = false;
			}
			nvgUpdateImage(vg, image, module->screen_data);

			NVGpaint paint = nvgImagePattern(vg, 0, 0, module->width, module->height, 0, image, 1);

			nvgBeginPath(vg);
			nvgScale(vg, real_width/module->width, real_height/module->height);
			nvgRect(vg, 0, 0, module->width, module->height);

			nvgFillPaint(vg, paint);
			nvgFill(vg);
		}
	}
};


struct VideoOutWidget : ModuleWidget {
	VideoOut* module = nullptr;
	int image = -1;
	int real_x, real_y, real_w, real_h = 0;
	VideoOutWidget(VideoOut* parent_module) {
		module = parent_module;
		setModule(module);
		setPanel(createPanel(asset::plugin(pluginInstance, "res/VideoOut.svg")));

		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		addParam(createParam<HorizontalSwitch>(mm2px(Vec(12, 85)), module, VideoOut::RGB_HSV_PARAM));

		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(6.7, 12)), module, VideoOut::XY_POLY_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(6.7, 23.5)), module, VideoOut::X_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(6.7, 32.5)), module, VideoOut::Y_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(6.7, 50.5)), module, VideoOut::COLOUR_POLY_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(6.7, 63)), module, VideoOut::R_H_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(6.7, 72)), module, VideoOut::G_S_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(6.7, 81)), module, VideoOut::B_V_INPUT));

		// mm2px(Vec(112.94, 112.94))
		VideoDisplay* display = createWidget<VideoDisplay>(mm2px(Vec(23.5, 8.008)));
		display->module = module;
		display->mw = this;
		display->real_width = mm2px(112.94);
		display->real_height = mm2px(112.94);
		addChild(display);

		addChild(createParam<VCVButton>(mm2px(Vec(6, 102)), module, VideoOut::CLEAR_PARAM));
		addChild(createParam<Trimpot>(mm2px(Vec(6, 113)), module, VideoOut::RESOLUTION_PARAM));
	}
};

// need std::vector because rack::math::Vec only does 2d
// based on https://www.codespeedy.com/hsv-to-rgb-in-cpp/
std::vector<int> hsv_to_rgb (int h_in, int s_in, int v_in) {
	float H = h_in * (360.0/256.0);
	float S = s_in / 256.0;
	float V = v_in / 256.0;
	float C = S*V;
	float X = C*(1-abs(fmod(H/60.0, 2)-1));
	float m = V-C;
	float r,g,b;
	if(H >= 0 && H < 60){
		r = C,g = X,b = 0;
	}
	else if(H >= 60 && H < 120){
		r = X,g = C,b = 0;
	}
	else if(H >= 120 && H < 180){
		r = 0,g = C,b = X;
	}
	else if(H >= 180 && H < 240){
		r = 0,g = X,b = C;
	}
	else if(H >= 240 && H < 300){
		r = X,g = 0,b = C;
	}
	else{
		r = C,g = 0,b = X;
	}
	int R = ((r+m)*255);
	int G = ((g+m)*255);
	int B = ((b+m)*255);
	return std::vector<int>({R,G,B});
}

Model* modelVideoOut = createModel<VideoOut, VideoOutWidget>("VideoOut");
