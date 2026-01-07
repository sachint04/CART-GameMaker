#include "component/HLayoutComponent.h"
#include "Application.h"
#include "World.h"
#include "UIElement.h"
namespace cart {

	HLayoutComponent::HLayoutComponent(std::string& id)
		: LayoutComponent{ id}
	{
	}
	bool HLayoutComponent::UpdateLayout(Vector2 size, float scaleX, float scaleY, const Rectangle& safeRect)
	{
		Rectangle rect = m_owner.get()->GetBounds();
		float xPos = 0;
		float maxHeight = 0;
		float yOffset = 0;
		std::vector<weak<UIElement>> ch = m_owner.get()->Children();
		for (size_t i = 0; i < ch.size(); i++)
		{
			if (!ch.at(i).lock().get()->IsVisible())continue;
			Rectangle elemRect = ch.at(i).lock().get()->GetBounds();

			if (xPos + elemRect.width > rect.x + rect.width) {
				yOffset += maxHeight;
				xPos = rect.x;
				maxHeight = 0;
			}
			Vector2 pos = ch.at(i).lock().get()->GetRawLocation();
			Rectangle anchor = ch.at(i).lock().get()->GetAnchor();
			
			ch.at(i).lock().get()->SetAnchor({ 0, anchor.y, 0, anchor.height});
			ch.at(i).lock().get()->SetPivot({ 0,  ch.at(i).lock().get()->GetPivot().y });
			ch.at(i).lock().get()->SetDefaultLocation({ xPos, pos.y + yOffset });
			xPos += elemRect.width;
			maxHeight = (elemRect.height > maxHeight) ? elemRect.height : maxHeight;
		}
		bool updated = LayoutComponent::UpdateLayout(size, scaleX, scaleY, safeRect);
		return updated;
	
	}

	
}
