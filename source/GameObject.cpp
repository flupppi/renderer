#include "GameObject.h"

void GameObject::Update()
{
	for (int i = 0; i < components.size(); i++)
	{
		components[i].Update();
	}
}

void GameObject::Start()
{
	for (int i = 0; i < components.size(); i++)
	{
		components[i].Start();
	}
}

void GameObject::Awake()
{
	for (int i = 0; i < components.size(); i++)
	{
		components[i].Awake();
	}
}

void GameObject::RemoveComponent(IComponent* component)
{

	
}

