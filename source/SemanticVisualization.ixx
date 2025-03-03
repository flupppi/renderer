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
import Annotation;
import Camera;
import Quad;
import InputSystem;
import SemanticSegmentationRenderer;

namespace Engine {


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
	static std::vector<uint8_t> g_maskComposite{}; // RGBA buffer
	GLuint g_depthCompositeTex{ 0 };
	// Instance Mask
	static std::vector<uint8_t> g_instanceComposite{}; // RGBA buffer
	GLuint g_instanceCompositeTex{ 0 };


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

	std::vector<uint8_t> getOverlayColor(std::string semClass) {
		std::vector<uint8_t> color;
		if (semClass == "sphere") {
			color = { 255, 0, 0, 100 };
		}
		else if (semClass == "box") {
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
	void generateCompositeImages()
	{
		// (A) Create/resize the composite buffers if needed
		g_depthComposite.resize(g_imageWidth * g_imageHeight * 4, 0);
		g_maskComposite.resize(g_imageWidth * g_imageHeight * 4, 0);
		g_instanceComposite.resize(g_imageWidth * g_imageHeight * 4, 0);

		// 1) Initialize them to zero (fully transparent)
		std::fill(g_depthComposite.begin(), g_depthComposite.end(), 0);
		std::fill(g_maskComposite.begin(), g_maskComposite.end(), 0);
		std::fill(g_instanceComposite.begin(), g_instanceComposite.end(), 0);

		// 2) For each annotation, if it passes filterClass,
		//    compute its depth color & "stamp" it within its bounding box
		for (auto& ann : g_annotations)
		{
			// Filter out annotations if we have a non-empty filterClass
			if (!filterClass.empty() && (ann.semClass != filterClass))
				continue;

			// Compute a depth-based color
			glm::vec4 depthColor = getDepthColor(ann.depth);
			uint8_t r = static_cast<uint8_t>(depthColor.r * 255.0f);
			uint8_t g = static_cast<uint8_t>(depthColor.g * 255.0f);
			uint8_t b = static_cast<uint8_t>(depthColor.b * 255.0f);
			uint8_t a = static_cast<uint8_t>(depthColor.a * 255.0f);

			// Also retrieve this annotation’s overlay color
			const std::vector<uint8_t>& overlayColor = ann.overlayColor;

			// -------------------------------------------------
			//   Use the bounding box to restrict iteration
			// -------------------------------------------------
			// Convert bbox floats to integer coords
			int xMin = ann.bbox[0];
			int yMin = ann.bbox[1];
			int xMax = xMin + ann.bbox[2];
			int yMax = yMin + ann.bbox[3];

			// Clip to the global image boundaries
			xMin = std::max(0, xMin);
			yMin = std::max(0, yMin);
			xMax = std::min(g_imageWidth, xMax);
			yMax = std::min(g_imageHeight, yMax);


			// Retrieve boundary color (fully opaque in this example)
			const std::vector<uint8_t>& boundaryColor = ann.boundaryColor;

			// Traverse only within the bounding box
			for (int y = yMin; y < yMax; y++)
			{
				for (int x = xMin; x < xMax; x++)
				{

					// Convert to index in the boolMask
					int colIndex = (x * ann.height + y);
					if (colIndex >= 0 && colIndex < static_cast<int>(ann.boolMask.size()) && ann.boolMask[colIndex])
					{


						// -----------------------------
						//   Check if this pixel is an edge
						// -----------------------------
						bool isEdge = false;
						for (int dy = -1; dy <= 1 && !isEdge; dy++)
						{
							for (int dx = -1; dx <= 1 && !isEdge; dx++)
							{
								if (dx == 0 && dy == 0)
									continue; // skip self

								int nx = x + dx;
								int ny = y + dy;
								// If neighbor is out of bounds, this pixel is an edge
								if (nx < 0 || nx >= ann.width || ny < 0 || ny >= ann.height)
								{
									isEdge = true;
								}
								else
								{
									// Convert neighbor (nx,ny) to boolMask index
									int nColIndex = nx * ann.height + ny;
									if (nColIndex < 0 || nColIndex >= static_cast<int>(ann.boolMask.size()) ||
										!ann.boolMask[nColIndex])
									{
										isEdge = true;
									}
								}
							}
						}
						// row-major offset in the global (800×600) image
						int rowIndex = (y * ann.width + x) * 4;
						// 4) Stamp the boundary color if isEdge, else transparent
						if (isEdge)
						{
							g_instanceComposite[rowIndex + 0] = boundaryColor[0];
							g_instanceComposite[rowIndex + 1] = boundaryColor[1];
							g_instanceComposite[rowIndex + 2] = boundaryColor[2];
							g_instanceComposite[rowIndex + 3] = boundaryColor[3];
						}
						else
						{
							// Interior => fully transparent
							g_instanceComposite[rowIndex + 0] = 0;
							g_instanceComposite[rowIndex + 1] = 0;
							g_instanceComposite[rowIndex + 2] = 0;
							g_instanceComposite[rowIndex + 3] = 0;
						}

						// Depth composite
						g_depthComposite[rowIndex + 0] = r;
						g_depthComposite[rowIndex + 1] = g;
						g_depthComposite[rowIndex + 2] = b;
						g_depthComposite[rowIndex + 3] = a;

						// Semantic mask
						g_maskComposite[rowIndex + 0] = overlayColor[0];
						g_maskComposite[rowIndex + 1] = overlayColor[1];
						g_maskComposite[rowIndex + 2] = overlayColor[2];
						g_maskComposite[rowIndex + 3] = overlayColor[3];
					}
				}
			}
		}

		// Tell the rest of the system we updated the composite
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
			int x{ bboxArr.at(0).get<int>() };
			int y{ bboxArr.at(1).get<int>() };
			int w{ bboxArr.at(2).get<int>() };
			int h{ bboxArr.at(3).get<int>() };
			std::array<int, 4> bbox{ x, y, w, h };

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
		SemanticVisualization(bool standalone = false)
			: m_isStandalone(standalone)
		{
		}
		void Initialize(GLFWwindow* window) override;
		void Render(float aspectRatio) override;
		void ClearResources() override;
		void Update(double deltaTime) override;
		void StandaloneImGuiFrame();
		void DrawImGui(bool* pWindowOpen = nullptr);
	private:
		SemanticSegmentationRenderer m_renderer;
		InputSystem m_input;
		bool m_isStandalone{ false };
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
		m_input.ObserveKey(GLFW_KEY_1);
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

		generateCompositeImages();
		m_renderer.uploadComposite(g_depthCompositeTex, g_depthComposite, g_imageHeight, g_imageWidth);
		m_renderer.uploadComposite(g_maskCompositeTex, g_maskComposite, g_imageHeight, g_imageWidth);
		m_renderer.uploadComposite(g_instanceCompositeTex, g_instanceComposite, g_imageHeight, g_imageWidth);
		g_selectionChanged = false;
	}


	void SemanticVisualization::Render(float aspectRatio)
	{

		if (m_isStandalone)
		{
			// If we are running as the only program, do the entire frame
			StandaloneImGuiFrame();
		}
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
		if (g_selectionChanged)
		{
			generateCompositeImages();
			m_renderer.uploadComposite(g_depthCompositeTex, g_depthComposite, g_imageHeight, g_imageWidth);
			m_renderer.uploadComposite(g_maskCompositeTex, g_maskComposite, g_imageHeight, g_imageWidth);
			m_renderer.uploadComposite(g_instanceCompositeTex, g_instanceComposite, g_imageHeight, g_imageWidth);
			g_selectionChanged = false;
		}
	}

	void SemanticVisualization::StandaloneImGuiFrame()
	{
		// This is only used if we are the sole program in the main loop
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		// Build the UI
		DrawImGui();

		// Render
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}


	void SemanticVisualization::DrawImGui(bool* pWindowOpen)
	{

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
		bool windowOpen = false;
		if (pWindowOpen)
			if (*pWindowOpen)
				windowOpen = ImGui::Begin("Semantic Visualization", pWindowOpen);
			else 
				return;
		else
			windowOpen = ImGui::Begin("Semantic Visualization");
		if (windowOpen) {
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
				if (currentFilterIndex == 0) {
					filterClass = "";
					g_selectionChanged = true;
				}
				else if (currentFilterIndex == 1) {
					filterClass = "sphere";
					g_selectionChanged = true;
				}
				else if (currentFilterIndex == 2) {
					filterClass = "box";
					g_selectionChanged = true;
				}
				else if (currentFilterIndex == 3) {
					filterClass = "cone";
					g_selectionChanged = true;
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
				// Where ImGui drew the base image
				ImVec2 topLeft = imagePos;
				ImVec2 size = ImVec2((float)g_imageWidth, (float)g_imageHeight);

				drawList->AddImage(
					reinterpret_cast<void*>(static_cast<intptr_t>(g_instanceCompositeTex)),
					topLeft,
					ImVec2(topLeft.x + size.x, topLeft.y + size.y)
				);
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
		}
		ImGui::End();
	}
}

