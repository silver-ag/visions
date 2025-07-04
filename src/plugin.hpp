#pragma once
#include <rack.hpp>


using namespace rack;

// Declare the Plugin, defined in plugin.cpp
extern Plugin* pluginInstance;

// Declare each Model, defined in each module source file
// extern Model* modelMyModule;

extern Model* modelVideoOut;
extern Model* modelImageIn;
extern Model* modelPathGenerator;
extern Model* modelCoordinateFolder;
extern Model* modelAffineTransform;
extern Model* modelPreciseDelay;
