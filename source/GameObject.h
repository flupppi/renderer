#pragma once
#include"IGameObject.h"
#include<vector>
namespace Engine {

	class GameObject : public IGameObject
	{
	public:
		GameObject() = default;
		~GameObject() = default;
		void Update() override;
		void Start() override;
		void Awake() override;
		void RemoveComponent(IComponent* component) override;

	private:
		std::vector<std::unique_ptr<IComponent>> components;
	};

}