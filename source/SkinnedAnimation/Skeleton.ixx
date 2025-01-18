// This is a personal academic project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com
module;
#pragma once
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
}