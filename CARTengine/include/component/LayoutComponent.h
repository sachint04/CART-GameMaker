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
		LayoutComponent(const std::string& id, shared<UIElement> ui, Rectangle Anchor, Rectangle rect);
		// Inherited via IComponent
		void Destroy() override;
		void Update() override;
		bool UpdateLayout(Vector2 size, float scaleX, float scaleY,  const Rectangle& safeRect);
		bool HasParent();
		void SetForUpdate();
		bool IsUpdated();
		bool IsOwnerReady();

		Delegate<> onLayoutChange;
	private:
		shared<UIElement> m_owner;
		float m_anchorMinX;
		float m_anchorMaxX;
		float m_anchorMinY;
		float m_anchorMaxY;
		bool m_isUpdated;
		Rectangle m_Rect;
	};

}