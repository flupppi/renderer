#pragma once
#include<vector>
#include"Plane.h"
#include"../Mesh.h"
namespace Engine {

	class IPlaneSelector
	{
	public:
		virtual std::vector<Plane> selectRepresentativePlanes(Mesh mesh) = 0;
		virtual ~IPlaneSelector() = default;
	};
}