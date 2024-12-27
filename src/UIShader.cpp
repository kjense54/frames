#include "UIShader.h"
#include "iostream" //TODO: Remove after debugging

GLuint UIShader::compileShader(GLenum type, const char* source) {
	GLuint shader = glCreateShader(type);
	glShaderSource(shader, 1, &source, NULL);
	glCompileShader(shader);

	GLint success;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if (!success) {
		char infoLog[512];
		glGetShaderInfoLog(shader, 512, NULL, infoLog);
		std::cerr << "ERROR: ui Shader compilation failed\n" << infoLog << std::endl;
		return 0;
	}

	return shader;
}

GLuint UIShader::compileProgram(GLuint vShader, GLuint fShader) {
	GLuint program = glCreateProgram();
	glAttachShader(program, vShader);
	glAttachShader(program, fShader);
	glLinkProgram(program);

	GLint success;
	glGetProgramiv(program, GL_LINK_STATUS, &success);
	if (!success) {
		char infoLog[512];
		glGetProgramInfoLog(program, 512, NULL, infoLog);
		std::cerr << "ERROR: ui Program linking failed\n" << infoLog << std::endl;
		return 0;
	}

	glDeleteShader(vShader);
	glDeleteShader(fShader);

	return program;
}

std::map<std::string, UIShader::UIButton> uiButtons; // store by name

void UIShader::createUIButtons() {
	std::vector<float> playButtonVertices = {
		-0.034f, -0.9f,					1.0f, 1.0f, 1.0f,
		 0.02f, -0.95f,					1.0f, 1.0f, 1.0f,
		-0.034f, -1.0f,					1.0f, 1.0f, 1.0f,
	};
	std::vector<float> pauseButtonVertices = {
		-0.035f, -0.91f, 					1.0f, 1.0f, 1.0f,
		0.015f, -0.91f, 					1.0f, 1.0f, 1.0f,
		0.015f, -0.99f,					1.0f, 1.0f, 1.0f,
		-0.035f, -0.99f,					1.0f, 1.0f, 1.0f,	
	};

	// play button
	UIShader::UIButton playButton;
	playButton.vertices = playButtonVertices;
	playButton.vertexCount = 3;
	uiButtons["play"] = playButton;

	UIShader::UIButton pauseButton;
	pauseButton.vertices = pauseButtonVertices;
	pauseButton.vertexCount = 4;
	uiButtons["pause"] = pauseButton;
}

void UIShader::createUIButtonBuffers() {
	for (auto& buttonMap : uiButtons) {
		UIButton& button = buttonMap.second;

		GLuint VBO, VAO;

		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);

		glBindVertexArray(VAO);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, button.vertices.size() * sizeof(float), button.vertices.data(), GL_STATIC_DRAW);

		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);

		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(2 * sizeof(float)));
		glEnableVertexAttribArray(1); 

		button.VAO = VAO;
		button.VBO = VBO;
	}
}

void UIShader::drawUIButtons() {
	for (const auto& buttonMap : uiButtons) {
		const UIButton button = buttonMap.second;
	
		if (button.isVisible) {
			glBindVertexArray(button.VAO);
			glDrawArrays(GL_TRIANGLE_FAN, 0, button.vertexCount);
		}
	}
}

void UIShader::toggleUIButtonVisibility(const std::string& buttonName, bool visibility) {
	if (uiButtons.find(buttonName) != uiButtons.end()) {
		uiButtons[buttonName].isVisible = visibility;
	}
}

GLuint UIShader::init() {
	GLuint vertexShader = UIShader::compileShader(GL_VERTEX_SHADER, UIShader::vShaderSource);
	GLuint fragmentShader = UIShader::compileShader(GL_FRAGMENT_SHADER, UIShader::fShaderSource);
	GLuint program = UIShader::compileProgram(vertexShader, fragmentShader);

	UIShader::createUIButtons();
	UIShader::createUIButtonBuffers();
	
	return program;
}
