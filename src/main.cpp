#include "../include/glad/glad.h"
#include <GLFW/glfw3.h>
#include <iostream>
#include <chrono>
#include "FrameDecoder.h"
#include "FrameShader.h"
#include "UIShader.h"

const int MAX_FRAME_SKIP = 30;
const int FRAME_HOLD_INCREMENT = 5;
bool paused = false;
bool adjust_skip = false;
bool adjust_hold = false;
int frame_skip = 5;
int frame_hold = 30;



void processInput(GLFWwindow* window) {
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    glfwSetWindowShouldClose(window, true);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (action == GLFW_PRESS) {
		if (key == GLFW_KEY_SPACE) {
			paused = !paused;
			adjust_hold = false;
			adjust_skip = false;
			std::cout << (paused ? "paused" : "playing") << std::endl;
		}
		if (paused) {
			switch (key) {
			case GLFW_KEY_I:
				adjust_hold = false;
				adjust_skip = true;
				break;
			case GLFW_KEY_H:
				adjust_hold = true;
				adjust_skip = false;
				break;
			case GLFW_KEY_UP:
				if (adjust_skip) {
					if (frame_skip < MAX_FRAME_SKIP) 
						frame_skip++;
				std::cout << "frame skip: " <<  frame_skip << std::endl;
				}
				if (adjust_hold) {
					frame_hold += FRAME_HOLD_INCREMENT;
					std::cout << "frame hold: " << frame_hold << std::endl;
				}
				break;
			case GLFW_KEY_DOWN:
				if (adjust_skip) {
					if (frame_skip > 1)
						frame_skip--;
					std::cout << "frame skip: " << frame_skip << std::endl;
				}
				if (adjust_hold) {
					if (frame_hold > FRAME_HOLD_INCREMENT)
						frame_hold -= FRAME_HOLD_INCREMENT;
					std::cout << "frame hold: " << frame_hold << std::endl;
				}
				break;
			}
		}
	}
}
  
const int INITIAL_WIDTH = 1920;
const int INITIAL_HEIGHT = 1080;

int main() {
  if (!glfwInit()) {
    std::cerr << "Failed to init GLFW" << std::endl;
    return -1;
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);

  // create window and context
  GLFWwindow* window = glfwCreateWindow(INITIAL_WIDTH, INITIAL_HEIGHT, "Frame by Frame", NULL, NULL);
  if (!window) {
    std::cerr << "Failed to create window" << std::endl;
    glfwTerminate();
    return -1;
  }

  glfwSetWindowPos(window, 100, 2000);
  glfwMakeContextCurrent(window);
	glfwSetKeyCallback(window, key_callback);

  // init GLAD
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    std::cerr << "Failed to initialize glad" << std::endl;
  }
  // set rendering viewport
  glViewport(0, 0, INITIAL_WIDTH, INITIAL_HEIGHT);

	// frame and ui programs
	GLuint frameProgram = FrameShader::init();
	GLuint uiProgram = UIShader::init();

  // Video Decoding
  const char* input_file = "resources/one_piece_test.mp4";
  FrameDecoder decoder(input_file);
	bool eoframes = false;

	// TODO: delete when gui done
	std::cout << "Press Spacebar to play/pause. Settings can be changed while paused." << std::endl;
	std::cout << "I: Enter frame skip adjustment mode. Up and Down arrows will increment by 1. Max skip is 30 frames." << std::endl;
	std::cout << "H: Enter frame hold adjustment mode. Up and Down arrows will increment by 5." << std::endl;

	// timer for frame hold
	auto start_time = std::chrono::steady_clock::now();
	std::chrono::duration<double> duration_time = std::chrono::duration<double>(frame_hold + 1.0);

 // RENDER -----------------------------------------------------------------------------------------------------------------------------
  while (!glfwWindowShouldClose(window)) {
    processInput(window);
    glClear(GL_COLOR_BUFFER_BIT);
		glDisable(GL_DEPTH_TEST);

		//TODO: change time to subtract timewhilePaused to not count it. 
		// FRAME 
		glUseProgram(frameProgram);
		GLenum err;
		if ((err = glGetError()) != GL_NO_ERROR) {
			std::cout << "OpenGl glUseProgram(frameProgram) error: " << err << std::endl;
		}
		if (!eoframes && !paused && duration_time >= std::chrono::seconds(frame_hold)) {
			Frame frame = decoder.next();
			for (int i = 1; i < frame_skip; i++) {
				frame = decoder.next();
			}
			FrameShader::Texture texture = FrameShader::imgToTexture(frame);
			FrameShader::updateTexture(frameProgram, texture);
			eoframes = frame.eof;
			start_time = std::chrono::steady_clock::now();
		}
		auto end_time = std::chrono::steady_clock::now();
		duration_time = end_time - start_time;

		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

		// UI 
		glUseProgram(uiProgram);
		if ((err = glGetError()) != GL_NO_ERROR) {
			std::cout << "OpenGl glUseProgram(uiProgram) error: " << err << std::endl;
		}
		UIShader::toggleUIButtonVisibility("play", paused);
		UIShader::toggleUIButtonVisibility("pause", !paused);
		UIShader::drawUIButtons();

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glfwDestroyWindow(window);
  glfwTerminate();

  return 0;
}

