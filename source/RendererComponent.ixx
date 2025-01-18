module;
#pragma once

export module RendererComponent;
import IComponent;
import IRenderer;
import IMesh;
import IMaterial;
import std;
namespace Engine {

	export class RendererComponent : public IComponent
	{
		RendererComponent(std::unique_ptr<IRenderer> renderer)
			: m_renderer(std::move(renderer)) {
		}
		void Update() {};
		void Start() {};
		void Awake() {};

	private:
		std::unique_ptr<IRenderer> m_renderer;
		std::unique_ptr<IMaterial> m_material;
		std::unique_ptr<IMesh> m_mesh;

	};

}