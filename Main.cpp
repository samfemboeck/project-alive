#include "bcpch.h"
#include "Shader.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

void onWindowResize(GLFWwindow* window, int width, int height);
void onMouseEvent(GLFWwindow* window, double xpos, double ypos);
void initGlfw();
void mainLoop();

GLFWwindow* window = nullptr;
Shader* shaderTexture = nullptr;
Shader* shaderFlatColor = nullptr;
unsigned int VBOQuad, VAOQuad, EBOQuad;
unsigned int VBOHex, VAOHex, EBOHexTris, EBOHexLines;
unsigned int texture;
glm::mat4 model, view, projection;
float width = 1600;
float height = 800;

int main()
{
	initGlfw();

	shaderTexture = new Shader("Shaders/texture.vert", "Shaders/texture.frag");
	shaderFlatColor = new Shader("Shaders/texture.vert", "Shaders/flatcolor.frag");

	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	int imgWidth, imgHeight, nrChannels;
	unsigned char *data = stbi_load("Textures/rock.jpg", &imgWidth, &imgHeight, &nrChannels, 0);
	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imgWidth, imgHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data);

	float verticesQuad[] = {
		0.5f,  0.5f, 0.0f, 1.0f, 1.0f,  // top right
		0.5f, -0.5f, 0.0f, 1.0f, 0.0f,  // bottom right
	   -0.5f, -0.5f, 0.0f, 0.0f, 0.0f,  // bottom left
	   -0.5f,  0.5f, 0.0f, 0.0f, 1.0f   // top left 
   };

	float verticesHex[] = {
		0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		1.0f, 0.5f, 0.0f,
		1.0f, -0.5f, 0.0f,
		0.0f, -1.0f, 0.0f,
		-1.0f, -0.5f, 0.0f,
		-1.0f, 0.5f, 0.0f
	};

	unsigned int indicesHexLines[] = {
		1, 2, 3, 4, 5, 6
	};
	
	unsigned int indicesHexTris[] = {
		0, 6, 1,
		0, 1, 2,
		0, 2, 3,
		0, 3, 4,
		0, 4, 5,
		0, 5, 6
	};

	unsigned int indicesQuad[] = {  // note that we start from 0!
		0, 1, 3,   // first triangle
		1, 2, 3    // second triangle
	};
	
	glGenBuffers(1, &VBOQuad);
	glGenBuffers(1, &VBOHex);
	glGenVertexArrays(1, &VAOQuad);
	glGenVertexArrays(1, &VAOHex);
	glGenBuffers(1, &EBOQuad);
	glGenBuffers(1, &EBOHexLines);
	glGenBuffers(1, &EBOHexTris);
	
	glBindVertexArray(VAOQuad);
	glBindBuffer(GL_ARRAY_BUFFER, VBOQuad);
	glBufferData(GL_ARRAY_BUFFER, sizeof(verticesQuad), verticesQuad, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOQuad);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indicesQuad), indicesQuad, GL_STATIC_DRAW);
	
	glBindVertexArray(VAOHex);
	glBindBuffer(GL_ARRAY_BUFFER, VBOHex);
	glBufferData(GL_ARRAY_BUFFER, sizeof(verticesHex), verticesHex, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOHexTris);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indicesHexTris), indicesHexTris, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOHexLines);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indicesHexLines), indicesHexLines, GL_STATIC_DRAW);

	glLineWidth(10);
	glEnable( GL_LINE_SMOOTH );
	glEnable( GL_POLYGON_SMOOTH );
	glHint( GL_LINE_SMOOTH_HINT, GL_NICEST );
	glHint( GL_POLYGON_SMOOTH_HINT, GL_NICEST );
	
	mainLoop();
	return 0;
}

void mainLoop()
{
	while (!glfwWindowShouldClose(window))
	{
		static float deltaTime = 0;
		static float lastElapsedRuntime = glfwGetTime();
		float currentTime = glfwGetTime();
		deltaTime = currentTime - lastElapsedRuntime;
		lastElapsedRuntime = currentTime;

		glClearColor(0, 0, 0, 1);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		model = glm::scale(glm::mat4(1.0f), {100, 100, 100});
		view = glm::translate(glm::mat4(1.0f), {0, 0, -1});
		projection = glm::ortho(-width * 0.5f, width * 0.5f, -height * 0.5f, height * 0.5f, 0.1f, 100.0f);
		
		shaderTexture->setMat4("model", model);
		shaderTexture->setMat4("view", view);
		shaderTexture->setMat4("projection", projection);
		
		shaderFlatColor->setMat4("model", model);
		shaderFlatColor->setMat4("view", view);
		shaderFlatColor->setMat4("projection", projection);
		
		//glBindTexture(GL_TEXTURE_2D, texture);
		//glDrawElements(GL_TRIANGLES, 3 * 6, GL_UNSIGNED_INT, 0);

		shaderFlatColor->use();
		glBindVertexArray(VAOHex);
		shaderFlatColor->setVec4("color", {1, 0, 0, 1});
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOHexTris);
		glDrawElements(GL_TRIANGLES, 3 * 6, GL_UNSIGNED_INT, 0);
		shaderFlatColor->setVec4("color", {1, 1, 1, 1});
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOHexLines);
		glDrawElements(GL_LINE_LOOP, 6, GL_UNSIGNED_INT, 0);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
}

void onWindowResize(GLFWwindow * window, int w, int h)
{
	width = w;
	height = h;
	glViewport(0, 0, width, height);
}

void onMouseEvent(GLFWwindow * window, double xpos, double ypos)
{
}

void initGlfw()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	window = glfwCreateWindow(width, height, "Project Alive", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return;
	}

	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, onWindowResize);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return;
	}

	glEnable(GL_DEPTH_TEST);

	glfwSetCursorPosCallback(window, onMouseEvent);
}