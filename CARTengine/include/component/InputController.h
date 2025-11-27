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
		void Clear();
		bool IsMouseOver(weak<Object> ui);
		~InputController();
		std::vector<weak<UIElement>> m_uilist = {};
		std::string m_id;

	};
}