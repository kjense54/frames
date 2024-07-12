#include "../include/glad/glad.h"
#include <GLFW/glfw3.h>
#include <iostream>
#include <chrono>
#include "FrameDecoder.h"
#include "Shader.h"

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
	// set callback for keypresses
	glfwSetKeyCallback(window, key_callback);

  // init GLAD
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    std::cerr << "Failed to initialize glad" << std::endl;
  }
  // set rendering viewport
  glViewport(0, 0, 1920, 1080);

	// set up shader program
	GLuint program = Shader::init();

  //Video Decoding
  const char* input_file = "resources/one_piece_test.mp4";
  FrameDecoder decoder(input_file);
	bool eoframes = false;

	// timer for frame hold
	auto start_time = std::chrono::steady_clock::now();
	std::chrono::duration<double> duration_time = std::chrono::duration<double>(frame_hold + 1.0);

 // render loop
  while (!glfwWindowShouldClose(window)) {
    processInput(window);
    glClear(GL_COLOR_BUFFER_BIT);


		if (!eoframes && !paused && duration_time >= std::chrono::seconds(frame_hold)) {
			Frame frame = decoder.next();
			for (int i = 1; i < frame_skip; i++) {
				frame = decoder.next();
			}
			Shader::Texture texture = Shader::imgToTexture(frame);
			Shader::updateTexture(program, texture);

			eoframes = frame.eof;
			start_time = std::chrono::steady_clock::now();
		}

		auto end_time = std::chrono::steady_clock::now();
		duration_time = end_time - start_time;
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);


    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glfwDestroyWindow(window);
  glfwTerminate();

  return 0;
}

