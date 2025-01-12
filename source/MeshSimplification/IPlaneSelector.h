#pragma once
#include<vector>
import Plane;
import Mesh;

namespace Engine {

	class IPlaneSelector
	{
	public:
		virtual std::vector<Plane> selectRepresentativePlanes(Mesh mesh) = 0;
		virtual ~IPlaneSelector() = default;
	};
}