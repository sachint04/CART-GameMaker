#pragma once
#include "component/LayoutComponent.h"
namespace cart {

	class HLayoutComponent : public LayoutComponent
	{
	public:
		HLayoutComponent(std::string& id);
		virtual bool UpdateLayout();
	protected:

	};
}