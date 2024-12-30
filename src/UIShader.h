#ifndef UISHADER_H
#define UISHADER_H

#include "../include/glad/glad.h"
#include <GLFW/glfw3.h>
#include <map>
#include <string>
#include <vector>

namespace UIShader {
// shaders
	static const char* vShaderSource = R"(
		#version 330
		in vec2 aPos;
		in vec3 aColor;
		out vec3 vertexColor;

		void main() {
			gl_Position = vec4(aPos, 1.0, 1.0);
			vertexColor = aColor;	
		}
	)";

	static const char* fShaderSource = R"(
		#version 330
		in vec3 vertexColor;
		out vec4 FragColor;

		void main() {
			FragColor = vec4(vertexColor, 1.0);	
		}
	)";

	struct UIButton {
		enum NAME {
			PAUSE = 1,
			PLAY,
			FILE,
			INC_HOLD,
			DEC_HOLD,
			INC_SKIP,
			DEC_SKIP
		};
		GLuint VAO;
		GLuint VBO;
		std::vector<float> vertices;
		size_t vertexCount;
		float x;
		float y;
		float x2;
		float y2;
		bool isVisible;

		UIButton() : VAO(0), VBO(0), vertexCount(0), x(0), y(0), x2(0), y2(0), isVisible(true) {}
	};
	
	void createUIButtons();
	void createUIButtonBuffers();
	void drawUIButtons();
	void toggleUIButtonVisibility(const UIShader::UIButton::NAME buttonName, bool visibility);
	int checkMouseOver(double xpos, double ypos, int width, int height);
	GLuint compileShader(GLenum type, const char* source);
	GLuint compileProgram(GLuint vShader, GLuint fShader);
	GLuint init();
}

#endif
