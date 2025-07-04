#include "plugin.hpp"


Plugin* pluginInstance;


void init(Plugin* p) {
	pluginInstance = p;

	// Add modules here
	// p->addModel(modelMyModule);

	p->addModel(modelVideoOut);
	p->addModel(modelImageIn);
	p->addModel(modelPathGenerator);
	p->addModel(modelCoordinateFolder);
	p->addModel(modelAffineTransform);
	p->addModel(modelPreciseDelay);

	// Any other plugin initialization may go here.
	// As an alternative, consider lazy-loading assets and lookup tables when your module is created to reduce startup times of Rack.
}
