#pragma once
#include<vector>
import Plane;
import Mesh;

#include"IPlaneSelector.h"

namespace Engine {

	class PlaneSelector : public IPlaneSelector
	{
	public:
		std::vector<Plane> selectRepresentativePlanes(Mesh mesh) override;
	};

}