#pragma once
#include <string>
#include "Types.h"
#include "Delegate.h"
namespace cart {
	class UIElement;
	class IComponent:public std::enable_shared_from_this<IComponent>
	{
	public:
		IComponent();
		virtual ~IComponent() = default;

		virtual void Init(shared<UIElement> ui, Rectangle Anchor, Rectangle rect, UI_Layout_Properties props)  = 0;
		virtual void Destroy() = 0;
		virtual void Update() = 0;
		virtual void SetForUpdate() = 0;
		virtual bool IsOwnerReady() = 0;
		virtual bool IsUpdated() = 0;
		virtual bool UpdateLayout() = 0;
		virtual bool IsEnabled() = 0;
		virtual weak<IComponent> GetWeakRef() = 0;
		virtual Layout_Component_Type type() = 0;
		virtual std::string GetId()const = 0;

	private:
		
	};

}