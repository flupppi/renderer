#include "GameObject.h"
namespace Engine {
	void GameObject::Update()
	{
		for (auto& c : components)
		{
			c->Update();
		}
	}

	void GameObject::Start()
	{
		for (auto& c : components)
		{
			c->Start();
		}
	}

	void GameObject::Awake()
	{
		for (auto& c : components)
		{
			c->Awake();
		}
	}

	void GameObject::RemoveComponent(IComponent* component)
	{


	}

}