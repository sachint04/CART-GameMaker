#include "component/LayoutComponent.h"
#include "Object.h"
#include "Actor.h"
#include "UIElement.h"
#include "UICanvas.h"
#include "Logger.h"
namespace cart
{
    LayoutComponent::LayoutComponent(const std::string& id, weak<UIElement> ui, Rectangle Anchor )
		:IComponent{id},
		m_ui{ui},
		m_anchorMinX{ Anchor.x },
		m_anchorMaxX{ Anchor.y },
		m_anchorMinY{ Anchor.width },
		m_anchorMaxY{ Anchor.height }
    {
    }
    void LayoutComponent::Destroy()
    {
    }

    void LayoutComponent::Update()
    {
    }

    void LayoutComponent::UpdateLayout(Vector2 size, float scale, const Rectangle& safeRect)
    {
		// compute anchors in pixels
		
		auto parent = m_ui.lock()->parent();
		Rectangle parentRect;
		if (!parent.expired()) {

			if (!parent.lock()->GetLayoutComponent().lock()->IsUpdated())
			{
				parent.lock()->GetLayoutComponent().lock()->UpdateLayout(size, scale, safeRect);// update parent first
			}

			parentRect = {	parent.lock()->GetLocation().x - parent.lock()->GetPivot().x,
							parent.lock()->GetLocation().y - parent.lock()->GetPivot().y,
							parent.lock()->GetSize().x, 
							parent.lock()->GetSize().y };
		}
		else {
			Vector2 canvas_size = UICanvas::Get().lock()->Size();
			parentRect = { 0, 0, (float)canvas_size.x, (float)canvas_size.y };
		}
			
		Vector2 ownerloc = m_ui.lock()->GetLocation();
		Rectangle ownerRect = {	m_ui.lock()->GetLocation().x,
									m_ui.lock()->GetLocation().y,
									m_ui.lock()->GetSize().x,
									m_ui.lock()->GetSize().y };
		int left = parentRect.x + int(parentRect.width * m_ui.lock()->GetAnchor().x);
		int right = parentRect.x + int(parentRect.width * m_ui.lock()->GetAnchor().width);
		int top = parentRect.y + int(parentRect.height * m_ui.lock()->GetAnchor().y);
		int bottom = parentRect.y + int(parentRect.height * m_ui.lock()->GetAnchor().height);
		int availW = std::max(0, right - left);
		int availH = std::max(0, bottom - top);

		// element pixel size based on design size and scale
		int pixelW = std::max(1, int(ownerRect.width * scale));
		int pixelH = std::max(1, int(ownerRect.height * scale));

		// place element at center of anchor rect with clamping
		/*int cx = left + availW / 2;
		int cy = top + availH / 2;*/

		// place element at left top of anchor rect with clamping
		int cx = left + (m_ui.lock()->GetLocation().x * scale) - (m_ui.lock()->GetPivot().x * scale);
		int cy = top + (m_ui.lock()->GetLocation().y * scale) - (m_ui.lock()->GetPivot().y * scale);

		Vector2 screenSize, screenLoc;
		screenSize.x = std::min(pixelW, availW);
		screenSize.y = std::min(pixelH, availH);
		//screenLoc.x = cx - screenSize.x / 2;
		//screenLoc.y = cy - screenSize.y / 2;

		screenLoc.x = cx;// -(screenSize.x / 2) + (m_ui.lock()->GetLocation().x * scale) - (m_ui.lock()->GetPivot().x * scale);
		screenLoc.y = cy;// -(screenSize.y / 2) + (m_ui.lock()->GetLocation().y * scale) - (m_ui.lock()->GetPivot().y * scale);

		m_ui.lock()->SetSize(screenSize);
		m_ui.lock()->SetLocation(screenLoc);

		// Safety check: is element outside safeRect?
		if (screenLoc.x < safeRect.x || screenLoc.y < safeRect.y ||
			screenLoc.x + screenSize.x > safeRect.x + screenSize.x ||
			screenLoc.y + screenSize.y > safeRect.y + screenSize.y) {
			// For demo we print; in real system we'd emit warning event
			std::cout << "[WARN] Element '" << m_Id << "' overlaps safe area.\n";
		}
		Logger::Get()->Trace(std::format("\n{} Layout updated x {} | y {} \n width {} | height {}\n", m_Id, screenLoc.x, screenLoc.y, screenSize.x, screenSize.y));
		m_isUpdated = true;
    }

	bool LayoutComponent::HasParent()
	{		
		return !m_ui.lock()->parent().expired();
	}

	void LayoutComponent::SetForUpdate()
	{
		m_isUpdated = false;

	}

   

}