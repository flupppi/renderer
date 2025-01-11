// This is a personal academic project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com
#pragma once
#include <glm/glm.hpp>
#include <vector>
#include <string>
#include <iostream>
namespace Engine {

	class Joint {

	public:
		Joint();
		Joint(const std::string& name, const glm::mat4& matrix, Joint* parent = nullptr);
		void SetLocalTransform(glm::mat4 transformOperation);
		std::string m_name;
		glm::mat4 m_localBindPoseMatrix{};
		glm::mat4 m_localTransformationMatrix{};
		glm::mat4 m_targetTransformationMatrix{};
		Joint* m_parent;
		std::vector<Joint*> children_;
	};
}