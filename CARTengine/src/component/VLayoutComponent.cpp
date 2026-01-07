#include "component/VLayoutComponent.h"
#include "Application.h"
#include "World.h"
#include "UIElement.h"
namespace cart {

	VLayoutComponent::VLayoutComponent(std::string& id)
		: LayoutComponent{id}
	{
	}
	bool VLayoutComponent::UpdateLayout(Vector2 size, float scaleX, float scaleY, const Rectangle& safeRect)
	{
		//bool updated = LayoutComponent::UpdateLayout(size, scaleX, scaleY, safeRect);
		//return updated;
		//Rectangle rect = m_owner.get()->GetBounds();
		//float yPos = 0;
		//float maxWidth = 0;
		//float xOffset = 0;
		//std::vector<weak<UIElement>> ch = m_owner.get()->Children();
		//for (size_t i = 0; i < ch.size(); i++)
		//{
		//	if (!ch.at(i).lock().get()->IsVisible())continue;
		//	Rectangle elemRect = ch.at(i).lock().get()->GetBounds();
		//	
		//	if (yPos + elemRect.height > rect.y + rect.height) {
		//		xOffset += maxWidth;
		//		yPos = rect.y;
		//		maxWidth = 0;
		//	}
		//	Vector2 pos = ch.at(i).lock().get()->GetRawLocation();
		//	Rectangle anchor = ch.at(i).lock().get()->GetAnchor();
		//	ch.at(i).lock().get()->SetAnchor({ anchor.x, 0, anchor.width, 0 });
		//	ch.at(i).lock().get()->SetPivot({ ch.at(i).lock().get()->GetPivot().x, 0 });
		//	//ch.at(i).lock().get()->SetDefaultLocation({ pos.x, yPos });
		//	ch.at(i).lock().get()->SetLocation({ pos.x, yPos });
		//	//ch.at(i).lock().get()->UpdateLayout();
		//	yPos += elemRect.height;

		//	maxWidth = (elemRect.width > maxWidth) ? elemRect.width : maxWidth;
		//}
		bool updated = LayoutComponent::UpdateLayout(size, scaleX, scaleY, safeRect);
		return updated;
	}
	
}
