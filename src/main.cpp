#include "../include/glad/glad.h"
#include <GLFW/glfw3.h>
#include <iostream>
#include "FrameDecoder.h"
#include "Texture.h"

void processInput(GLFWwindow* window) {
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    glfwSetWindowShouldClose(window, true);
}

// shaders
const char* vShaderSource = R"(
  #version 330 
  const vec2 vertices[4] = vec2[4](
    vec2(-1.0, -1.0), 	// bottom left
    vec2(1.0, -1.0),	// bottom right
    vec2(1.0, 1.0),		// top right
    vec2(-1.0, 1.0) // top left
  );
  out vec2 position;
  void main() {
    position = vertices[gl_VertexID];
    gl_Position = vec4(position , 0.0, 1.0);
  }
)";

const char* fShaderSource = R"(
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
GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vShaderSource, NULL);
    glCompileShader(vertexShader);

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fShaderSource, NULL);
    glCompileShader(fragmentShader);

    // set up shader program
    GLuint program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);
    glUseProgram(program);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

  //Video Decoding
  const char* input_file = "resources/one_piece_test.mp4";
  FrameDecoder decoder(input_file);

 // render loop
  glClearColor(0.0f, 0.4f, 0.4f, 1.0f);

  while (!glfwWindowShouldClose(window)) {
    processInput(window);
    glClear(GL_COLOR_BUFFER_BIT);

		/*TODO: playspeed function: play at default speed or specified fps*/
		Image image = decoder.next();
		Texture texture = imgToTexture(image);
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

    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glfwDestroyWindow(window);
  glfwTerminate();

  return 0;
}

