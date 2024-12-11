#pragma once
#include<vector>
#include<memory>
#include "IComponent.h"
class IGameObject
{
public:
	
	virtual void Update() = 0;
	virtual void Start() = 0;
	virtual void Awake() = 0;
    template<typename T, typename... Args>
    T* AddComponent(Args&&... args) {
        static_assert(std::is_base_of<IComponent, T>::value, "T must inherit from IComponent");
        auto component = std::make_unique<T>(std::forward<Args>(args)...);
        T* componentPtr = component.get();  // Get raw pointer before transferring ownership
        components.push_back(std::move(component));
        return componentPtr;
    }	
    virtual void RemoveComponent(IComponent* component) = 0;
protected:
    std::vector<std::unique_ptr<IComponent>> components;
};

