#include <memory>
#include "component/InputController.h"
#include "Object.h"
#include "UIElement.h"
namespace cart {

	void InputController::Notify( const std::string& event)
	{
		for (size_t i = 0; i < m_uilist.size(); i++)
		{
			//m_uilist->Notify(event);
		}

	}

	void InputController::RegisterUI(weak<Object> ui)
	{
		
		weak<UIElement> shr_ui = std::dynamic_pointer_cast<UIElement>(ui.lock());
		m_uilist.push_back(shr_ui);
	}

	bool InputController::IsMouseOver(weak<Object> ui)
	{
		weak<UIElement> shr_ui = std::dynamic_pointer_cast<UIElement>(ui.lock());
		for (int i = m_uilist.size() - 1; i >= 0; --i)
		{
			
			if (m_uilist.at(i).expired())continue;
			
			if (CheckCollisionPointRec(GetMousePosition(), m_uilist.at(i).lock()->GetBounds())) {
				
					return ui.lock() == m_uilist.at(i).lock();
			}
		}
	
		return false;
	}

	InputController::~InputController()
	{
	}



}
