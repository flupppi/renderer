module;
export module IPlaneSelector;
import std;
import Plane;
import Model;

namespace Engine {
	/*
  * @brief Interface for selecting representative planes
  */
	export class IPlaneSelector
	{
	public:
		virtual ~IPlaneSelector() = default;
		virtual std::vector<Plane> selectRepresentativePlanes(Model model) = 0;
	};
}