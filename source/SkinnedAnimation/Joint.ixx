// This is a personal academic project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com
module;

#include <glm/glm.hpp>
export module Joint;
import std;
namespace Engine {

	export class Joint {

	public:
		//************************************
		// Sets transformation matrices to Identity Matrix, parent Null
		//************************************
		Joint() {
			m_localBindPoseMatrix = glm::mat4(1.0f);
			m_localTransformationMatrix = glm::mat4(1.0f);
			m_targetTransformationMatrix = glm::mat4(1.0f);
			m_parent = nullptr;
		}
		//************************************
		// Construct Joint from name, bind-pose matrix and parent.
		//************************************
		Joint(const std::string& name, const glm::mat4& matrix, Joint* parent = nullptr) {
			this->m_name = name;
			this->m_localBindPoseMatrix = matrix;
			this->m_parent = parent;
			this->m_localTransformationMatrix = glm::mat4(1.0f);
			this->m_targetTransformationMatrix = glm::mat4(1.0f);
		}
		//************************************
		// set local transform to change joint orientation.
		//************************************
		void SetLocalTransform(glm::mat4 transformOperation) {
			this->m_localTransformationMatrix = transformOperation;
		}
		std::string m_name;
		glm::mat4 m_localBindPoseMatrix{};
		glm::mat4 m_localTransformationMatrix{};
		glm::mat4 m_targetTransformationMatrix{};
		Joint* m_parent;
		std::vector<Joint*> children_;
	};
}