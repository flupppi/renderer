module;
#pragma once
export module IPlaneSelector;
import std;
import Plane;
import Mesh;

namespace Engine {

	export class IPlaneSelector
	{
	public:
		virtual std::vector<Plane> selectRepresentativePlanes(Mesh mesh) = 0;
		virtual ~IPlaneSelector() = default;
	};
}