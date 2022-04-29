#include "pch.h"
#include "Organism.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "Engine/Physics.h"

void onWindowResize(GLFWwindow* window, int width, int height);
void onMouseEvent(GLFWwindow* window, double xpos, double ypos);
void error_callback(int error_code, const char* desc)
{
	log(desc);
}

void initGlfw();
void mainLoop();

GLFWwindow* window = nullptr;
glm::mat4 projection;
float width = 1600;
float height = 800;
Organism* organism1 = nullptr;
Organism* organism2 = nullptr;
TickCountTimer* timer = nullptr;
ClockTimer* timerclock = nullptr;

std::string randomDNA()
{
	auto vocabulary = "LTM";
	auto params = "0123";
	std::stringstream dna;
	int length = rand() % 5 + 1;
	for (int i = 0; i < length; i++)
	{
		if (dna.str().find('M') == std::string::npos)
			dna << vocabulary[rand() % 3];
		else
			dna << vocabulary[rand() % 2];
		dna << "(";
		dna << rand() % 4;
		dna << ")";
	}
	
	return dna.str();
}


int main()
{
	initGlfw();

	srand(time(NULL));

	Renderer2D::init();
	Renderer2D::setClearColor({ 0, 0, 0, 1 });

	TextureManager::add("cell_leaf.png");
	TextureManager::add("cell_thorn.png");
	TextureManager::add("cell_mover.png");
	TextureManager::add("collider.png");

	organism1 = new Organism(randomDNA());
	organism1->set_position({ 0, 0 });
	organism1->set_velocity_linear({ 0, 0 });
	organism1->set_velocity_angular(1);
	/*
	organism2 = new Organism(randomDNA());
	organism2->set_position({ -200, -200 });
	*/
	timer = new TickCountTimer(1000);

	mainLoop();

	return 0;
}

void mainLoop()
{
	while (!glfwWindowShouldClose(window))
	{
		static clock_t start = clock();
		clock_t now = clock();
		float deltaTime = (now - start) / (float)CLOCKS_PER_SEC;
		start = now;

		auto projection = glm::ortho(-0.5f * width, 0.5f * width, -0.5f * height, 0.5f * height);

		organism1->tick(deltaTime);

		Physics::update(deltaTime);

		Renderer2D::clear();
		Renderer2D::beginTextures(projection);
		organism1->draw();
		Renderer2D::endTextures();

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
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
	glfwSetErrorCallback(error_callback);

	if (!glfwInit())
	{
		log("Failed to init glfw");
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	window = glfwCreateWindow(width, height, "Project Alive", 0, nullptr);
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

	glfwSetCursorPosCallback(window, onMouseEvent);
}