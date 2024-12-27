#ifndef FRAMESHADER_H
#define FRAMESHADER_H

#include "../include/glad/glad.h"
#include <GLFW/glfw3.h>
#include "Frame.h"

namespace FrameShader {
// shaders
	static const char* vShaderSource = R"(
		#version 330 
		const vec2 vertices[4] = vec2[4](
			vec2(-1.0, -0.9), 	// bottom left
			vec2(1.0, -0.9),	// bottom right
			vec2(1.0, 1.0),		// top right
			vec2(-1.0, 1.0) // top left
		);
		out vec2 position;
		void main() {
			position = vertices[gl_VertexID];
			gl_Position = vec4(position , 0.0, 1.0);
		}
	)";

	static const char* fShaderSource = R"(
		#version 330 
		in vec2 position;
		out vec4 FragColor;

		uniform sampler2D yTexture;
		uniform sampler2D uTexture;
		uniform sampler2D vTexture;
		
		void main() {
			vec2 flipped = (position + 1.0) / 2.0;
			flipped.y = 1.0 - flipped.y;

			float y = texture(yTexture, flipped).r;
			float u = texture(uTexture, flipped).r - 0.5;
			float v = texture(vTexture, flipped).r - 0.5;

			float r = y + 1.13983f * v;
			float g = y - 0.39465f * u - 0.58060f * v;
			float b = y + 2.03211f * u;

			FragColor = vec4(r, g, b, 1.0);
		}
	)";
	
	GLuint compileShader(GLenum type, const char* source);
	GLuint compileProgram(GLuint vShader, GLuint fShader); 
	GLuint init();

	// TEXTURE
		struct Texture {
			GLuint yTex;
			GLuint uTex;
			GLuint vTex;
		};

	Texture imgToTexture(const Frame &frame);
	void updateTexture(GLuint program, Texture &texture);
}
#endif
