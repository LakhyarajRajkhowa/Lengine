#pragma once

#include <GL/glew.h>
#include <cstdint>

#include "../core/paths.h"

#include "../graphics/geometry/FullScreenQuad.h"
#include "../graphics/frameBuffers/FrameBuffer.h"
#include "../graphics/opengl/GLSLProgram.h"

namespace Lengine {
	class GBuffer {
	public:
		GBuffer(const uint32_t width, const uint32_t height);
		~GBuffer();

		
		
    private:
        uint32_t width = 0, height = 0;

		GLuint gBuffer;
		GLuint gPosition, gNormal, gAlbedoSpec;
		unsigned int attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };

		GLSLProgram shaderLightingPass;
		FullscreenQuad fullscreenQuad;

	private:
		void Create();
		void configureShaders();
		void destroy();

	
		
	};
}