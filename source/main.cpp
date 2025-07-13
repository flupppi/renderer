#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <lua.hpp> // Lua header for C API
#include <cxxopts.hpp>
#include <fmt/format.h>
#include <range/v3/view.hpp>

import std;
import BillboardCloud;
import GameInterface;
import Application;
import Game;
import Raytracer;
import BillboardGenerator;
import PlaneSelector;
import SemanticVisualization;
import CombinedInterface;


using namespace Engine;
GameInterface* gUsedInterface;
const int WIDTH{ 1280 };
const int HEIGHT{ 720 };


// Helper to display a little (?) mark which shows a tooltip when hovered.
// In your own code you may want to display an actual icon if you are using a merged icon fonts (see docs/FONTS.md)
static void HelpMarker(const char* desc)
{
	ImGui::TextDisabled("(?)");
	if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayShort))
	{
		ImGui::BeginTooltip();
		ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
		ImGui::TextUnformatted(desc);
		ImGui::PopTextWrapPos();
		ImGui::EndTooltip();
	}
}


//************************************
// Initialize Dear ImGui into the existing GLFW / OpenGL session.
//************************************
void InitializeDearImGui(GLFWwindow* window)
{
	ImGui::CreateContext();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init(nullptr);
	ImGui::StyleColorsDark();
}




// Function to test Lua integration
void TestLua() {
	// Initialize Lua
	lua_State* L = luaL_newstate(); // Create new Lua state
	if (!L) {
		std::cerr << "Failed to create Lua state\n";
		return;
	}

	// Load Lua standard libraries
	luaL_openlibs(L);

	// Lua script (embedded as a string)
	const char* luaScript = R"(
        print("Hello, World from Lua!")
        function add(a, b)
            return a + b
        end
    )";

	// Execute the Lua script
	if (luaL_dostring(L, luaScript) != LUA_OK) {
		std::cerr << "Error running Lua script: " << lua_tostring(L, -1) << "\n";
		lua_close(L);
		return;
	}

	// Call the Lua function `add` from C++
	lua_getglobal(L, "add"); // Push the function onto the stack
	lua_pushnumber(L, 5);   // Push first argument
	lua_pushnumber(L, 10);  // Push second argument

	// Call the function (2 arguments, 1 return value)
	if (lua_pcall(L, 2, 1, 0) != LUA_OK) {
		std::cerr << "Error calling Lua function: " << lua_tostring(L, -1) << "\n";
		lua_close(L);
		return;
	}

	// Retrieve the result
	if (lua_isnumber(L, -1)) {
		double result = lua_tonumber(L, -1);
		std::cout << "Result of add(5, 10): " << result << "\n";
	}

	// Close Lua
	lua_close(L);
}


void TestAssimp() {
	// Create an instance of the Importer class
	Assimp::Importer importer;

	// Load a test model (replace "path/to/model.obj" with an actual path to a model)
	const aiScene* scene = importer.ReadFile(
		"./input/Car-Model/Car.obj", // Path to your 3D model
		aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_JoinIdenticalVertices
	);

	// Check if the model was loaded successfully
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
		std::cerr << "Assimp Error: " << importer.GetErrorString() << "\n";
		return;
	}

	// Print basic information about the model
	std::cout << "Model loaded successfully!\n";
	std::cout << "Number of meshes: " << scene->mNumMeshes << "\n";
	std::cout << "Number of materials: " << scene->mNumMaterials << "\n";

	// Print information about the first mesh
	if (scene->mNumMeshes > 0) {
		aiMesh* mesh = scene->mMeshes[0];
		std::cout << "First mesh has " << mesh->mNumVertices << " vertices.\n";
		if (mesh->HasFaces()) {
			std::cout << "First mesh has " << mesh->mNumFaces << " faces.\n";
		}
	}
}

//************************************
// Initialize GLFW, Load OpenGL with glew and set up the renderer.
//************************************
GLFWwindow* InitializeSystem()
{
	// Setup window
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, gUsedInterface->mode.c_str(), nullptr, nullptr);

	glfwMakeContextCurrent(window);
	//glfwSwapInterval(1); // Enable vsync
	glewInit();
	InitializeDearImGui(window);

	gUsedInterface->Initialize(window);
	return window;
}

//************************************
// Set up Game Loop and render scene + Process and Draw UI
//************************************
void RunCoreloop(GLFWwindow* window)
{
	double lastTime = glfwGetTime();
	double timeDifference = 0.0f;
	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();

		gUsedInterface->Update(timeDifference);
		int screenWidth, screenHeight;
		glfwGetFramebufferSize(window, &screenWidth, &screenHeight);
		float aspectRatio = (float)screenWidth / (float)screenHeight;
		glViewport(0, 0, screenWidth, screenHeight);
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);
		// Enable blending
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		gUsedInterface->Render(aspectRatio);

		glfwSwapBuffers(window);

		double currentTime = glfwGetTime();
		timeDifference = currentTime - lastTime;
		lastTime = currentTime;
	}
}

std::unique_ptr<GameInterface> CreateGameInterface(const cxxopts::ParseResult& results) {
	// Populate objects depending on the parsed results
	std::string modelPath;
	if (results.count("path")) {
		modelPath = results["path"].as<std::string>();
	}
	else {
		std::cout << "Error: Missing parameter for 'path'" << std::endl;
		return 0;
	}
	// Select the game mode based on some configuration, argument, or state.
	const std::string mode = results["program"].as<std::string>();
	if (mode == "BillboardCloud") {
		// Create specific implementations for generator and selector
		auto generator = std::make_unique<BillboardGenerator>();
		auto selector = std::make_unique<PlaneSelector>();
		return std::make_unique<BillboardCloud>(std::move(generator), std::move(selector), modelPath);
	}
	else if (mode == "Application") {
		return std::make_unique<Application>(true);
	}
	else if (mode == "Game") {
		return std::make_unique<Game>();
	}
	else if (mode == "Raytracer") {
		return std::make_unique<Raytracer>();
	}
	else if (mode == "SemanticVisualization") {
		return std::make_unique<SemanticVisualization>(true);
	}
	else if (mode == "CombinedInterface") {
		return std::make_unique<CombinedInterface>();
	}
	else {
		throw std::runtime_error("Unknown game mode: " + mode);
	}
}

//************************************
// Unload all resources used by the System.
//************************************
void ShutdownSystem()
{
	gUsedInterface->ClearResources();
	glfwTerminate();
}

//************************************
// Program entry point
//************************************
int main(int argc, char* argv[])
{
	try {
		// Command-line parsing using cxxopts
		cxxopts::Options options("Engine", "A simple engine with different modes:");
		options.add_options()
			("p, program", "Program mode (Application, BillboardCloud, etc.)", cxxopts::value<std::string>()->default_value("Application"))
			("m, path", "Path to glTF model file", cxxopts::value<std::string>())
			("h, help", "Print usage information");
		auto result = options.parse(argc, argv);
		// Show help message
		if (result.count("help")) {
			std::cout << options.help() << std::endl;
			return 0;
		}
		//TestLua();
		//TestAssimp();
		// Create the GameInterface instance using the factory function
		const auto gameInterface = CreateGameInterface(result);
		// Set the global pointer to the created instance
		gUsedInterface = gameInterface.get();  // Assign to the global pointer
		GLFWwindow* window = InitializeSystem();
		RunCoreloop(window);
		ShutdownSystem();
	}
	catch (std::out_of_range&)
	{
		std::cerr << "range error\n";
	}
	catch (...)
	{
		std::cerr << "unknown exception thrown\n";
	}
}
