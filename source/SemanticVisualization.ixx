module;
#pragma once
#include <glm/ext/quaternion_float.hpp>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <stb_image.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <nlohmann/json.hpp>
using json = nlohmann::json;

export module SemanticVisualization;

import std;
import GameInterface;
import Camera;
import Quad;
import InputSystem;
import SemanticSegmentationRenderer;
namespace Engine {
	std::vector<uint8_t> getOverlayColor(std::string semClass) {
		std::vector<uint8_t> color;
		if (semClass == "sphere") {
			color = { 255, 0, 0, 100 };
		}
		else if (semClass =="box") {
			color = { 0, 255, 0, 100 };
		}
		else if (semClass == "cone") {
			color = { 0, 0, 255, 100 };
		}
		else {
			color = { 255, 255, 255, 100 };
		}
		return color;
	}
	// --- Get Boundary Color ---
// Returns a fully opaque color for boundary drawing.
	std::vector<uint8_t> getBoundaryColor(std::string semClass) {
		std::vector<uint8_t> color{ 255, 255, 255, 255 };
		return color;
	}

	// A lightweight equivalent of your "Annotation" class in Processing.
	// Store any fields you need (bbox, mask, depth, etc.)
	struct Annotation
	{
	public:

		std::string id;          // e.g., "object_51"
		std::string semClass;    // e.g., "sphere", "box", "cone"
		std::array<float, 4> bbox; // {x, y, w, h}
		float depth{ 0.0f };
		int height{ 0 };
		int width{ 0 };
		std::vector<uint8_t> overlayColor{};
		std::vector<uint8_t> boundaryColor{};

		// For the mask, we can store a vector<bool> or vector<uint8_t>.
		// This mimics your "boolMask" from Processing.
		std::vector<bool> boolMask;

		Annotation(std::string id, std::string semClass, std::array<float, 4> bbox, float depth, int H, int W, std::vector<bool> boolMask, std::vector<uint8_t> overlayColor, std::vector<uint8_t> boundaryColor) :
			id(id), semClass(semClass), bbox(bbox), depth(depth), height(H), width(W), boolMask(boolMask), overlayColor(overlayColor), boundaryColor(boundaryColor) {
		}

	};

	// Controls
	bool drawImage{ true };
	bool drawBoundingBox{ true };
	bool drawInstanceBoundary{ true };
	bool drawSemanticMask{ true };
	bool drawDepthOverlay{ false };

	// Rendering Data
	int g_imageWidth{ 800 };
	int g_imageHeight{ 600 };
	std::vector<uint8_t> g_image;

	// Semantic Mask
	static std::vector<uint8_t> g_maskComposite; // RGBA buffer
	GLuint g_depthCompositeTex{ 0 };


	// Depth Color Mapping
	static std::vector<uint8_t> g_depthComposite; // RGBA buffer
	GLuint g_maskCompositeTex{ 0 };
	static bool g_selectionChanged = true;     // True if we need to rebuild
	glm::vec4  g_nearColor, g_farColor;
	static float g_minDepth = std::numeric_limits<float>::max();
	static float g_maxDepth = std::numeric_limits<float>::lowest();

	json cocoData;
	std::string filterClass{ "" }; // Filter classes should be adaptable

	// Global or static vector to store all annotations (similar to your Processing `ArrayList<Annotation>`)
	std::vector<Annotation> g_annotations;



	float normClamped(float value, float minVal, float maxVal)
	{
		float t = (value - minVal) / (maxVal - minVal);
		return std::clamp(t, 0.0f, 1.0f);
	}
	// --- Get Depth Color ---
	// Maps a depth value to a color gradient between nearColor and farColor.
	glm::vec4 getDepthColor(float depth) {
		float t = normClamped(depth, g_minDepth, g_maxDepth);
		return glm::mix(g_nearColor, g_farColor, t);
	}



	void exportSVGWithMetadata(std::string filename, float scale) {
		// TODO: Implement this function

	}

	std::vector<bool> decodeRLE(const json& counts, int height, int width) {
		int n{ height * width };
		std::vector<bool> mask(n, false);

		int idx{ 0 };
		bool isForeground{ false };

		if (!counts.empty() && counts.at(0).get<int>() == 0) {
			isForeground = true;
		}

		for (int i{ 0 }; i < counts.size(); i++) {
			int count{ counts.at(i).get<int>() };
			for (int j{ 0 }; j < count; j++) {
				if (idx < n)
				{
					mask[idx] = isForeground;
					idx++;
				}
			}
			isForeground = !isForeground;
		}
		return mask;
	}

	void buildDepthComposite()
	{
		// (A) Create/resize the composite buffer if needed
		g_depthComposite.resize(g_imageWidth * g_imageHeight * 4, 0);
		g_maskComposite.resize(g_imageWidth * g_imageHeight * 4, 0);

		// 1) Initialize everything transparent
		// Just ensure it's zeroed:
		std::fill(g_depthComposite.begin(), g_depthComposite.end(), 0);
		std::fill(g_maskComposite.begin(), g_maskComposite.end(), 0);
		// 2) For each annotation, if it passes filterClass,
		//    we compute depthColor & "stamp" it onto the composite
		for (auto& ann : g_annotations)
		{
			// Filter
			if (!filterClass.empty() && (ann.semClass != filterClass))
				continue;


			// Compute color from depth
			glm::vec4 c = getDepthColor(ann.depth);
			// Convert to RGBA [0..255]
			uint8_t r = (uint8_t)(c.r * 255.0f);
			uint8_t g = (uint8_t)(c.g * 255.0f);
			uint8_t b = (uint8_t)(c.b * 255.0f);
			uint8_t a = (uint8_t)(c.a * 255.0f);
			std::vector<uint8_t> overlayColor = ann.overlayColor;
			// "Stamp" each pixel where boolMask == true
			// Same "column-major" approach from your code:
			// colIndex = x*H + y => unusual, but matches your decodeRLE
			for (int y = 0; y < ann.height; y++)
			{
				for (int x = 0; x < ann.width; x++)
				{
					int colIndex = x * ann.height + y;
					if (colIndex < (int)ann.boolMask.size() && ann.boolMask[colIndex])
					{
						// This annotation covers pixel x,y.
						// We need to place that at (x,y) in the "global" composite
						// BUT WAIT: The annotation might not always match the entire image size 
						// if (ann.width != g_imageWidth || ann.height != g_imageHeight).
						// For now, we assume it does match. 

						// row-major offset
						int rowIndex = (y * g_imageWidth + x) * 4;
						// "Blending" for example: if it’s currently 0 alpha, set the color.
						// or if you want to overwrite, just set it.

						g_depthComposite[rowIndex + 0] = r;
						g_depthComposite[rowIndex + 1] = g;
						g_depthComposite[rowIndex + 2] = b;
						g_depthComposite[rowIndex + 3] = a;

						g_maskComposite[rowIndex + 0] = overlayColor.at(0);
						g_maskComposite[rowIndex + 1] = overlayColor.at(1);
						g_maskComposite[rowIndex + 2] = overlayColor.at(2);
						g_maskComposite[rowIndex + 3] = overlayColor.at(3);
					}
				}
			}
		}
		g_selectionChanged = true;
	}




	// --- Load Annotations from COCO JSON ---
	void loadAnnotations() {
		json segmentation{ cocoData["segmentation"] };
		for (auto it = segmentation.begin(); it != segmentation.end(); ++it) {
			std::string id{ it.key() };
			json obj{ it.value() };
			std::string semClass{ obj.at("class") };
			if (semClass == "unknown") continue;

			// Get bounding box.
			json bboxArr{ obj.at("bbox") };
			float x{ bboxArr.at(0).get<float>() };
			float y{ bboxArr.at(1).get<float>() };
			float w{ bboxArr.at(2).get<float>() };
			float h{ bboxArr.at(3).get<float>() };
			std::array<float, 4> bbox{ x, y, w, h };

			// Parse size (height, width).
			json sizeArr{ obj.at("size") };
			int H{ sizeArr.at(0).get<int>() };
			int W{ sizeArr.at(1).get<int>() };

			// Decode the RLE into a boolean mask.
			json countsArr{ obj.at("counts") };
			std::vector<bool> boolMask = decodeRLE(countsArr, H, W);

			std::vector<uint8_t> overlayColor = getOverlayColor(semClass);
			std::vector<uint8_t> boundaryColor = getBoundaryColor(semClass);			
			// Get depth from JSON.
			float depth = obj.at("depth");
			// Update global min and max depth.
			if (depth < g_minDepth) g_minDepth = depth;
			if (depth > g_maxDepth) g_maxDepth = depth;

			// Create and store annotation (store the boolean mask as well).
			Annotation ann(id, semClass, bbox, depth, H, W, std::move(boolMask), overlayColor, boundaryColor);
			g_annotations.push_back(std::move(ann));

		}
		std::cout << "Loaded " << g_annotations.size() << " annotations.\n";
		std::cout << "Depth range: min=" << g_minDepth << ", max=" << g_maxDepth << std::endl;
		
	}

	// --- Load Image from File ---
	std::vector<uint8_t> loadImage(std::string filename)
	{
		int width = 0;
		int height = 0;
		int channels = 0;

		// Force STB to give us RGBA (4 channels), so it's easier to make an OpenGL texture
		unsigned char* data = stbi_load(filename.c_str(), &width, &height, &channels, STBI_rgb_alpha);
		if (!data)
		{
			std::cerr << "Failed to load image: " << filename << "\n";
			return {};
		}

		// Copy raw bytes into a std::vector<uint8_t>
		// width * height * 4 because we requested RGBA
		size_t totalBytes = static_cast<size_t>(width) * static_cast<size_t>(height) * 4;
		std::vector<uint8_t> imageData(totalBytes);
		std::memcpy(imageData.data(), data, totalBytes);

		// Free STB image buffer
		stbi_image_free(data);

		// Store the dimensions in our global variables
		g_imageWidth = width;
		g_imageHeight = height;

		return imageData;
	}


	


	export class SemanticVisualization : public GameInterface
	{
	public:
		void Initialize(GLFWwindow* window) override;
		void Render(float aspectRatio) override;
		void ClearResources() override;
		void Update(double deltaTime) override;
		void RenderIMGui();
	private:
		SemanticSegmentationRenderer m_renderer;
		InputSystem m_input;
		Camera m_camera;
	};

	struct Ray {
		glm::vec3 origin;
		glm::vec3 direction;
	};

	struct Sphere {
		glm::vec3 center;
		float radius;
	};
	json loadJSONObject(std::string filename)
	{
		std::fstream file{};
		json doc{};
		try {
			file.open(filename, std::ios::in);
			doc = { json::parse(file) };
			return doc;
		}
		catch (json::parse_error e)
		{
			std::cout << e.what() << std::endl;
			return nullptr;
		}
	}


	//************************************
// Set up Keyboard Observer, Initialize the Renderer and Initialize the Skeleton.
//************************************
	void SemanticVisualization::Initialize(GLFWwindow* window)
	{
		m_input.SetWindow(window);
		// Observe Input
		m_input.ObserveKey(GLFW_KEY_SPACE);
		m_input.ObserveKey(GLFW_KEY_RIGHT);
		m_input.ObserveKey(GLFW_KEY_LEFT);
		m_input.ObserveKey(GLFW_KEY_UP);
		m_input.ObserveKey(GLFW_KEY_DOWN);
		m_input.ObserveKey(GLFW_KEY_1);
		m_input.ObserveKey(GLFW_KEY_R);
		m_input.ObserveKey(GLFW_KEY_LEFT_SHIFT);
		m_input.ObserveKey(GLFW_KEY_S);
		m_input.ObserveKey(GLFW_KEY_B);
		m_input.ObserveKey(GLFW_KEY_C);
		m_input.ObserveKey(GLFW_KEY_A);
		m_input.ObserveKey(GLFW_KEY_O);
		m_input.ObserveKey(GLFW_KEY_I);
		m_input.ObserveKey(GLFW_KEY_M);
		m_input.ObserveKey(GLFW_KEY_D);
		m_input.ObserveKey(GLFW_KEY_V);
		m_renderer.Initialize();
		// Load and Upload Image
		g_image = loadImage("./input/segmentation/render.png");
		m_renderer.createImage(g_image, g_imageHeight, g_imageWidth);


		// Load Annotation JSON Data
		cocoData = loadJSONObject("./input/segmentation/coco_output.json");
		loadAnnotations();

		// 50% alpha red to 50% alpha blue
		g_nearColor = glm::vec4(1.0f, 0.0f, 0.0f, 0.5f);  // half-alpha red
		g_farColor = glm::vec4(0.0f, 0.0f, 1.0f, 0.5f);  // half-alpha blue

		buildDepthComposite();
		m_renderer.uploadComposite(g_depthCompositeTex, g_depthComposite, g_imageHeight, g_imageWidth);
		m_renderer.uploadComposite(g_maskCompositeTex, g_maskComposite, g_imageHeight, g_imageWidth);
		g_selectionChanged = false;

		Quad quad(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(3.0f, 0.0f, 0.0f), glm::vec3(2.0f, 2.0f, 0.0f), glm::vec3(0.0f, 2.0f, 0.0f));
		m_renderer.InitQuad(quad);
	}


	//************************************
	// Calculate mvp matrix, calculate and render joint transforms and calculate and render skin using the boneModelMatrices.
	//************************************
	void SemanticVisualization::Render(float aspectRatio)
	{
		glm::mat4 Projection = m_camera.GetProjectionMatrix(aspectRatio);
		glm::mat4 View = m_camera.GetViewMatrix();
		glm::mat4 Model = glm::mat4(1.0f);
		glm::mat4 mvp = Projection * View * Model;
		glm::mat4 quadTransform = mvp * glm::mat4(1.0f);



	

		m_renderer.RenderQuad(quadTransform);
		// Render the gizmo lines
		m_renderer.RenderGizmo(mvp);
		RenderIMGui();
	}

	//************************************
	// Calculate mvp matrix, calculate and render joint transforms and calculate and render skin using the boneModelMatrices.
	//************************************
	void SemanticVisualization::ClearResources()
	{
		m_renderer.ClearResources();
	}


	//************************************
	// Handle Input and Update Animation
	//************************************
	void SemanticVisualization::Update(double deltaTime)
	{
		m_input.Update();
		// Handle Keyboard Input
		if (m_input.WasKeyPressed(GLFW_KEY_1)) {
			drawImage = !drawImage;
		}

		if (m_input.WasKeyPressed(GLFW_KEY_S)) {
			filterClass = "sphere";
			std::cout << "Filtering for spheres\n";
			g_selectionChanged = true;
		}
		else if (m_input.WasKeyPressed(GLFW_KEY_B)) {
			filterClass = "box";
			std::cout << "Filtering for boxes\n";
			g_selectionChanged = true;
		}
		else if (m_input.WasKeyPressed(GLFW_KEY_C)) {
			filterClass = "cone";
			std::cout << "Filtering for cones\n";
			g_selectionChanged = true;
		}
		else if (m_input.WasKeyPressed(GLFW_KEY_A)) {
			filterClass = "";
			std::cout << "Clearing filter: showing all known objects\n";
		}

		if (m_input.WasKeyPressed(GLFW_KEY_O)) {
			drawBoundingBox = !drawBoundingBox;

		}
		if (m_input.WasKeyPressed(GLFW_KEY_I)) {
			drawInstanceBoundary = !drawInstanceBoundary;
		}
		if (m_input.WasKeyPressed(GLFW_KEY_M)) {
			drawSemanticMask = !drawSemanticMask;
		}
		if (m_input.WasKeyPressed(GLFW_KEY_D)) {
			drawDepthOverlay = !drawDepthOverlay;
			std::cout << "Depth overlay: " << drawDepthOverlay << "\n";
		}
		if (m_input.WasKeyPressed(GLFW_KEY_V)) {
			exportSVGWithMetadata("output_metadata.svg", 1.0f);
		}
		
		bool rotateLeft = m_input.IsKeyDown(GLFW_KEY_LEFT);
		bool rotateRight = m_input.IsKeyDown(GLFW_KEY_RIGHT);
		bool zoomIn = m_input.IsKeyDown(GLFW_KEY_UP);
		bool zoomOut = m_input.IsKeyDown(GLFW_KEY_DOWN);

		if (g_selectionChanged)
		{
			buildDepthComposite();
			m_renderer.uploadComposite(g_depthCompositeTex, g_depthComposite, g_imageHeight, g_imageWidth);
			m_renderer.uploadComposite(g_maskCompositeTex, g_maskComposite, g_imageHeight, g_imageWidth);
			g_selectionChanged = false;
		}


		




		// Update the camera with input flags
		m_camera.Update(deltaTime, rotateLeft, rotateRight, zoomIn, zoomOut);
	}



	void SemanticVisualization::RenderIMGui()
	{
		// Start a new ImGui frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		// -----------------------------------------------
		// 1) Show a simple window for stats (already in your code)
		// -----------------------------------------------
		{
			ImGui::Begin("Raytracing Stats");

			ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",
				1000.0f / ImGui::GetIO().Framerate,
				ImGui::GetIO().Framerate);

			ImGui::End();
		}

		// -----------------------------------------------
		// 2) Create a new window with our semantic visualization controls
		// -----------------------------------------------
		{
			ImGui::Begin("Semantic Visualization");

			// Checkboxes for toggles that also exist as keybinds
			ImGui::Checkbox("Draw Image (Key: 1)", &drawImage);
			ImGui::Checkbox("Bounding Box (Key: O)", &drawBoundingBox);
			ImGui::Checkbox("Instance Boundary (Key: I)", &drawInstanceBoundary);
			ImGui::Checkbox("Semantic Mask (Key: M)", &drawSemanticMask);
			ImGui::Checkbox("Depth Overlay (Key: D)", &drawDepthOverlay);

			// A combo box (or radio buttons) for filterClass
			static const char* filterItems[] = { "All", "Sphere", "Box", "Cone" };
			// We'll keep track of which item is selected
			static int currentFilterIndex = 0;

			// Convert our existing filterClass => index
			// (You could do more robust logic if needed)
			if (filterClass == "")        currentFilterIndex = 0;
			else if (filterClass == "sphere") currentFilterIndex = 1;
			else if (filterClass == "box")    currentFilterIndex = 2;
			else if (filterClass == "cone")   currentFilterIndex = 3;

			// Draw the combo
			if (ImGui::Combo("Filter Class", &currentFilterIndex,
				filterItems, IM_ARRAYSIZE(filterItems)))
			{
				// Update filterClass whenever user changes the combo
				if (currentFilterIndex == 0) { filterClass = ""; g_selectionChanged = true; }
				else if (currentFilterIndex == 1) { filterClass = "sphere";			g_selectionChanged = true; }
				else if (currentFilterIndex == 2) {filterClass = "box";			g_selectionChanged = true;
			}
			else if (currentFilterIndex == 3) {filterClass = "cone";			g_selectionChanged = true;
		}

			}

			// A button to export metadata
			if (ImGui::Button("Export SVG (Key: V)"))
			{
				exportSVGWithMetadata("output_metadata.svg", 1.0f);
			}

			ImGui::Separator();
			// Shows the loaded image if we have a valid texture and drawImage == true
			if (drawImage && m_renderer.m_image != 0)
			{
				ImGui::Text("Loaded Image:");
				ImGui::Image(
					reinterpret_cast<void*>(static_cast<intptr_t>(m_renderer.m_image)),
					ImVec2(static_cast<float>(g_imageWidth), static_cast<float>(g_imageHeight))
				);
			}
			else
			{ // We can deactivate showing the actual image
				ImGui::Text("No image loaded. Showing placeholder background...");
				ImVec2 imageSize((float)g_imageWidth, (float)g_imageHeight);
				ImGui::InvisibleButton("image_placeholder", imageSize);
				ImVec2 startPos = ImGui::GetItemRectMin(); // top-left
				ImVec2 endPos = ImGui::GetItemRectMax(); // bottom-right
				ImDrawList* drawList = ImGui::GetWindowDrawList();
				drawList->AddRectFilled(
					startPos,
					endPos,
					IM_COL32(20, 20, 20, 255) 
				);
			}
			// Iterate through each annotation
			// Where ImGui drew the image in the window:
			ImVec2 imagePos = ImGui::GetItemRectMin(); // top-left corner in screen coords
			ImVec2 imageSize = ImGui::GetItemRectSize(); // size of the drawn image

			// Retrieve ImGui's current draw list for this window
			ImDrawList* drawList = ImGui::GetWindowDrawList();
			if (drawDepthOverlay)
			{
				// Where ImGui drew the base image
				ImVec2 topLeft = imagePos;
				ImVec2 size = ImVec2((float)g_imageWidth, (float)g_imageHeight);

				drawList->AddImage(
					reinterpret_cast<void*>(static_cast<intptr_t>(g_depthCompositeTex)),
					topLeft,
					ImVec2(topLeft.x + size.x, topLeft.y + size.y)
				);
			}
			else if (drawSemanticMask) {
				// Where ImGui drew the base image
				ImVec2 topLeft = imagePos;
				ImVec2 size = ImVec2((float)g_imageWidth, (float)g_imageHeight);

				drawList->AddImage(
					reinterpret_cast<void*>(static_cast<intptr_t>(g_maskCompositeTex)),
					topLeft,
					ImVec2(topLeft.x + size.x, topLeft.y + size.y)
					);
			}
			if (drawInstanceBoundary)
			{
			}
			for (auto& ann : g_annotations)
			{
				// For each annotation, if it passes our filter, overlay its mask, boundaries, bbox, and label.
				if (!filterClass.empty() && (ann.semClass != filterClass)) {
					continue;
				}
				// Only draw bounding boxes if toggled
				if (drawBoundingBox)
				{
					// BBox coordinates in the *image* coordinate system
					float x = ann.bbox[0];
					float y = ann.bbox[1];
					float w = ann.bbox[2];
					float h = ann.bbox[3];
					ImVec2 topLeft = ImVec2(imagePos.x + x, imagePos.y + y);
					ImVec2 botRight = ImVec2(imagePos.x + x + w, imagePos.y + y + h);

					// Draw a red rectangle
					drawList->AddRect(
						topLeft,
						botRight,
						IM_COL32(255, 0, 0, 255), // Red color
						0.0f,    // Rounding
						0,       // Corner flags
						2.0f     // Thickness
					);

					// Draw a label above the top-left corner
					//    e.g., "object_51 (box)"
					ImVec2 textPos = ImVec2(topLeft.x, topLeft.y - 5.0f);
					std::string label = ann.id + " (" + ann.semClass + ")";
					drawList->AddText(textPos, IM_COL32(255, 0, 0, 255), label.c_str());
				}
			}

			ImGui::End();
		}

		// -----------------------------------------------
		// 3) Render all ImGui windows
		// -----------------------------------------------
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}




}
