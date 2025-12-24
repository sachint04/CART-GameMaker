#pragma once
#include <vector>
#include "Core.h"

namespace cart
{
	class Object;
	class UIElement;
	
	class InputController 
	{
	public: 
		
	
		void Notify(const std::string& event);
		void RegisterUI(weak<Object> ui);
		void RemoveUI(const std::string& id);
		void SetFocus(const std::string& id);
		std::string GetFocusedId() { return m_curFocusedId; };
		bool HasFocus() { return m_curFocusedId.size() > 0; };
		void Clear();
		bool IsMouseOver(weak<Object> ui);
		~InputController();
		std::vector<weak<UIElement>> m_uilist = {};
		std::string m_id;
		std::string m_curFocusedId;
	};
}