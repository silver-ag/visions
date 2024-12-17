#include "plugin.hpp"
#include "osdialog.h"
#include "widgets/switches.hpp"
 
#include "../dep/lodepng.h"

std::vector<int> rgb_to_hsv(int r, int g, int b);

struct ImageIn : Module {
	enum ParamId {
		RGB_HSV_PARAM,
		LOAD_PARAM,
		PARAMS_LEN
	};
	enum InputId {
		POSITION_INPUT,
		X_INPUT,
		Y_INPUT,
		INPUTS_LEN
	};
	enum OutputId {
		COLOUR_OUTPUT,
		R_H_OUTPUT,
		G_S_OUTPUT,
		B_V_OUTPUT,
		OUTPUTS_LEN
	};
	enum LightId {
		LIGHTS_LEN
	};

	unsigned char* image_data = {};
	unsigned int width = 0;
	unsigned int height = 0;
	int image = 0;
	char* filename = "";

	ImageIn() {
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
		configSwitch(RGB_HSV_PARAM, 0.f, 1.f, 0.f, "RGB or HSV", {"RGB", "HSV"});
		configButton(LOAD_PARAM, "Load");
		configInput(POSITION_INPUT, "Polyphonic position");
		configOutput(COLOUR_OUTPUT, "Polyphonic Colour");
		configInput(X_INPUT, "X position");
		configInput(Y_INPUT, "Y position");
		configOutput(R_H_OUTPUT, "Red or Hue");
		configOutput(G_S_OUTPUT, "Green or Saturation");
		configOutput(B_V_OUTPUT, "Blue or Value");
	}

	void process(const ProcessArgs& args) override {

		if (params[LOAD_PARAM].getValue() == 1) {
			get_filename();
		}
		float x_voltage = 0;
                float y_voltage = 0;
                if (inputs[POSITION_INPUT].isConnected()) {
                        x_voltage = inputs[POSITION_INPUT].getPolyVoltage(0);
                        y_voltage = inputs[POSITION_INPUT].getPolyVoltage(1);
                } else {
                        x_voltage = inputs[X_INPUT].getVoltage();
                        y_voltage = inputs[Y_INPUT].getVoltage();
                }
                int x = int(clamp((x_voltage / 10) + 0.5f, 0.0, 0.999) * width);
                int y = int(clamp((y_voltage / 10) + 0.5f, 0.0, 0.999) * height);

		if (image > 0) {

			int offset = 4 * (x + (y * height));
			float rh_voltage = 0;
			float gs_voltage = 0;
			float bv_voltage = 0;
			if (params[RGB_HSV_PARAM].getValue() == 0) {
				rh_voltage = (10 * (float(image_data[offset]) / 256)) - 5;
				gs_voltage = (10 * (float(image_data[offset+1]) / 256)) - 5;
				bv_voltage = (10 * (float(image_data[offset+2]) / 256)) - 5;
			} else {
				std::vector<int> hsv = rgb_to_hsv(image_data[offset], image_data[offset+1], image_data[offset+2]);
				rh_voltage = (10 * (float(hsv[0]) / 256)) - 5;
                                gs_voltage = (10 * (float(hsv[1]) / 256)) - 5;
                                bv_voltage = (10 * (float(hsv[2]) / 256)) - 5;
			}

			outputs[R_H_OUTPUT].setVoltage(rh_voltage);
			outputs[G_S_OUTPUT].setVoltage(gs_voltage);
			outputs[B_V_OUTPUT].setVoltage(bv_voltage);
			outputs[COLOUR_OUTPUT].setVoltage(rh_voltage, 0);
			outputs[COLOUR_OUTPUT].setVoltage(gs_voltage, 1);
			outputs[COLOUR_OUTPUT].setVoltage(bv_voltage, 2);
			outputs[COLOUR_OUTPUT].setChannels(3);

		}

	}

	void get_filename() {
		filename = osdialog_file(OSDIALOG_OPEN, "", "", osdialog_filters_parse("png:png"));
		if (filename) {
			load_file(filename);
		}
	}

	void load_file(char* filename) {
		std::vector<unsigned char> local_data; //the raw pixels
		//decode
		unsigned int error = lodepng::decode(local_data, width, height, filename);
		if(error) { fprintf(stderr, "decoder error %u: %s\n", error, lodepng_error_text(error)); }

		// copy over to image_data directly, only reason is because we can't cast (vector<unsigned char>) to (unsigned char*)
		image_data = new unsigned char[4 * width * height];
		for (unsigned int x = 0; x < width; x++) {
			for (unsigned int y = 0; y < height; y++) {
				int offset = 4 * (x + (y * height));
				image_data[offset] = local_data[offset];
				image_data[offset+1] = local_data[offset+1];
				image_data[offset+2] = local_data[offset+2];
				image_data[offset+3] = 255;
			}
		}

		image = -1;
	}
};

struct ImageDisplay : TransparentWidget {
	ImageIn* module = nullptr;
	float real_width = -1;
	float real_height = -1;
	void drawLayer(const DrawArgs& args, int layer) override {
		NVGcontext* vg = args.vg;
		if (module && (layer == 1)) {
			if (module->image == -1) {
				module->image = nvgCreateImageRGBA(vg, module->width, module->height, 0, module->image_data);
				//module->image = nvgCreateImage(vg, module->filename, 0);
			}
			if (module->image > 0) { // image can be 0 as well as -1
				NVGpaint paint = nvgImagePattern(vg, 0, 0, module->width, module->height, 0, module->image, 1);
				nvgBeginPath(vg);
				nvgScale(vg, real_width/module->width, real_height/module->height);
				nvgRect(vg, 0, 0, module->width, module->height);

				nvgFillPaint(vg, paint);
				nvgFill(vg);
			}
		}
	}
};


std::vector<int> rgb_to_hsv(int r_in, int g_in, int b_in) 
{ 
  
    // R, G, B values are divided by 255 
    // to change the range from 0..255 to 0..1 
    float r = r_in / 255.0; 
    float g = g_in / 255.0; 
    float b = b_in / 255.0; 

  
    // h, s, v = hue, saturation, value 
    float cmax = std::max(r, std::max(g, b)); // maximum of r, g, b 
    float cmin = std::min(r, std::min(g, b)); // minimum of r, g, b 
    float diff = cmax - cmin; // diff of cmax and cmin. 
    float h = -1, s = -1; 
  
    // if cmax and cmax are equal then h = 0 
    if (cmax == cmin) 
        h = 0; 
  
    // if cmax equal r then compute h 
    else if (cmax == r) 
        h = fmod(60 * ((g - b) / diff) + 360, 360); 
  
    // if cmax equal g then compute h 
    else if (cmax == g) 
        h = fmod(60 * ((b - r) / diff) + 120, 360); 
  
    // if cmax equal b then compute h 
    else if (cmax == b) 
        h = fmod(60 * ((r - g) / diff) + 240, 360); 
  
    // if cmax equal zero 
    if (cmax == 0) 
        s = 0; 
    else
        s = (diff / cmax) * 100; 
  
    // compute v 
    float v = cmax * 100; 

	return std::vector<int>({int(h),int(3*s),int(3*v)}); // i don't know why these *3s are needed, but they get the output much closer to correct
} 
  

struct ImageInWidget : ModuleWidget {
	ImageInWidget(ImageIn* module) {
		setModule(module);
		setPanel(createPanel(asset::plugin(pluginInstance, "res/ImageIn.svg")));

		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		addChild(createParam<HorizontalSwitch>(mm2px(Vec(135, 50)), module, ImageIn::RGB_HSV_PARAM));
		addChild(createParam<VCVButton>(mm2px(Vec(6.5, 69)), module, ImageIn::LOAD_PARAM));

		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(6.957, 14.232)), module, ImageIn::POSITION_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(6.936, 26.109)), module, ImageIn::X_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(6.936, 34.876)), module, ImageIn::Y_INPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(150.74, 14.103)), module, ImageIn::COLOUR_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(150.87, 26.772)), module, ImageIn::R_H_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(150.727, 35.575)), module, ImageIn::G_S_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(150.733, 44.299)), module, ImageIn::B_V_OUTPUT));


		// mm2px(Vec(112.94, 112.94))
		ImageDisplay* display = createWidget<ImageDisplay>(mm2px(Vec(20.567, 8.008)));
                display->module = module;
                display->real_width = mm2px(112.94);
                display->real_height = mm2px(112.94);
                addChild(display);
	}
};


Model* modelImageIn = createModel<ImageIn, ImageInWidget>("ImageIn");
