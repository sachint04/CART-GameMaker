#pragma once
#include "IComponent.h"
#include "Types.h"
#include <raylib.h>
namespace cart {
	class UIElement;
	class LayoutComponent : public IComponent
	{
	public:
		LayoutComponent(const std::string& id, weak<UIElement> ui, Rectangle Anchor);
		// Inherited via IComponent
		void Destroy() override;
		void Update() override;
		void UpdateLayout(Vector2 size, float scale, const Rectangle& safeRect);
		bool HasParent();
		void SetForUpdate();
		bool IsUpdated() { return m_isUpdated; };
	private:
		weak<UIElement> m_ui;
		float m_anchorMinX;
		float m_anchorMaxX;
		float m_anchorMinY;
		float m_anchorMaxY;
		bool m_isUpdated;
	};

}