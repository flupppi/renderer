#pragma once
#include "IComponent.h"
#include "IRenderer.h"
#include "IMesh.h"
#include "IMaterial.h"
namespace Engine {

	class RendererComponent : public IComponent
	{
		RendererComponent(std::unique_ptr<IRenderer> renderer)
			: m_renderer(std::move(renderer)) {
		}
		void Update() override;
		void Start() override;
		void Awake() override;

	private:
		std::unique_ptr<IRenderer> m_renderer;
		std::unique_ptr<IMaterial> m_material;
		std::unique_ptr<IMesh> m_mesh;

	};

}