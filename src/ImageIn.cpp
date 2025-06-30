#include "plugin.hpp"
#include "osdialog.h"
#include "widgets/switches.hpp"
#include "stb_image.h"
#include "stb_image_write.h"

std::vector<int> rgb_to_hsv(int r, int g, int b);
float fold_into_range(float n, float a, float b);

struct ImageIn : Module {
	enum ParamId {
		RGB_HSV_PARAM,
		COLOUR_POLARITY_PARAM,
		XY_POLARITY_PARAM,
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
	int width = 0;
	int height = 0;
	int image = 0;
	char* filename;
	int x = 0;
	int y = 0;
	osdialog_filters* filename_filters = osdialog_filters_parse("Images:png,Png,PNG,jpg,Jpg,JPG,jpeg,Jpeg,JPEG,bmp,Bmp,BMP,gif,Gif,GIF,psd,Psd,PSD,hdr,Hdr,HDR,pic,Pic,PIC,pnm,Pnm,PNM"); // stbi supported filetypes (not necessarily all kinds though, sometimes just a subset)

	ImageIn() {
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
		configSwitch(RGB_HSV_PARAM, 0.f, 1.f, 0.f, "RGB or HSV", {"RGB", "HSV"});
		configSwitch(COLOUR_POLARITY_PARAM, 0.f, 1.f, 0.f, "Bi/Unipolar Colour", {"Bipolar", "Unipolar"});
		configSwitch(XY_POLARITY_PARAM, 0.f, 1.f, 0.f, "Bi/Unipolar Position", {"Bipolar", "Unipolar"});
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
		if (params[XY_POLARITY_PARAM].getValue() == 0) {
	                x = int(((fold_into_range(x_voltage,-5,5) / 10) + 0.5) * width);
        	        y = int(((fold_into_range(y_voltage,-5,5) / 10) + 0.5) * height);
		} else {
	                x = int((fold_into_range(x_voltage,0,10) / 10) * width);
        	        y = int((fold_into_range(y_voltage,0,10) / 10) * height);
		}

		if (image > 0) {

			int offset = ((4 * width) * y) + (4 * x);
			float rh_voltage = 0;
			float gs_voltage = 0;
			float bv_voltage = 0;
			if (params[RGB_HSV_PARAM].getValue() == 0) {
				rh_voltage = 10 * (float(image_data[offset]) / 256);
				gs_voltage = 10 * (float(image_data[offset+1]) / 256);
				bv_voltage = 10 * (float(image_data[offset+2]) / 256);
			} else {
				std::vector<int> hsv = rgb_to_hsv(image_data[offset], image_data[offset+1], image_data[offset+2]);
				rh_voltage = 10 * (float(hsv[0]) / 256);
                                gs_voltage = 10 * (float(hsv[1]) / 256);
                                bv_voltage = 10 * (float(hsv[2]) / 256);
			}
			if (params[COLOUR_POLARITY_PARAM].getValue() == 0) {
				rh_voltage -= 5;
				gs_voltage -= 5;
				bv_voltage -= 5;
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
		filename = osdialog_file(OSDIALOG_OPEN, NULL, NULL, filename_filters);
		if (filename) {
			load_file(filename);
		}
	}

	void load_file(char* fn) {
		if (FILE *file = fopen(fn, "r")) {
			(void)file; // does nothing, stops the compiler warning about variable being unused
			int channels = 0;
			image_data = stbi_load(fn, &width, &height, &channels, 4);
			image = -1;
		}
	}

	void onAdd(const AddEvent& e) override {
		std::string path = system::join(getPatchStorageDirectory(), "image.png");
		load_file((char*)path.c_str());
	}

	void onSave(const SaveEvent& e) override {
		std::string path = system::join(createPatchStorageDirectory(), "image.png");
		stbi_write_png(path.c_str(), width, height, 4, image_data, width * 4); // 4 is the amount of channels (inc. alpha)
	}
};

float fold_into_range(float n, float a, float b) {
	if (a > b) {
		float s = a;
		a = b;
		b = s;
	}
	float diff = b-a;
	while (n < a) {
		n += diff;
	}
	while (n > b) {
		n -= diff;
	}
	return n;
}

struct ImageDisplay : TransparentWidget {
	ImageIn* module = nullptr;
	float real_width = -1;
	float real_height = -1;
	void drawLayer(const DrawArgs& args, int layer) override {
		NVGcontext* vg = args.vg;
		if (module && (layer == 1)) {
			if (module->image == -1) {
				module->image = nvgCreateImageRGBA(vg, module->width, module->height, 0, module->image_data);
			}
			if (module->image > 0) { // image can be 0 as well as -1
				NVGpaint paint = nvgImagePattern(vg, 0, 0, module->width, module->height, 0, module->image, 1);
				nvgBeginPath(vg);
				nvgScale(vg, real_width/module->width, real_height/module->height);
				nvgRect(vg, 0, 0, module->width, module->height);
				nvgFillPaint(vg, paint);
				nvgFill(vg);
				nvgBeginPath(vg);
				nvgScale(vg, module->width/real_width, module->height/real_height); // back to unscaled
				nvgCircle(vg, module->x * real_width/module->width, module->y * real_height/module->height, 4);
				nvgFillColor(vg, nvgRGBA(255 - module->image_data[((4 * module->width) * module->y) + (4 * module->x)],
							 255 - module->image_data[((4 * module->width) * module->y) + (4 * module->x) + 1],
							 255 - module->image_data[((4 * module->width) * module->y) + (4 * module->x) + 2],
							 255));
				nvgFill(vg);
			}
			nvgClosePath(vg);
		}
	}
};


std::vector<int> rgb_to_hsv(int r_in, int g_in, int b_in) {
	float r = r_in / 255.0;
	float g = g_in / 255.0;
	float b = b_in / 255.0;

	float h = 0;
	float s = 0;
	float v = 0;

	float top = std::max(r,std::max(g,b));
	float bottom = std::min(r,std::min(g,b));
	float difference = top - bottom;

	if (difference == 0) {
		h = 0;
	} else if (r == top) {
		h = fmod((((1.0/6.0)*((g-b)/difference))+1.0), 1.0);
	} else if (g == top) {
		h = fmod((((1.0/6.0)*((b-r)/difference))+(1.0/3.0)), 1.0);
	} else { // b == top
		h = fmod((((1.0/6.0)*((r-g)/difference))+(2.0/3.0)), 1.0);
	}

	if (top == 0) {
		s = 0;
	} else {
		s = difference / top;
	}

	v = top;

	return std::vector<int>({int(h*256),int(s*256),int(v*256)});
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
		addChild(createParam<PolaritySwitch>(mm2px(Vec(135, 57)), module, ImageIn::COLOUR_POLARITY_PARAM));
		addChild(createParam<PolaritySwitch>(mm2px(Vec(8, 42)), module, ImageIn::XY_POLARITY_PARAM));
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
