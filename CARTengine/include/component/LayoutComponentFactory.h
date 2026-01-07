#pragma once
#include "LayoutComponent.h"
#include "Core.h"
#include "Types.h"
namespace cart {

	class LayoutComponentFactory {

	public:
		weak<IComponent> make(Layout_Component_Type type, std::string id);

		bool HasComponent(std::string id);
		bool HasComponent(Layout_Component_Type type);
		bool HasComponents();
		bool IsAllCompUpdated();
		bool UpdateUI();
		void Destroy();
		weak<IComponent> GetComponent(std::string id);
		weak<IComponent> GetComponent(Layout_Component_Type type);
	private:
		Dictionary<std::string, shared<IComponent>> m_components;
	};
}