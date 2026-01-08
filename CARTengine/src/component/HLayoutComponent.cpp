#include "component/HLayoutComponent.h"
#include "Application.h"
#include "World.h"
#include "UIElement.h"
#include "Core.h"
namespace cart {

	HLayoutComponent::HLayoutComponent(std::string& id)
		: LayoutComponent{ id}
	{
	}
	bool HLayoutComponent::UpdateLayout(Vector2 size, float scaleX, float scaleY, const Rectangle& safeRect)
	{
		

		float startX, startY, curContentWidth, contentWidth = 0, totalpadding, totalavailableWidth = 0;
		bool updated = LayoutComponent::UpdateLayout(size, scaleX, scaleY, safeRect);
		Rectangle rect = m_owner.get()->GetBounds();
		std::vector<weak<UIElement>> ch = m_owner.get()->Children();
		std::vector<std::vector<int>> rows;
		rows.push_back( std::vector<int>{});
		int count = 1;
		for (size_t i = 0; i < ch.size(); i++)
		{
			Rectangle elemRect = ch.at(i).lock().get()->GetBounds();
			contentWidth += elemRect.width;
			float padding = m_padding * (count - 1);
			curContentWidth = m_margin + contentWidth + padding;
			
			if (curContentWidth > rect.width)
			{			
				count = 0;
				contentWidth = elemRect.width;
				rows.push_back(std::vector<int>{});
			}
			else {
				if (curContentWidth > totalavailableWidth)totalavailableWidth = curContentWidth;
			}
			std::vector<int>& row = rows.at(rows.size() - 1);			
			row.push_back((int)i);
			count++;
		}

		switch (m_align) {
		case LEFT :
			startX = rect.x + m_margin;
			break;
		case CENTER:
			startX = rect.x + (rect.width - totalavailableWidth) * 0.5f;
			break;
		case RIGHT:
			startX = rect.x + rect.width - totalavailableWidth;
			break;
		case JUSTIFIED:
			startX = 0;
			break;
		}

		auto findmaxheight = [&](float& max, int row)
		{
			max = 0;
			for (size_t i = 0; i < rows.at(row).size(); i++)
			{
				Rectangle rect = ch.at(rows.at(row).at(i)).lock().get()->GetBounds();
				if (rect.height > max)max = rect.height;
			}
		};

		float maxHeight = 0;
		findmaxheight(maxHeight, 0);

		switch (m_valign) {
		case TOP:
			startY = rect.y;
			break;
		case MIDDLE:
			startY = rect.y + (rect.height - (maxHeight + m_padding * 2)) * 0.5f;
			break;
		case BOTTOM:
			startY = rect.y + rect.height - (maxHeight + m_padding * 2);
			break;
		}
		count = 0;
		for (auto iter = rows.begin(); iter != rows.end(); ++iter)
		{
			float x = startX, y = startY, m;
			findmaxheight(m, count);
			for (size_t i = 0; i < iter->size(); i++)
			{
				ch.at(iter->at(i)).lock().get()->SetLocation({ x, y });
				x += m_padding + ch.at(iter->at(i)).lock().get()->GetBounds().width;
			}
			startY += m + m_padding;
			count++;
		}

		//float xPos = rect.x + m_margin;
		//if (m_align == MIDDLE) {
		//	xPos = (rect.x - contentWidth) * 0.5f;
		//}
		//else if (m_align == RIGHT) {
		//	xPos = rect.x + rect.width;
		//}

		//float yOffset = rect.y + m_valign;
		//for (size_t i = 0; i < ch.size(); i++)
		//{
		//	if (!ch.at(i).lock().get()->IsVisible())continue;
		//	Rectangle elemRect = ch.at(i).lock().get()->GetBounds();

		//	if (xPos + elemRect.width > rect.x + rect.width) {
		//		yOffset += maxHeight;
		//		xPos = rect.x;
		//		maxHeight = 0;
		//	}
		//	Vector2 pos = ch.at(i).lock().get()->GetRawLocation();
		//	Rectangle anchor = ch.at(i).lock().get()->GetAnchor();
		//	
		//	ch.at(i).lock().get()->SetLocation({ xPos, yOffset });
		//	xPos += elemRect.width + m_padding;
		//	maxHeight = (elemRect.height > maxHeight) ? elemRect.height : maxHeight;
		//}
		
		return updated;
	
	}

	
}
