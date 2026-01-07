#pragma once

#include <iostream>
#include <fstream>

#include "../core/Errors.h"
#include "../core/paths.h"

#include "../external/json.hpp"

using json = nlohmann::json;

enum WindowMode {
	BORDERLESS = 0,
	INVISIBLE = 1,
	FULLSCREEN = 2
};

class EngineSettings {
public:
	std::string windowName = "Lengine";
	uint16_t  windowWidth = 1280;
	uint16_t  windowHeight = 720 ;
	WindowMode windowMode = WindowMode::BORDERLESS;
	uint8_t msaaSamples = 4;
	
	int HDR = 0;
	uint16_t resolution_X = 1280;
	uint16_t resolution_Y = 720;

	uint16_t shadowMapResolution = 1024;


	float cameraPosX = 0;
	float cameraPosY = 0;
	float cameraPosZ = 0;
	float cameraFov = 45;

	std::string gameFolderPath ;

	const bool loadSettings();
	const bool saveSettings();
};

