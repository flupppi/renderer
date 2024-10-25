#pragma once
#include "GameInterface.h"
#include "Renderer.h"
#include "Joint.h"
#include "Skeleton.h"
#include <glm/ext/quaternion_float.hpp>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <iostream>
#include "InputSystem.h"
#include "Quad.h"
class BillboardCloud : public GameInterface
{
public:
	void Initialize(GLFWwindow* window) override;
	void BuildSkeleton();
	void Render(float aspectRatio) override;
	void ClearResources() override;
	void Update(double deltaTime) override;
	Skeleton m_skeleton;
	//PoseManager m_poseManager;
	GLfloat lightPos[3] = { 15.0f, 1.0f, 15.0f };
	GLfloat lightColor[3] = { 1.0f, 1.0f, 1.0f };

private:
	float t = 0;
	Renderer m_renderer;
	InputSystem m_input;
	float deltaTime_ = 0.0f;
	glm::quat m_orientationQuaternion = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);;
	float m_currentAngle = 0.0f;
	float m_transitionTime = 0.0f;
	const float TRANSITION_DURATION = 10.0f;
};

