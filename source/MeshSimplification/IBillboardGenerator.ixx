module;
#pragma once
export module IBillboardGenerator;
import std;
import Plane;
import Billboard;
import Mesh;
import Face;

namespace Engine {

	export class IBillboardGenerator
	{
	public:
		virtual std::vector<Billboard> generateBillboards(std::vector<Plane> planes, Mesh mesh) = 0;
		virtual ~IBillboardGenerator() = default;
		

	};
}