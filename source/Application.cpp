// This is a personal academic project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com
#include "Application.h"

//************************************
// Build Skeleton from Joints using hard coded transformation matrices and store their relationships. 
// Return vector of joints in correct joint order.
//************************************
void Application::BuildSkeleton()
{
	std::string joint_name[] = { std::string("NeckJoint"), std::string("HeadJoint"),std::string("LtUpperLidJoint"), std::string("RtUpperLidJoint"), std::string("LtLowerLidJoint"), std::string("RtLowerLidJoint"),std::string("JawJoint"), std::string("LtEyeJoint"), std::string("RtEyeJoint") };
	m_skeleton.m_joints.reserve(9);
	// Neck Joint
	glm::mat4 neck_joint_matrix(0, 0.997385, 0.0722743, 0, 0, 0.0722743, -0.997385, 0, -1, 0, 0, 0, 0, -13.942331, -1.898722, 1);
	Joint neck_joint(joint_name[0], neck_joint_matrix, nullptr);
	neck_joint.children_.reserve(1);
	m_skeleton.m_joints.push_back(neck_joint);
	// HeadJoint
	glm::mat4 head_joint_matrix(0.982982, 0.183701, 0, 0, 0, 0, -1, 0, -0.183701, 0.982982, 0, 0, 3.529217, 0, 0, 1);
	Joint head_joint(joint_name[1], head_joint_matrix, &m_skeleton.m_joints.at(0));
	head_joint.children_.reserve(7);
	m_skeleton.m_joints.push_back(head_joint);
	// LtUpperLidJoint
	glm::mat4 lt_upper_lid_joint_matrix(0.517129, 0, -0.855907, 0, 0.855907, 0, 0.517129, 0, 0, -1, 0, 0, 10.258537, 3.271436, -4.268526, 1);
	Joint lt_upper_lid_joint(joint_name[2], lt_upper_lid_joint_matrix, &m_skeleton.m_joints.at(1));
	m_skeleton.m_joints.push_back(lt_upper_lid_joint);
	m_skeleton.m_joints.at(1).children_.push_back(&m_skeleton.m_joints.at(2));
	// RtUpperLidJoint
	glm::mat4 rt_upper_lid_joint_matrix(-0.517129, 0, 0.855907, 0, -0.855907, 0, -0.517129, 0, 0, -1, 0, 0, 10.258537, -3.27144, -4.268522, 1);
	Joint rt_upper_lid_joint(joint_name[3], rt_upper_lid_joint_matrix, &m_skeleton.m_joints.at(1));
	m_skeleton.m_joints.push_back(rt_upper_lid_joint);
	m_skeleton.m_joints.at(1).children_.push_back(&m_skeleton.m_joints.at(3));
	// LtLowerLidJoint
	glm::mat4 lt_lower_lid_joint_matrix(-0.487324, 0, -0.873221, 0, 0.873221, 0, -0.487324, 0, 0, -1, 0, 0, 10.252075, 3.270312, -4.268398, 1);
	Joint lt_lower_lid_joint(joint_name[4], lt_lower_lid_joint_matrix, &m_skeleton.m_joints.at(1));
	m_skeleton.m_joints.push_back(lt_lower_lid_joint);
	m_skeleton.m_joints.at(1).children_.push_back(&m_skeleton.m_joints.at(4));
	// RtLowerLidJoint
	glm::mat4 rt_lower_lid_joint_matrix(0.487324, 0, 0.873221, 0, -0.873221, 0, 0.487324, 0, 0, -1, 0, 0, 10.252076, -3.27031, -4.268396, 1);
	Joint rt_lower_lid_joint(joint_name[5], rt_lower_lid_joint_matrix, &m_skeleton.m_joints.at(1));
	m_skeleton.m_joints.push_back(rt_lower_lid_joint);
	m_skeleton.m_joints.at(1).children_.push_back(&m_skeleton.m_joints.at(5));
	// JawJoint
	glm::mat4 jaw_joint_matrix(-0.640359, 0, -0.768076, 0, 0.768076, 0, -0.640359, 0, 0, -1, 0, 0, 6.914116, 0, -0.723195, 1);
	Joint jaw_joint(joint_name[6], jaw_joint_matrix, &m_skeleton.m_joints.at(1));
	m_skeleton.m_joints.push_back(jaw_joint);
	m_skeleton.m_joints.at(1).children_.push_back(&m_skeleton.m_joints.at(6));
	// LtEyeJoint
	glm::mat4 lt_eye_joint_matrix(-0.0874302, 0, -0.996171, 0, 0.996171, 0, -0.0874302, 0, 0, -1, 0, 0, 10.252075, 3.270312, -4.268398, 1);
	Joint lt_eye_joint(joint_name[7], lt_eye_joint_matrix, &m_skeleton.m_joints.at(1));
	m_skeleton.m_joints.push_back(lt_eye_joint);
	m_skeleton.m_joints.at(1).children_.push_back(&m_skeleton.m_joints.at(7));
	// RtEyeJoint
	glm::mat4 rt_eye_joint_matrix(-0.0874302, 0, -0.996171, 0, 0.996171, 0, -0.0874302, 0, 0, -1, 0, 0, 10.258537, -3.27144, -4.268522, 1);
	Joint rt_eye_joint(joint_name[8], rt_eye_joint_matrix, &m_skeleton.m_joints.at(1));
	m_skeleton.m_joints.push_back(rt_eye_joint);
	m_skeleton.m_joints.at(1).children_.push_back(&m_skeleton.m_joints.at(8));
	m_skeleton.m_joints.at(0).children_.push_back(&m_skeleton.m_joints.at(1));
	m_skeleton.SetJointNameToIndex();
}
//************************************
// Set up Keyboard Observer, Initialize the Renderer and Initialize the Skeleton.
//************************************
void Application::Initialize(GLFWwindow* window)
{
	m_input.SetWindow(window);
	// Observe Input
	m_input.ObserveKey(GLFW_KEY_SPACE);
	m_input.ObserveKey(GLFW_KEY_RIGHT);
	m_input.ObserveKey(GLFW_KEY_LEFT);
	m_input.ObserveKey(GLFW_KEY_UP);
	m_input.ObserveKey(GLFW_KEY_DOWN);
	m_input.ObserveKey(GLFW_KEY_9);
	m_input.ObserveKey(GLFW_KEY_8);
	m_input.ObserveKey(GLFW_KEY_7);
	m_input.ObserveKey(GLFW_KEY_6);
	m_input.ObserveKey(GLFW_KEY_4);
	m_input.ObserveKey(GLFW_KEY_3);
	m_input.ObserveKey(GLFW_KEY_2);
	m_input.ObserveKey(GLFW_KEY_1);
	m_input.ObserveKey(GLFW_KEY_R);
	m_input.ObserveKey(GLFW_KEY_LEFT_SHIFT);
	
	m_currentAngle = 0.0f;
	m_orientationQuaternion = glm::quat(1.0f, glm::vec3(0.0f, 0.0f, 0.0f));
	m_renderer.Initialize();
	m_currentAngle = 0.0f;

	BuildSkeleton();
}
//************************************
// Calculate mvp matrix, calculate and render joint transforms and calculate and render skin using the boneModelMatrices.
//************************************
void Application::Render(float aspectRatio)
{
	glm::mat4 Projection = glm::perspective(glm::radians(45.0f), aspectRatio, 0.1f, 100.0f);
	glm::mat4 View = glm::lookAt(glm::vec3(0.0f, 3.0f, 10.0f),glm::vec3(0.0f, 0.0f, 0.0f),glm::vec3(0.0f, 1.0f, 0.0f));
	glm::quat transformOpertation = glm::angleAxis(0.0f, glm::vec3(0.0f, 1.0f, 0.0f));
	glm::quat model_quat = m_orientationQuaternion * transformOpertation;
	glm::mat4 Model = glm::mat4_cast(model_quat);      
	glm::mat4 mvp = Projection * View * Model;

	// Render Skin Mesh using the boneModelMatrices.
	std::vector<glm::mat4> boneModelMatrices;
	boneModelMatrices.reserve(9);

	glm::mat4 quadTransform = mvp * glm::mat4(1.0f);

	Quad quad(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(3.0f, 0.0f, 0.0f), glm::vec3(2.0f, 2.0f, 0.0f), glm::vec3(0.0f, 2.0f, 0.0f));
	m_renderer.InitQuad(quad);
	m_renderer.RenderQuad(quadTransform);
}

//************************************
// Calculate mvp matrix, calculate and render joint transforms and calculate and render skin using the boneModelMatrices.
//************************************
void Application::ClearResources()
{
	m_renderer.ClearResources();
}

//************************************
// Handle Input and Update Animation
//************************************
void Application::Update(double deltaTime)
{
	m_input.Update();

	// Keyboard Rotation
	float xVel = 0.0f;
	if (m_input.IsKeyDown(GLFW_KEY_UP))
		xVel = glm::radians(90.0f);
	if (m_input.IsKeyDown(GLFW_KEY_DOWN))
		xVel = glm::radians(-90.0f);
	float yVel = 0.0f;
	if (m_input.IsKeyDown(GLFW_KEY_RIGHT))
		yVel = glm::radians(90.0f);
	if (m_input.IsKeyDown(GLFW_KEY_LEFT))
		yVel = glm::radians(-90.0f);

}
