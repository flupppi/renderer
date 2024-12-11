#pragma once
#include"IGameObject.h"
#include<vector>
class GameObject : public IGameObject
{
public:
	
	void Update() override;
	void Start() override;
	void Awake() override;
	void RemoveComponent(IComponent* component) override;
	
	std::vector<IComponent> components;

};

