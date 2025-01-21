module;

export module PlaneSpace;

import std;
import Plane;
namespace Engine {

	export class PlaneSpace {
		std::vector<std::vector<Plane>> grid; //Store the grid of planes in spherical coordinates
		
		std::vector<float> binDensities;// Store density values for each bin in the grid
		void InitializeGrid(int resolution);
		Plane findDensestPlane();
		//Update the density of bins as faces are collapsed onto planes
		void updateDensity(const Plane& plane);
		// Fetch neighboring bins for adaptive refinement
		std::vector<Plane> getNeighbors(const Plane& bin);
	};

	void PlaneSpace::InitializeGrid(int resolution)
	{
	}

	Plane PlaneSpace::findDensestPlane()
	{
		return Plane();
	}
	void PlaneSpace::updateDensity(const Plane& plane)
	{
	}
	std::vector<Plane> PlaneSpace::getNeighbors(const Plane& bin)
	{
		return std::vector<Plane>();
	}
}