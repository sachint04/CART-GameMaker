#pragma once
#include "IComponent.h"
#include "Types.h"
#include <raylib.h>
#include "Delegate.h"

namespace cart {
	class UIElement;
	class LayoutComponent : public IComponent
	{
	public:
		LayoutComponent(std::string& id);
		~LayoutComponent();
		// Inherited via IComponent
		virtual void Init(shared<UIElement> ui, Rectangle Anchor, Rectangle rect) override;
		virtual void Destroy() override;
		virtual void Update() override;
		virtual void SetForUpdate() override;
		virtual bool IsOwnerReady() override;
		virtual bool UpdateLayout(Vector2 size, float scaleX, float scaleY,  const Rectangle& safeRect)override;
		bool IsUpdated() override;
		bool IsEnabled()override { return m_isEnabled; };
		weak<IComponent> GetWeakRef()override;
		Layout_Component_Type type()override { return Layout_Component_Type::LAYOUT; };
		std::string GetId()const override ;

		bool HasParent();

		Delegate<> onLayoutChange;
	protected:
		std::string m_Id;
		shared<UIElement> m_owner;
		float m_anchorMinX;
		float m_anchorMaxX;
		float m_anchorMinY;
		float m_anchorMaxY;
		bool m_isUpdated;
		Rectangle m_Rect;
		bool m_isEnabled;
		Layout_Component_Type m_type;
	};

}