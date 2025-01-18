module;
#pragma once
export module PlaneSelector;
import std;
import IPlaneSelector;
import Plane;
import Mesh;
namespace Engine {

	export class PlaneSelector : public IPlaneSelector
	{
	public:
		std::vector<Plane> selectRepresentativePlanes(Mesh mesh) {
			return std::vector<Plane>();

		}
	};

}