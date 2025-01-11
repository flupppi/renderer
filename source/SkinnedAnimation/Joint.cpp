// This is a personal academic project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com
#include "Joint.h"
namespace Engine {
	//************************************
	// Sets transformation matrices to Identity Matrix, parent Null
	//************************************
	Joint::Joint()
	{
		m_localBindPoseMatrix = glm::mat4(1.0f);
		m_localTransformationMatrix = glm::mat4(1.0f);
		m_targetTransformationMatrix = glm::mat4(1.0f);
		m_parent = nullptr;
	}

	//************************************
	// Construct Joint from name, bind-pose matrix and parent.
	//************************************
	Joint::Joint(const std::string& name, const glm::mat4& matrix, Joint* parent /*= nullptr*/)
	{

		this->m_name = name;
		this->m_localBindPoseMatrix = matrix;
		this->m_parent = parent;
		this->m_localTransformationMatrix = glm::mat4(1.0f);
		this->m_targetTransformationMatrix = glm::mat4(1.0f);
		//// Print the parent joint's name, if it exists
		//if (m_parent != nullptr)
		//{
		//	std::cerr << "Parent joint's name is: " << m_parent->m_name << std::endl;
		//}
		//else
		//{
		//	std::cerr << "Parent joint's name is: " << this->m_name << std::endl;

		//}

	}

	//************************************
	// set local transform to change joint orientation.
	//************************************
	void Joint::SetLocalTransform(glm::mat4 matrix)
	{
		this->m_localTransformationMatrix = matrix;
	}

}