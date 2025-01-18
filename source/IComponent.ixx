module;
#pragma once

export module IComponent;
namespace Engine {

	export class IComponent
	{
	public:

		virtual void Update() = 0;
		virtual void Start() = 0;
		virtual void Awake() = 0;
	};
}
