#pragma once
#include<vector>
import Plane;
import Billboard;
import Mesh;
import Face;

namespace Engine {

	class IBillboardGenerator
	{
	public:
		virtual std::vector<Billboard> generateBillboards(std::vector<Plane> planes, Mesh mesh) = 0;
		virtual ~IBillboardGenerator() = default;


	};
}