#include "../include/glad/glad.h"
#include <GLFW/glfw3.h>
#include <iostream>
#include <chrono>
#include "FrameDecoder.h"
#include "FrameShader.h"
#include "UIShader.h"

const int INITIAL_WIDTH = 1920;
const int INITIAL_HEIGHT = 1200;
const int MAX_FRAME_SKIP = 30;
const int FRAME_HOLD_INCREMENT = 5;
struct {
	bool paused = false;
	bool justChanged = false;
} videoState;
bool adjust_skip = false;
bool adjust_hold = false;
int frame_skip = 5;
int frame_hold = 30;

//--------------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window) {
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    glfwSetWindowShouldClose(window, true);
}
void window_size_callback(GLFWwindow* window, int width, int height) {
	// recalculate and set ui button xy positions
	std::cout << "window resized to: " << width << ", " << height << std::endl;
	glViewport(0, 0, width, height);
}

// TODO: add other UI elements
// TODO: calculate xy pos of element in window
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
	// other UI buttons to add:
	// two numInput boxes w/ increment arrows on either side, like:
	// < [ 15 ] > 
	// [ videofile.mp4 ] selection box
	// button by: getwindowwidth, getwindowheight, getbuttonx, getbuttony, convert w/ math func for pos.  
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
		double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);
		int width, height;
		glfwGetWindowSize(window, &width, &height);
		int button = UIShader::checkMouseOver(xpos, ypos, width, height);
		switch (button) {
			case UIShader::UIButton::NAME::FILE:
				std::cout << "clicked on file selection button" << std::endl;
			break;
			default: 
			videoState.paused = !videoState.paused;
			videoState.justChanged = true;
		}
	}
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (action == GLFW_PRESS) {
		if (key == GLFW_KEY_SPACE) {
			videoState.paused = !videoState.paused;
			videoState.justChanged = true;
			adjust_hold = false;
			adjust_skip = false;
		}
		if (videoState.paused) {
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
  
//----------------------------------------------------------------------------------------------------------
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

  glfwSetWindowPos(window, 100, 100);
  glfwMakeContextCurrent(window);
	glfwSetWindowAspectRatio(window, 16, 10);
	glfwSetWindowSizeCallback(window, window_size_callback);
	glfwSetKeyCallback(window, key_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);

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
	std::cout << "I: Enter frame skip adjustment mode. Up and Down arrows will increment by 1. Max skip is 30 frames." << std::endl;
	std::cout << "H: Enter frame hold adjustment mode. Up and Down arrows will increment by 5." << std::endl;

	std::chrono::steady_clock::time_point start_time;
	std::chrono::steady_clock::time_point start_pause_time;
	std::chrono::duration<double> pause_duration; 
	std::chrono::duration<double> duration_time = std::chrono::duration<double>(frame_hold + 1.0);

 //-------------------------------------------------------------------------------------------------------------------------------
  while (!glfwWindowShouldClose(window)) {
    processInput(window);
    glClear(GL_COLOR_BUFFER_BIT);
		glDisable(GL_DEPTH_TEST);

		// FRAME 
		glUseProgram(frameProgram);
		GLenum err;
		if ((err = glGetError()) != GL_NO_ERROR) {
			std::cout << "OpenGl glUseProgram(frameProgram) error: " << err << std::endl;
		}

		if (videoState.paused) {
			if (videoState.justChanged) {														
				std::cout << "paused" << std::endl;
				videoState.justChanged = false;
				start_pause_time = std::chrono::steady_clock::now();
			}
		} else {
			if (videoState.justChanged) {														
				std::cout << "playing" << std::endl;
				videoState.justChanged = false;
				pause_duration += std::chrono::steady_clock::now() - start_pause_time;
			}
			
			duration_time = std::chrono::steady_clock::now() - start_time - pause_duration;

			// if it is time to change the frame (x seconds have passed)
			if (!eoframes && duration_time >= std::chrono::seconds(frame_hold)) { 
				start_time = std::chrono::steady_clock::now();
				pause_duration = std::chrono::duration<double>::zero();

				Frame frame = decoder.next();
				for (int i = 1; i < frame_skip; i++) {
					frame = decoder.next();
				}
				FrameShader::Texture texture = FrameShader::imgToTexture(frame);
				FrameShader::updateTexture(frameProgram, texture);
				eoframes = frame.eof;
			}																																									 
		}
		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

		// UI 
		glUseProgram(uiProgram);
		if ((err = glGetError()) != GL_NO_ERROR) {
			std::cout << "OpenGl glUseProgram(uiProgram) error: " << err << std::endl;
		}
		UIShader::toggleUIButtonVisibility(UIShader::UIButton::NAME::PLAY, videoState.paused);
		UIShader::toggleUIButtonVisibility(UIShader::UIButton::NAME::PAUSE, !videoState.paused);
		UIShader::drawUIButtons();

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glfwDestroyWindow(window);
  glfwTerminate();

  return 0;
}

