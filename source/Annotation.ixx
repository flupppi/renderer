module;
#include<glm/glm.hpp>

export module Annotation;

import std;

namespace Engine {

	// A lightweight equivalent of your "Annotation" class in Processing.
	// Store any fields you need (bbox, mask, depth, etc.)
	export struct Annotation
	{
	public:

		std::string id;          // e.g., "object_51"
		std::string semClass;    // e.g., "sphere", "box", "cone"
		std::array<int, 4> bbox; // {x, y, w, h}
		float depth{ 0.0f };
		int height{ 0 };
		int width{ 0 };
		std::vector<uint8_t> overlayColor{};
		std::vector<uint8_t> boundaryColor{};

		// For the mask, we can store a vector<bool> or vector<uint8_t>.
		// This mimics your "boolMask" from Processing.
		std::vector<bool> boolMask;

		Annotation(std::string id, std::string semClass, std::array<int, 4> bbox, float depth, int H, int W, std::vector<bool> boolMask, std::vector<uint8_t> overlayColor, std::vector<uint8_t> boundaryColor) :
			id(id), semClass(semClass), bbox(bbox), depth(depth), height(H), width(W), boolMask(boolMask), overlayColor(overlayColor), boundaryColor(boundaryColor) {
		}

	};
}