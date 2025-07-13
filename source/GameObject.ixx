
module;
export module GameObject;
import std;
import IGameObject;
import IComponent;
namespace Engine {

	export class GameObject : public IGameObject
	{
	public:
		GameObject() = default;
		~GameObject() = default;
		void Update() {
			for (auto& c : components)
			{
				c->Update();
			}
		};
		void Start() {
			for (auto& c : components)
			{
				c->Start();
			}
		}
		void Awake() {
			for (auto& c : components)
			{
				c->Awake();
			}
		}
		void RemoveComponent(IComponent* component) override;

	private:
		std::vector<std::unique_ptr<IComponent>> components;
	};

}