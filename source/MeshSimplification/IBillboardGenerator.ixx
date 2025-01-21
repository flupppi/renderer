module;
#pragma once
export module IBillboardGenerator;
import std;
import Plane;
import Billboard;
import Mesh;
import Face;

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