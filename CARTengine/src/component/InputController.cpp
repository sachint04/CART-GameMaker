#include <memory>
#include <algorithm> 
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

	void InputController::RemoveUI(const std::string& id) {
		auto find = std::find_if(m_uilist.begin(), m_uilist.end(), [&](const weak<UIElement>& p) {
			return !p.expired() && p.lock()->GetId().compare(id) == 0;
		});

		if (find != m_uilist.end()) {
				find->reset();
				m_uilist.erase(find);				
		}
	}

	void InputController::Clear() {
		m_uilist.clear();
	}

	bool InputController::IsMouseOver(weak<Object> ui)
	{
		weak<UIElement> shr_ui = std::dynamic_pointer_cast<UIElement>(ui.lock());
		for (int i = m_uilist.size() - 1; i >= 0; --i)
		{
			
			if (m_uilist.at(i).expired() || !m_uilist.at(i).lock()->IsVisible())continue;
			
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
