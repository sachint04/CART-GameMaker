#pragma once
#include <string>
#include "Core.h"

namespace cart
{
	class Object;
	class IController {

	public:
		virtual void Notify(const std::string& event)const  = 0;
		virtual void RegisterUI(weak<Object> ui)const = 0;
		virtual bool IsMouseOver(weak<Object> ui) const = 0;

		
	};

}