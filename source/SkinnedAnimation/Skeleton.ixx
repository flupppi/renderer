// This is a personal academic project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com
module;
#include <glm/glm.hpp>

export module Skeleton;
import Joint;
import std;
namespace Engine {

	export class Skeleton {
	public:
		Skeleton();

		int GetJointIndex(const std::string& name) const;
		void SetJointNameToIndex();
		float expression = 0.5f;
		float targetExpression = 0.5f;
		glm::mat4 GetGlobalTransformationMatrix(const Joint& joint) const;
		glm::mat4 GetGlobalWorldBindPoseMatrix(const Joint& joint) const;
		std::vector<Joint> m_joints;
		std::map<std::string, int> m_joint_name_to_index;
	};

	Skeleton::Skeleton() {};

	int Skeleton::GetJointIndex(const std::string& name) const
	{
		auto it = m_joint_name_to_index.find(name);
		if (it != m_joint_name_to_index.end()) {
			return it->second;
		}
		return -1;
	}

	void Skeleton::SetJointNameToIndex()
	{
		for (int i = 0; i < m_joints.size(); i++) {
			m_joint_name_to_index[m_joints[i].m_name] = i;
		}
	}

	glm::mat4 Skeleton::GetGlobalTransformationMatrix(const Joint& joint) const
	{
		// non-recursive calculation of the global Transformation Matrix
		glm::mat4 transform = joint.m_localBindPoseMatrix * joint.m_localTransformationMatrix;
		Joint* current = joint.m_parent;
		while (current) {
			glm::mat4 currentTransform = current->m_localBindPoseMatrix * current->m_localTransformationMatrix;
			transform = currentTransform * transform;
			current = current->m_parent;
		}
		return transform;
	}

	glm::mat4 Skeleton::GetGlobalWorldBindPoseMatrix(const Joint& joint) const
	{
		// non-recursive calculation of the global World-Bind-Pose-Matrix
		glm::mat4 transform = joint.m_localBindPoseMatrix;
		Joint* current = joint.m_parent;
		while (current) {
			transform = current->m_localBindPoseMatrix * transform;
			current = current->m_parent;
		}
		return transform;
	}



}