#pragma once

#include <imgui.h>

#include "../utils/fps.h"


namespace Lengine {
	class PerformancePanel {
	public:
		PerformancePanel();
		
		void OnImGuiRender();
		float getDeltaTime() const { return deltaTime; }

	private:
		int targetFPS = 144; // my 144 Hz monitor
		float deltaTime;
		bool limitFPS = true;
		float smoothedFPS = 0.0f;
		float smoothedMs = 0.0f;

	};
}