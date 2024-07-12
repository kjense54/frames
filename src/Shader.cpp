#include "Shader.h"

GLuint Shader::compileShader(GLenum type, const char* source) {
	GLuint shader = glCreateShader(type);
	glShaderSource(shader, 1, &source, NULL);
	glCompileShader(shader);
	return shader;
}
GLuint Shader::compileProgram(GLuint vShader, GLuint fShader) {
	GLuint program = glCreateProgram();
	glAttachShader(program, vShader);
	glAttachShader(program, fShader);
	glLinkProgram(program);
	glUseProgram(program);

	glDeleteShader(vShader);
	glDeleteShader(fShader);

	return program;
}
GLuint Shader::init() {
	GLuint vertexShader = Shader::compileShader(GL_VERTEX_SHADER, Shader::vShaderSource);
	GLuint fragmentShader = Shader::compileShader(GL_FRAGMENT_SHADER, Shader::fShaderSource);
	GLuint program = Shader::compileProgram(vertexShader, fragmentShader); 
	return program;
}
Shader::Texture Shader::imgToTexture(const Frame &frame) {
  // create texture for each plane
  GLuint yTexture, uTexture, vTexture;
  glGenTextures(1, &yTexture);
  glGenTextures(1, &uTexture);
  glGenTextures(1, &vTexture);

  // Y plane
  glBindTexture(GL_TEXTURE_2D, yTexture);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, frame.stride, frame.height, 0, GL_RED, GL_UNSIGNED_BYTE, frame.yData.data());
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,  GL_CLAMP_TO_BORDER);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  // U plane
  glBindTexture(GL_TEXTURE_2D, uTexture);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, frame.stride / 2, frame.height / 2, 0, GL_RED, GL_UNSIGNED_BYTE, frame.uData.data());
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  // V plane
  glBindTexture(GL_TEXTURE_2D, vTexture);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, frame.stride / 2, frame.height / 2, 0, GL_RED, GL_UNSIGNED_BYTE, frame.vData.data());
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  return { yTexture, uTexture, vTexture };
}

void Shader::updateTexture(GLuint program, Shader::Texture &texture) {
	// bind textures
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture.yTex);
	glUniform1i(glGetUniformLocation(program, "yTexture"), 0);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, texture.uTex);
	glUniform1i(glGetUniformLocation(program, "uTexture"), 1);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, texture.vTex);
	glUniform1i(glGetUniformLocation(program, "vTexture"), 2);
}
