#pragma once
#include<vector>
#include"Plane.h"
#include"../Mesh.h"
#include"IPlaneSelector.h"
class PlaneSelector : public IPlaneSelector
{
public:
	std::vector<Plane> selectRepresentativePlanes(Mesh mesh) override ;
};

