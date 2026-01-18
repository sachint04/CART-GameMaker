#include "component/IComponent.h"
#include "component/LayoutComponentFactory.h"
#include "component/LayoutComponent.h"
#include "component/VLayoutComponent.h"
#include "component/HLayoutComponent.h"
#include "World.h"
#include "UICanvas.h"
namespace cart {
	
	weak<IComponent> LayoutComponentFactory::make(Layout_Component_Type type, std::string id)
	{
		
		switch (type)
		{
			case LAYOUT :
			{
				shared<LayoutComponent> component{ new LayoutComponent(id) };
				m_components.insert({ id, component });
				World::UI_CANVAS.get()->RegisterComponent(component.get()->GetWeakRef());
				return component;
				break;
			}
			case V_LAYOUT :
			{

				shared<VLayoutComponent> component{ new VLayoutComponent(id) };
				m_components.insert({ id, component });
				World::UI_CANVAS.get()->RegisterComponent(component.get()->GetWeakRef());
				return component;
				break;
			}
			case H_LAYOUT :
			{
				shared<HLayoutComponent> component{ new HLayoutComponent(id) };
				m_components.insert({ id, component });
				World::UI_CANVAS.get()->RegisterComponent(component.get()->GetWeakRef());
				return component;
				break;
			}
			case NO_LAYOUT :
				break;
		}
		return weak<IComponent>();
	}

	bool LayoutComponentFactory::HasComponent(std::string id)
	{
		auto find = m_components.find(id);
		if (find != m_components.end()) {
			return true;
		}
		return false;
	}

	bool LayoutComponentFactory::HasComponent(Layout_Component_Type type)
	{
		for (auto iter = m_components.begin(); iter != m_components.end(); ++iter)
		{
			if (iter->second.get()->type() == type) {

				return true;
			}
		}
		return false;
	}

	bool LayoutComponentFactory::HasComponents()
	{		
		int count = 0;
		for (auto iter = m_components.begin(); iter != m_components.end(); ++iter)
		{
			count++;
		}
		return count > 0;
	}

	bool LayoutComponentFactory::IsAllCompUpdated()
	{
		for (auto iter = m_components.begin(); iter != m_components.end(); ++iter)
		{
			if (!iter->second.get()->IsUpdated())return false;
		}
		return true;
	}

	bool LayoutComponentFactory::UpdateUI()
	{
		for (auto iter = m_components.begin(); iter != m_components.end(); ++iter)
		{
			iter->second.get()->Update();
		}
		return true;
	}

	weak<IComponent> LayoutComponentFactory::GetComponent(std::string id)
	{
		auto find = m_components.find(id);
		if (find != m_components.end()) {
			return find->second.get()->GetWeakRef();
		}
		return shared<IComponent> {nullptr};;
	}

	weak<IComponent> LayoutComponentFactory::GetComponent(Layout_Component_Type type)
	{
		for (auto iter = m_components.begin(); iter != m_components.end(); ++iter)
		{
			if (iter->second.get()->type() == type) {

				return iter->second.get()->GetWeakRef();
			}
		}
		return  shared<IComponent> {nullptr};
	}
	
	void LayoutComponentFactory::Destroy()
	{
		for (auto iter = m_components.begin(); iter != m_components.end();)
		{
			World::UI_CANVAS.get()->RemoveComponent(iter->first);
			iter->second.get()->Destroy();
			iter->second.reset();
			iter = m_components.erase(iter);
		}
	}
}