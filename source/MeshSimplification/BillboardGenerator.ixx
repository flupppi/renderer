module;
#include<glm/glm.hpp>
export module BillboardGenerator;
import std;
import Plane;
import Billboard;
import Mesh;
import Face;
import IBillboardGenerator;
import BoundingBox;


namespace Engine {

	export class BillboardGenerator : public IBillboardGenerator
	{

	private:
		unsigned int textureWidth;
		unsigned int textureHeight;
		bool enableAlphaBlending;
		glm::vec4 backgroundColor;
	public:
		BillboardGenerator(unsigned int textureWidth = 512, unsigned int textureHeight = 512, bool enableAlphaBlending = true, glm::vec4 backgroundColor = { 0, 0, 0, 1 })
			: textureWidth(textureWidth), textureHeight(textureHeight), enableAlphaBlending(enableAlphaBlending), backgroundColor(backgroundColor) {
		}
		~BillboardGenerator() = default;
		auto generateBillboards(std::vector<Plane> planes, Mesh mesh) -> std::vector<Billboard>;
		BoundingBox calculateBoundingRectangle(const Plane& plane, const Mesh& mesh);
		Texture renderTexture(const Plane& plane, const Mesh& mesh);
		Billboard createBillboard(const Plane& plane, const Texture& texture);

	};

	// Define, Render and Create the billboards and return an assemled Billboard cloud.
	auto BillboardGenerator::generateBillboards(std::vector<Plane> planes, Mesh mesh) -> std::vector<Billboard> {
		return std::vector<Billboard>();

	}
	// 4. Calculate the bounding rectangle of the billboard based plane and mesh.
	BoundingBox BillboardGenerator::calculateBoundingRectangle(const Plane& plane, const Mesh& mesh)
	{
		return BoundingBox();
	}
	// 5. Render the texture for the billboard withing the Bounds of the Bounding Box.
	Texture BillboardGenerator::renderTexture(const Plane& plane, const Mesh& mesh)
	{
		return Texture();
	}

	// 6. Map the texture onto the plane to create the billboard.
	Billboard BillboardGenerator::createBillboard(const Plane& plane, const Texture& texture)
	{
		return Billboard();
	}


}