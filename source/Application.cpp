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
	glm::mat4 View = glm::lookAt(glm::vec3(0.0f, 3.0f, 50.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	glm::quat transformOpertation = glm::angleAxis(0.0f, glm::vec3(0.0f, 1.0f, 0.0f));
	glm::quat model_quat = m_orientationQuaternion * transformOpertation;
	glm::mat4 Model = glm::mat4_cast(model_quat);
	glm::mat4 mvp = Projection * View * Model;

	// Render Skin Mesh using the boneModelMatrices.
	std::vector<glm::mat4> boneModelMatrices;
	boneModelMatrices.reserve(9);
	for (const auto& Joint : m_skeleton.m_joints)
	{
		glm::mat4 globalWorldBindPoseMatrix = m_skeleton.GetGlobalWorldBindPoseMatrix(Joint);
		glm::mat4 globalTransformationMatrix = m_skeleton.GetGlobalTransformationMatrix(Joint);
		glm::mat4 inverseGlobalWorldBindPoseMatrix = glm::inverse(globalWorldBindPoseMatrix);
		glm::mat4 boneModelMatrix = globalTransformationMatrix * inverseGlobalWorldBindPoseMatrix;
		glm::mat4 boneMVP = Projection * View * glm::mat4_cast(m_orientationQuaternion) * boneModelMatrix;
		boneModelMatrices.push_back(boneMVP);
	}

	// Render skeleton Joint Squares
	m_renderer.RenderSkin(mvp, boneModelMatrices, m_skeleton.expression, lightColor, lightPos);
	for (const auto& joint : m_skeleton.m_joints) {
		glm::mat4 joint_matrix = m_skeleton.GetGlobalTransformationMatrix(joint);
		glm::mat4 jointTransform = mvp * joint_matrix;
		m_renderer.RenderSkeleton(jointTransform);
	}

	RenderIMGui();

}

void Application::RenderIMGui()
{

	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
	// 2. Show a simple window that we create ourselves. We use a Begin/End pair to create a named window.
	{
		static float xTurn = 0.0f;
		static float yBend = 0.0f;
		static float zBow = 0.0f;
		static float yOpen = 0.0f;
		static float xJut = 0.0f;
		static float zlookUDleft = 0.0f;
		static float ylookLRleft = 0.0f;
		static float zlookUDright = 0.0f;
		static float ylookLRright = 0.0f;
		static float zLtUpperLid = 0.0f;
		static float zLtLowerLid = 0.0f;
		static float zRtUpperLid = 0.0f;
		static float zRtLowerLid = 0.0f;
		static float expression = 0.5f;

		bool save = false;

		ImGui::Begin("Animation Controls");

		ImGui::Text("edit animation poses by moving the sliders");
		ImGui::Checkbox("Edit Animations", &edit_state);      // Edit bools storing our window open/close state
		if (edit_state) {
			static int item_current = 1;

			// Using the _simplified_ one-liner ListBox() api here
			// See "List boxes" section for examples of how to use the more flexible BeginListBox()/EndListBox() api.
			ImGui::Text("Head");
			ImGui::SliderFloat("Turn", &xTurn, -90.0f, 90.0f);
			ImGui::SliderFloat("Bend", &yBend, -40.0f, 40.0f);
			ImGui::SliderFloat("Bow", &zBow, -50.0f, 50.0f);
			ImGui::Text("Jaw");
			ImGui::SliderFloat("Open", &yOpen, -20.0f, 0.0f);
			ImGui::SliderFloat("Jut", &xJut, -8.0f, 8.0f);
			ImGui::Text("Left Eye");
			ImGui::SliderFloat("Look Up-Down (left)", &zlookUDleft, -25.0f, 25.0f);
			ImGui::SliderFloat("Look Left-Right (left)", &ylookLRleft, -25.0f, 25.0f);
			ImGui::SliderFloat("Open-Close Upper Eye Lid (left)", &zLtUpperLid, -45.0f, 10.0f);
			ImGui::SliderFloat("Open-Close Lower Eye Lid (left)", &zLtLowerLid, -5.0f, 25.0f);
			ImGui::Text("Right Eye");
			ImGui::SliderFloat("Look Up-Down (right)", &zlookUDright, -25.0f, 25.0f);
			ImGui::SliderFloat("Look Left-Right (right)", &ylookLRright, -25.0f, 25.0f);
			ImGui::SliderFloat("Open-Close Upper Eye Lid (right)", &zRtUpperLid, -45.0f, 10.0f);
			ImGui::SliderFloat("Open-Close Lower Eye Lid (right)", &zRtLowerLid, -5.0f, 25.0f);
			ImGui::Text("Expression");
			ImGui::SliderFloat("Smile - Frown", &expression, 0.0f, 1.0f);



			glm::quat MTurn = glm::angleAxis(glm::radians(xTurn), glm::vec3(1.0f, 0.0f, 0.0f));
			glm::quat MBow = glm::angleAxis(glm::radians(yBend), glm::vec3(0.0f, 1.0f, 0.0f));
			glm::quat MBend = glm::angleAxis(glm::radians(zBow), glm::vec3(0.0f, 0.0f, 1.0f));
			glm::quat MOpen = glm::angleAxis(glm::radians(yOpen), glm::vec3(0.0f, 0.0f, 1.0f));
			glm::quat MJut = glm::angleAxis(glm::radians(xJut), glm::vec3(0.0f, 1.0f, 0.0f));
			glm::quat MlookUDleft = glm::angleAxis(glm::radians(zlookUDleft), glm::vec3(0.0f, 0.0f, 1.0f));
			glm::quat MlookLRleft = glm::angleAxis(glm::radians(ylookLRleft), glm::vec3(0.0f, 1.0f, 0.0f));
			glm::quat MlookUDright = glm::angleAxis(glm::radians(zlookUDright), glm::vec3(0.0f, 0.0f, 1.0f));
			glm::quat MlookLRright = glm::angleAxis(glm::radians(ylookLRright), glm::vec3(0.0f, 1.0f, 0.0f));
			glm::quat LLtUpperLid = glm::angleAxis(glm::radians(zLtUpperLid), glm::vec3(0.0f, 0.0f, 1.0f));
			glm::quat LLtLowerLid = glm::angleAxis(glm::radians(zLtLowerLid), glm::vec3(0.0f, 0.0f, 1.0f));
			glm::quat LRtUpperLid = glm::angleAxis(glm::radians(zRtUpperLid), glm::vec3(0.0f, 0.0f, 1.0f));
			glm::quat LRtLowerLid = glm::angleAxis(glm::radians(zRtLowerLid), glm::vec3(0.0f, 0.0f, 1.0f));


			glm::mat4 LHead = glm::mat4_cast(MTurn * MBend * MBow);
			glm::mat4 LJaw = glm::mat4_cast(MOpen * MJut);
			glm::mat4 LLeftEye = glm::mat4_cast(MlookLRleft * MlookUDleft);
			glm::mat4 LRightEye = glm::mat4_cast(MlookLRright * MlookUDright);
			glm::mat4 LtLowerLid = glm::mat4_cast(LLtLowerLid);
			glm::mat4 LtUpperLid = glm::mat4_cast(LLtUpperLid);
			glm::mat4 RtLowerLid = glm::mat4_cast(LRtLowerLid);
			glm::mat4 RtUpperLid = glm::mat4_cast(LRtUpperLid);


			m_skeleton.m_joints[1].m_targetTransformationMatrix = LHead;
			m_skeleton.m_joints[6].m_targetTransformationMatrix = LJaw;
			m_skeleton.m_joints[7].m_targetTransformationMatrix = LLeftEye;
			m_skeleton.m_joints[8].m_targetTransformationMatrix = LRightEye;
			m_skeleton.m_joints[4].m_targetTransformationMatrix = LtLowerLid;
			m_skeleton.m_joints[2].m_targetTransformationMatrix = LtUpperLid;
			m_skeleton.m_joints[5].m_targetTransformationMatrix = RtLowerLid;
			m_skeleton.m_joints[3].m_targetTransformationMatrix = RtUpperLid;
			m_skeleton.targetExpression = expression;
		}
		{
			ImGui::Text("Light Color");
			ImGui::ColorEdit3("clear color", lightColor); // Edit 3 floats representing a color
			ImGui::Text("Light Position");
			ImGui::SliderFloat("X-Position", &lightPos[0], -25.0f, 25.0f);
			ImGui::SliderFloat("Y-Position", &lightPos[1], -25.0f, 25.0f);
			ImGui::SliderFloat("Z-Position", &lightPos[2], -25.0f, 25.0f);
		}
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	}
	ImGui::End();
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
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

	//Camera Reset
	if (m_input.WasKeyPressed(GLFW_KEY_R))
		m_orientationQuaternion = glm::quat(1.0f, glm::vec3(0.0f, 0.0f, 0.0f));


	//Transform a local Transformation matrix to change something in the model
	// Later we can do this through poses or a slider.
	// Now we just have to bind everything to the shader and fill in the code inside the shader. 
	// Then we can manually start transforming each element

			// Set standard pose for key 1
	if (m_input.WasKeyPressed(GLFW_KEY_1)) {
		glm::quat MTurn = glm::angleAxis(glm::radians(-5.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		glm::quat MBow = glm::angleAxis(glm::radians(2.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		glm::quat MBend = glm::angleAxis(glm::radians(-10.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		glm::quat MOpen = glm::angleAxis(glm::radians(-5.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		glm::quat MJut = glm::angleAxis(glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		glm::quat MlookUDleft = glm::angleAxis(glm::radians(3.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		glm::quat MlookLRleft = glm::angleAxis(glm::radians(3.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		glm::quat MlookUDright = glm::angleAxis(glm::radians(3.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		glm::quat MlookLRright = glm::angleAxis(glm::radians(3.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		glm::quat LLtUpperLid = glm::angleAxis(glm::radians(-5.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		glm::quat LLtLowerLid = glm::angleAxis(glm::radians(3.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		glm::quat LRtUpperLid = glm::angleAxis(glm::radians(-5.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		glm::quat LRtLowerLid = glm::angleAxis(glm::radians(5.0f), glm::vec3(0.0f, 0.0f, 1.0f));

		float exp = 0.0;
		glm::mat4 LHead = glm::mat4_cast(MTurn * MBend * MBow);
		glm::mat4 LJaw = glm::mat4_cast(MOpen * MJut);
		glm::mat4 LLeftEye = glm::mat4_cast(MlookLRleft * MlookUDleft);
		glm::mat4 LRightEye = glm::mat4_cast(MlookLRright * MlookUDright);
		glm::mat4 LtLowerLid = glm::mat4_cast(LLtLowerLid);
		glm::mat4 LtUpperLid = glm::mat4_cast(LLtUpperLid);
		glm::mat4 RtLowerLid = glm::mat4_cast(LRtLowerLid);
		glm::mat4 RtUpperLid = glm::mat4_cast(LRtUpperLid);
		m_transitionTime = 0.0f;
		m_skeleton.m_joints[1].m_targetTransformationMatrix = LHead;
		m_skeleton.m_joints[6].m_targetTransformationMatrix = LJaw;
		m_skeleton.m_joints[7].m_targetTransformationMatrix = LLeftEye;
		m_skeleton.m_joints[8].m_targetTransformationMatrix = LRightEye;
		m_skeleton.m_joints[4].m_targetTransformationMatrix = LtLowerLid;
		m_skeleton.m_joints[2].m_targetTransformationMatrix = LtUpperLid;
		m_skeleton.m_joints[5].m_targetTransformationMatrix = RtLowerLid;
		m_skeleton.m_joints[3].m_targetTransformationMatrix = RtUpperLid;
		m_skeleton.targetExpression = exp;

	}
	// Set standard pose for key 2
	if (m_input.WasKeyPressed(GLFW_KEY_2)) {
		glm::quat MTurn = glm::angleAxis(glm::radians(2.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		glm::quat MBow = glm::angleAxis(glm::radians(-3.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		glm::quat MBend = glm::angleAxis(glm::radians(-0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		glm::quat MOpen = glm::angleAxis(glm::radians(-8.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		glm::quat MJut = glm::angleAxis(glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		glm::quat MlookUDleft = glm::angleAxis(glm::radians(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		glm::quat MlookLRleft = glm::angleAxis(glm::radians(-4.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		glm::quat MlookUDright = glm::angleAxis(glm::radians(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		glm::quat MlookLRright = glm::angleAxis(glm::radians(-4.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		glm::quat LLtUpperLid = glm::angleAxis(glm::radians(9.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		glm::quat LLtLowerLid = glm::angleAxis(glm::radians(-3.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		glm::quat LRtUpperLid = glm::angleAxis(glm::radians(9.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		glm::quat LRtLowerLid = glm::angleAxis(glm::radians(-3.0f), glm::vec3(0.0f, 0.0f, 1.0f));

		float exp = 0.5;
		glm::mat4 LHead = glm::mat4_cast(MTurn * MBend * MBow);
		glm::mat4 LJaw = glm::mat4_cast(MOpen * MJut);
		glm::mat4 LLeftEye = glm::mat4_cast(MlookLRleft * MlookUDleft);
		glm::mat4 LRightEye = glm::mat4_cast(MlookLRright * MlookUDright);
		glm::mat4 LtLowerLid = glm::mat4_cast(LLtLowerLid);
		glm::mat4 LtUpperLid = glm::mat4_cast(LLtUpperLid);
		glm::mat4 RtLowerLid = glm::mat4_cast(LRtLowerLid);
		glm::mat4 RtUpperLid = glm::mat4_cast(LRtUpperLid);
		m_transitionTime = 0.0f;
		m_skeleton.m_joints[1].m_targetTransformationMatrix = LHead;
		m_skeleton.m_joints[6].m_targetTransformationMatrix = LJaw;
		m_skeleton.m_joints[7].m_targetTransformationMatrix = LLeftEye;
		m_skeleton.m_joints[8].m_targetTransformationMatrix = LRightEye;
		m_skeleton.m_joints[4].m_targetTransformationMatrix = LtLowerLid;
		m_skeleton.m_joints[2].m_targetTransformationMatrix = LtUpperLid;
		m_skeleton.m_joints[5].m_targetTransformationMatrix = RtLowerLid;
		m_skeleton.m_joints[3].m_targetTransformationMatrix = RtUpperLid;
		m_skeleton.targetExpression = exp;
	}
	// Set standard pose for key 3
	if (m_input.WasKeyPressed(GLFW_KEY_4)) {
		glm::quat MTurn = glm::angleAxis(glm::radians(-0.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		glm::quat MBow = glm::angleAxis(glm::radians(2.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		glm::quat MBend = glm::angleAxis(glm::radians(-6.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		glm::quat MOpen = glm::angleAxis(glm::radians(-2.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		glm::quat MJut = glm::angleAxis(glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		glm::quat MlookUDleft = glm::angleAxis(glm::radians(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		glm::quat MlookLRleft = glm::angleAxis(glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		glm::quat MlookUDright = glm::angleAxis(glm::radians(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		glm::quat MlookLRright = glm::angleAxis(glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		glm::quat LLtUpperLid = glm::angleAxis(glm::radians(-15.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		glm::quat LLtLowerLid = glm::angleAxis(glm::radians(9.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		glm::quat LRtUpperLid = glm::angleAxis(glm::radians(-15.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		glm::quat LRtLowerLid = glm::angleAxis(glm::radians(9.0f), glm::vec3(0.0f, 0.0f, 1.0f));

		float exp = 1.0f;
		glm::mat4 LHead = glm::mat4_cast(MTurn * MBend * MBow);
		glm::mat4 LJaw = glm::mat4_cast(MOpen * MJut);
		glm::mat4 LLeftEye = glm::mat4_cast(MlookLRleft * MlookUDleft);
		glm::mat4 LRightEye = glm::mat4_cast(MlookLRright * MlookUDright);
		glm::mat4 LtLowerLid = glm::mat4_cast(LLtLowerLid);
		glm::mat4 LtUpperLid = glm::mat4_cast(LLtUpperLid);
		glm::mat4 RtLowerLid = glm::mat4_cast(LRtLowerLid);
		glm::mat4 RtUpperLid = glm::mat4_cast(LRtUpperLid);
		m_transitionTime = 0.0f;
		m_skeleton.m_joints[1].m_targetTransformationMatrix = LHead;
		m_skeleton.m_joints[6].m_targetTransformationMatrix = LJaw;
		m_skeleton.m_joints[7].m_targetTransformationMatrix = LLeftEye;
		m_skeleton.m_joints[8].m_targetTransformationMatrix = LRightEye;
		m_skeleton.m_joints[4].m_targetTransformationMatrix = LtLowerLid;
		m_skeleton.m_joints[2].m_targetTransformationMatrix = LtUpperLid;
		m_skeleton.m_joints[5].m_targetTransformationMatrix = RtLowerLid;
		m_skeleton.m_joints[3].m_targetTransformationMatrix = RtUpperLid;
		m_skeleton.targetExpression = exp;
	}
	// set standard pose for key 4
	if (m_input.WasKeyPressed(GLFW_KEY_3)) {
		m_transitionTime = 0.0f;
		// RtEyeJoint
		m_skeleton.m_joints[8].m_targetTransformationMatrix = glm::mat4(1.0f);
		// RtUpperEyeLid
		m_skeleton.m_joints[3].m_targetTransformationMatrix = glm::mat4(1.0f);
		// RtLowerEyeLid
		m_skeleton.m_joints[5].m_targetTransformationMatrix = glm::mat4(1.0f);
		// LtEyeJoint
		m_skeleton.m_joints[7].m_targetTransformationMatrix = glm::mat4(1.0f);
		// HeadJoint
		m_skeleton.m_joints[1].m_targetTransformationMatrix = glm::mat4(1.0f);
		// JawJoint
		glm::quat transformationOpertation = glm::angleAxis(glm::radians(-2.0f), glm::normalize(glm::vec3(0.0f, 0.0f, 1.0f)));
		m_skeleton.m_joints[6].m_targetTransformationMatrix = glm::mat4_cast(transformationOpertation);
		m_skeleton.targetExpression = 0.9f;
	}


	t = glm::clamp(m_transitionTime / TRANSITION_DURATION, 0.0f, 1.0f);

	for (int i = 0; i < m_skeleton.m_joints.size(); i++) {
		// Calculate interpolated transform using slerp...
		glm::mat4 interpolatedTransform = glm::mat4_cast(glm::slerp(glm::quat_cast(m_skeleton.m_joints[i].m_localTransformationMatrix), glm::quat_cast(m_skeleton.m_joints[i].m_targetTransformationMatrix), t));
		m_skeleton.m_joints[i].m_localTransformationMatrix = interpolatedTransform;
	}
	// Calculate the interpolated facial expression using mix
	m_skeleton.expression = glm::mix(m_skeleton.expression, m_skeleton.targetExpression, t);

	glm::mat4 orientation = mat4_cast(m_orientationQuaternion);
	deltaTime_ = (float)deltaTime;
	glm::quat updatedRotation = glm::quat(0.0f, glm::vec3(xVel, yVel, 0.0f));
	m_orientationQuaternion += 0.5f * ((float)deltaTime) * updatedRotation * m_orientationQuaternion;
	m_orientationQuaternion = normalize(m_orientationQuaternion);
	m_transitionTime += float(deltaTime);
	if (m_transitionTime >= TRANSITION_DURATION) {
		// Reset the target pose as the current pose...
		for (int i = 0; i < m_skeleton.m_joints.size(); i++)
		{
			// RtEyeJoint
			m_skeleton.m_joints[8].m_localTransformationMatrix = m_skeleton.m_joints[8].m_targetTransformationMatrix;
			// RtUpperEyeLid
			m_skeleton.m_joints[3].m_localTransformationMatrix = m_skeleton.m_joints[3].m_targetTransformationMatrix;
			// RtLowerEyeLid
			m_skeleton.m_joints[5].m_localTransformationMatrix = m_skeleton.m_joints[5].m_targetTransformationMatrix;
			// LtEyeJoint
			m_skeleton.m_joints[7].m_localTransformationMatrix = m_skeleton.m_joints[7].m_targetTransformationMatrix;
			// HeadJoint
			m_skeleton.m_joints[1].m_localTransformationMatrix = m_skeleton.m_joints[1].m_targetTransformationMatrix;
			// JawJoint
			m_skeleton.m_joints[6].m_localTransformationMatrix = m_skeleton.m_joints[6].m_targetTransformationMatrix;
			m_skeleton.expression = m_skeleton.targetExpression;
		}

	}
}
