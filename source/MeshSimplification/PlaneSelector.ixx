module;
#include<glm/glm.hpp>
#pragma once
export module PlaneSelector;
import std;
import IPlaneSelector;
import Plane;
import Mesh;
import Model;

namespace Engine {
	/*
	 * @brief Interface for selecting representative planes
	 * Here a 3D model is passed and the representative planes optimally covering the object for the billboard cloud are returned.
	 */
	export class PlaneSelector : public IPlaneSelector
	{
	private:
		float errorThreshold;
		int resolution;
		float penaltyWeight;
		float contributionWeight;
		glm::vec3 referenceViewpoint;
	public:
		PlaneSelector(float errorThreshold = 0.01, int resolution = 32, float penaltyWeight = 1.0, float contributionWeight = 1.0, glm::vec3 referenceViewpoint = { 0, 0, 0 })
			: errorThreshold(errorThreshold), resolution(resolution), penaltyWeight(penaltyWeight), contributionWeight(contributionWeight), referenceViewpoint(referenceViewpoint) {
		}		
		~PlaneSelector() = default;
		std::vector<Plane> selectRepresentativePlanes(Model model) override;
		void computeMetrics(const Model& model);
		void calculateDensity(const Model& model);
		Plane refineBin(const Plane& bin, const Mesh& mesh);
		void updateDensity(const Plane& plane, const Mesh& mesh);
	};

	// Return representative planes by computing metrics and greedily selecting the densest planes.
	std::vector<Plane> PlaneSelector::selectRepresentativePlanes(Model model)
	{
		
		// Placeholder
		return std::vector<Plane>();
	}

	// Firstly Pre-process the model to compute the metrics for each face
	void PlaneSelector::computeMetrics(const Model& model)
	{
		// Placeholder
	}
	// Secondly Calculate the density of each bin by discretizing the plane space
	void PlaneSelector::calculateDensity(const Model& model)
	{
		// Placeholder
	}
	// Thirdly Greedily efine the bin by selecting the plane with the highest density
	Plane PlaneSelector::refineBin(const Plane& bin, const Mesh& mesh)
	{
		// Placeholder
		return Plane();
	}
	// Update the density of the bin after refining
	void PlaneSelector::updateDensity(const Plane& plane, const Mesh& mesh)
	{
		// Placeholder
	}
	


}