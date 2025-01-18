module;
#pragma once
export module BillboardGenerator;
import std;
import Plane;
import Billboard;
import Mesh;
import Face;
import IBillboardGenerator;
namespace Engine {

	export class BillboardGenerator : public IBillboardGenerator
	{
	public:
		auto generateBillboards(std::vector<Plane> planes, Mesh mesh) -> std::vector<Billboard> {
			return std::vector<Billboard>();

		}
	};

}