#pragma once
#include<vector>
import Plane;
import Billboard;
import Mesh;
import Face;
#include"IBillboardGenerator.h"
namespace Engine {

	class BillboardGenerator : public IBillboardGenerator
	{
	public:
		auto generateBillboards(std::vector<Plane> planes, Mesh mesh) -> std::vector<Billboard> override ;
	};

}