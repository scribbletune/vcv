#pragma once
#include <rack.hpp>

using namespace rack;

// Declare the Plugin, defined in plugin.cpp
extern Plugin *pluginInstance;

// Declare each Model, defined in each module source file
extern Model *modelBrahmaQuantizer;
extern Model *modelInduQuantizer;
extern Model *modelNetraQuantizer;
extern Model *modelAgniQuantizer;
extern Model *modelBanaQuantizer;
extern Model *modelRutuQuantizer;
