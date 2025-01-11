#pragma once
#include<vector>
#include"Plane.h"
#include"Billboard.h"
#include"../Mesh.h"
#include"../Face.h"
namespace Engine {

	class IBillboardGenerator
	{
	public:
		virtual std::vector<Billboard> generateBillboards(std::vector<Plane> planes, Mesh mesh) = 0;
		virtual ~IBillboardGenerator() = default;

	};
}