#include "pch.h"
#include "Organism.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "Engine/IComponent.h"

void onWindowResize(GLFWwindow* window, int width, int height);
void onMouseEvent(GLFWwindow* window, double xpos, double ypos);
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
	organism1->set_position({ 200, 200 });
	organism2 = new Organism(randomDNA());
	organism2->set_velocity({ 100, 100 });
	timer = new TickCountTimer(1000);
	//timerclock = new ClockTimer(1000);

	entity e = 0;
	auto& sys = RigidBodySys::get();
	sys.add_component(e);
	auto* rb = sys.get_component(e);
	rb->Velocity = { 10, 10 };

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

		projection = glm::ortho(-width * 0.5f, width * 0.5f, -height * 0.5f, height * 0.5f, 0.00000001f, 100.0f);

		organism1->tick(deltaTime);
		organism2->tick(deltaTime);

		Renderer2D::clear();
		Renderer2D::beginTextures(projection);
		organism1->draw();
		organism2->draw();
		Renderer2D::endTextures();

		auto sys = RigidBodySys::get();
		sys.update();
		auto* rb = sys.get_component(0);
		log("velo: {0}", rb->Transform.Position.x);

		PhysicsManager::update(deltaTime);

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

	glfwSetCursorPosCallback(window, onMouseEvent);
}