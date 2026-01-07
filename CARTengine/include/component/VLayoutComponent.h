#pragma once
#include "component/LayoutComponent.h"
#include "Types.h"
namespace cart {

	class VLayoutComponent : public LayoutComponent
	{
	public:
		VLayoutComponent(std::string& id);		
		virtual bool UpdateLayout(Vector2 size, float scaleX, float scaleY, const Rectangle& safeRect);
	protected:


	};
}