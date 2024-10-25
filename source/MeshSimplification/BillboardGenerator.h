#pragma once
#include<vector>
#include"Plane.h"
#include"Billboard.h"
#include"../Mesh.h"
#include"../Face.h"
#include"IBillboardGenerator.h"

class BillboardGenerator : public IBillboardGenerator
{
public:
	std::vector<Billboard> generateBillboards(std::vector<Plane> planes, Mesh mesh) override;
};

