module;
#include<glm/glm.hpp>
export module IBillboardGenerator;

import Plane;
import Billboard;
import Mesh;
import Face;
import std;
namespace Engine {
	/*
	 * @brief Interface for generating billboards
	 * Here the finished selection of planes are passed along with the mesh, and the billboards are rendered and returned.
	 */
	export class IBillboardGenerator
	{
	public:
		virtual std::vector<Billboard> generateBillboards(std::vector<Plane> planes, Mesh mesh) = 0;
		virtual ~IBillboardGenerator() = default;
		

	};
}