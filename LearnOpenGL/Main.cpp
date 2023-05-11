#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <iostream>
#include "Shader.h"
#include "stb_image.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

const unsigned int width = 800;
const unsigned int height = 600;
GLFWwindow* window;
float mixValue = 0.2;
float texCoordScale = 1.0;
float texCoordScrollX = 0.0;
float texCoordScrollY = 0.0;
const float modificationAmount = 0.1;



void FramebufferSizeCallback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}


void InitWindow()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	window = glfwCreateWindow(width, height, "LearnOpenGL", NULL, NULL);
	if (window == NULL) {
		std::cout << "Failed to create GLFW window!" << std::endl;
		glfwTerminate();
	}
	glfwMakeContextCurrent(window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to initialize GLAD!" << std::endl;
	}

	glViewport(0, 0, width, height);

}

unsigned int CreateVAO(float vertices[], int verticesSize, int indices[], int indicesSize) {
	unsigned int VBO, VAO, EBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, verticesSize, vertices, GL_STATIC_DRAW);
	//indices
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicesSize, indices, GL_STATIC_DRAW);
	// position
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// color
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	// texture
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	// unbind VAO/VBO
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	return VAO;
}

unsigned int LoadTexture(const char * textureLocation, bool rgba, bool flip);

void HandleTextureEdits(int key, int action);

unsigned int LoadTexture(const char * textureLocation, bool rgba, bool flip)
{
	unsigned int texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	// set the texture wrapping/filtering options (on the currently bound texture object)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// load and generate the texture
	int width, height, nrChannels;

	if (flip) {
		stbi_set_flip_vertically_on_load(true);
	}

	unsigned char* data = stbi_load(textureLocation, &width, &height, &nrChannels, 0);
	if (data)
	{
		if (rgba) {
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		}
		else {
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		}

		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data);
	return texture;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	HandleTextureEdits(key, action);

	// Close window.
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}
		
}

void HandleTextureEdits(int key, int action)
{
	if (key == GLFW_KEY_PERIOD && (action == GLFW_REPEAT || action == GLFW_PRESS)) {
		mixValue += modificationAmount;
		if (mixValue > 1.0) {
			mixValue = 1.0;
		}
	}
	if (key == GLFW_KEY_COMMA && (action == GLFW_REPEAT || action == GLFW_PRESS)) {
		mixValue -= modificationAmount;
		if (mixValue < 0.0) {
			mixValue = 0.0;
		}
	}

	if (key == GLFW_KEY_EQUAL && (action == GLFW_REPEAT || action == GLFW_PRESS)) {
		texCoordScale -= modificationAmount;
	}

	if (key == GLFW_KEY_MINUS && (action == GLFW_REPEAT || action == GLFW_PRESS)) {
		texCoordScale += modificationAmount;
	}

	if (key == GLFW_KEY_RIGHT && (action == GLFW_REPEAT || action == GLFW_PRESS)) {
		texCoordScrollX -= modificationAmount;
	}

	if (key == GLFW_KEY_LEFT && (action == GLFW_REPEAT || action == GLFW_PRESS)) {
		texCoordScrollX += modificationAmount;
	}

	if (key == GLFW_KEY_UP && (action == GLFW_REPEAT || action == GLFW_PRESS)) {
		texCoordScrollY -= modificationAmount;
	}

	if (key == GLFW_KEY_DOWN && (action == GLFW_REPEAT || action == GLFW_PRESS)) {
		texCoordScrollY += modificationAmount;
	}
}

int main() {

	InitWindow();
	glfwSetKeyCallback(window, key_callback);
	glfwSetFramebufferSizeCallback(window, FramebufferSizeCallback);

	
	Shader ourShader("VertexShader.vert", "FragmentShader.frag"); 

	ourShader.use(); // don't forget to activate the shader before setting uniforms!  

	unsigned int texture1 = LoadTexture("container.jpg", false, false);
	ourShader.setInt("texture1", 0); 

	unsigned int texture2 = LoadTexture("awesomeface.png", true, true);
	ourShader.setInt("texture2", 1); 

	// bind textures on corresponding texture units
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture1);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, texture2);

	float vertices[] = {
		// positions          // colors           // texture coords
		 0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,   // top right
		 0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,   // bottom right
		-0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,   // bottom left
		-0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f    // top left 
	};

	int indices[] = {  // CCW Winding Order
		3,2,1, // First triangle
		1,0,3  // Second triangle
	};


	unsigned int squareVAO = CreateVAO(vertices, sizeof(vertices), indices, sizeof(indices));

	while (!glfwWindowShouldClose(window)) {
		// Rendering
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		ourShader.setFloat("mixValue", mixValue); 
		ourShader.setFloat("texCoordScale", texCoordScale);
		ourShader.setFloat("texCoordScrollX", texCoordScrollX);
		ourShader.setFloat("texCoordScrollY", texCoordScrollY);

		glm::mat4 squareTransformation = glm::mat4(1.0f);
		squareTransformation = glm::translate(squareTransformation, glm::vec3(.5, 0, 0));
		squareTransformation = glm::rotate(squareTransformation, (float)glfwGetTime(), glm::vec3(0.0, 0.0, 1.0));
		squareTransformation = glm::scale(squareTransformation, glm::vec3(0.5, 0.5, 0.5));

		unsigned int transformLoc = glGetUniformLocation(ourShader.ID, "transform");
		glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(squareTransformation));

		glBindVertexArray(squareVAO);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);


		glm::mat4 squareTransformation2 = glm::mat4(1.0f);
		squareTransformation2 = glm::translate(squareTransformation2, glm::vec3(-.5f, 0, 0));
		squareTransformation2 = glm::rotate(squareTransformation2, -(float)glfwGetTime(), glm::vec3(0.0, 0.0, 1.0));
		squareTransformation2 = glm::scale(squareTransformation2, glm::vec3(0.5, 0.5, 0.5));

		glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(squareTransformation2));

		glBindVertexArray(squareVAO);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		// Check and call events and swap the buffers
		glfwSwapBuffers(window);
		glfwPollEvents();
	}


	// Cleanup
	glfwTerminate();

	return 0;

}



