#include "rack.hpp"

struct HorizontalSwitch : app::SvgSwitch {
        HorizontalSwitch() {
            addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/HorizontalSwitch_0.svg")));
            addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/HorizontalSwitch_1.svg")));
        }
};
