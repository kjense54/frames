#include "../include/glad/glad.h"
#include <GLFW/glfw3.h>
#include <iostream>
#include "FrameDecoder.h"
#include "Shader.h"

void processInput(GLFWwindow* window) {
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    glfwSetWindowShouldClose(window, true);
}
  

int main() {
  if (!glfwInit()) {
    std::cerr << "Failed to init GLFW" << std::endl;
    return -1;
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);

  // create window and context
  GLFWwindow* window = glfwCreateWindow(1920, 1080, "Frame by Frame", NULL, NULL);
  if (!window) {
    std::cerr << "Failed to create window" << std::endl;
    glfwTerminate();
    return -1;
  }

  glfwSetWindowPos(window, 100, 2000);
  glfwMakeContextCurrent(window);

  // init GLAD
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    std::cerr << "Failed to initialize glad" << std::endl;
  }

  // set rendering viewport
  glViewport(0, 0, 1920, 1080);

  // compile shaders

	// set up shader program
	GLuint program = Shader::init();

  //Video Decoding
  const char* input_file = "resources/one_piece_test.mp4";
  FrameDecoder decoder(input_file);
	bool eoframes = false;

 // render loop
  while (!glfwWindowShouldClose(window)) {
    processInput(window);
    glClear(GL_COLOR_BUFFER_BIT);

		if (!eoframes) {
			Frame frame = decoder.next();
			Shader::Texture texture = Shader::imgToTexture(frame);
			Shader::updateTexture(program, texture);

			eoframes = frame.eof;
		}

    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glfwDestroyWindow(window);
  glfwTerminate();

  return 0;
}

