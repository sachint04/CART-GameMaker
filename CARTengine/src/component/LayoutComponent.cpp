#include "component/LayoutComponent.h"
#include "Object.h"
#include "Actor.h"
#include "UIElement.h"
#include "UICanvas.h"
#include "Logger.h"
#include "Types.h"
namespace cart
{
    LayoutComponent::LayoutComponent(const std::string& id, shared<UIElement> ui, Rectangle Anchor, Rectangle rect)
		:IComponent{id, LAYOUT_COMPONENT},
		m_owner{ui},
		m_anchorMinX{ Anchor.x },
		m_anchorMaxX{ Anchor.y },
		m_anchorMinY{ Anchor.width },
		m_anchorMaxY{ Anchor.height },
		m_isUpdated{false},
		m_Rect{rect}
    {
    }
    void LayoutComponent::Destroy()
    {
		m_owner.reset();
		m_isEnabled = false;
    }

    void LayoutComponent::Update()
    {
    }

    bool LayoutComponent::UpdateLayout(Vector2 size, float scale, const Rectangle& safeRect)
    {
		if (!m_isEnabled)return false; // do nothing;

		Logger::Get()->Trace(std::format("LayoutComponent::UpdateLayout() ======== {} ========== ", GetId()));
		
		auto parent = m_owner.get()->parent();
		Rectangle parentRect = {};
		if (!parent.expired()) {
			// has owner
			if (parent.lock()->IsReady())
			{
			// owner is ready
				if (!parent.lock()->GetLayoutComponent().lock()->IsUpdated()) 
				{
					// owner not updated
					if (parent.lock()->GetLayoutComponent().lock()->UpdateLayout(size, scale, safeRect))// Update Parent first
					{
						// update parent first
						parentRect = { parent.lock()->GetLocation().x - parent.lock()->GetPivot().x,
										parent.lock()->GetLocation().y - parent.lock()->GetPivot().y,
										parent.lock()->GetSize().x,
										parent.lock()->GetSize().y };
					}
					else {
						// owner failed to update
						Logger::Get()->Warn(std::format("LayoutComponent::UpdateLayout() Error!! owner {} is Read  but failed to update.\n", m_owner.get()->GetId()));
						return false;// failed to update parent, no point  goint further
					}

				}
				else {
				// owner is already updated!!!
					parentRect = { parent.lock()->GetLocation().x - parent.lock()->GetPivot().x,
										parent.lock()->GetLocation().y - parent.lock()->GetPivot().y,
										parent.lock()->GetSize().x,
										parent.lock()->GetSize().y };
				}
				
			}
			else {
				// owner not ready
				Logger::Get()->Warn(std::format("LayoutComponent::UpdateLayout() owner {} Not ready \n", m_owner.get()->GetId()));
				return false;
			}
		}
		else {
			Vector2 canvas_size = UICanvas::Get().lock()->Size();
			parentRect = { 0, 0, (float)canvas_size.x, (float)canvas_size.y };
		}
			

		Vector2 ownerpivot = m_owner.get()->GetPivot();
		int left = parentRect.x + int(parentRect.width * m_owner.get()->GetAnchor().x);
		int right = parentRect.x + int(parentRect.width * m_owner.get()->GetAnchor().width);
		int top = parentRect.y + int(parentRect.height * m_owner.get()->GetAnchor().y);
		int bottom = parentRect.y + int(parentRect.height * m_owner.get()->GetAnchor().height);
		int availW = std::max(0, right - left);
		int availH = std::max(0, bottom - top);

		// element pixel size based on design size and scale
		int pixelW = std::max(1, int(m_Rect.width * scale));
		int pixelH = std::max(1, int(m_Rect.height * scale));

		// place element at center of anchor rect with clamping
		/*int cx = left + availW / 2;
		int cy = top + availH / 2;*/

		// place element at left top of anchor rect with clamping
		int cx = left + (m_Rect.x * scale);
		int cy = top + (m_Rect.y * scale);

		Vector2 screenSize, screenLoc;
		screenSize.x = std::min(pixelW, availW);
		screenSize.y = std::min(pixelH, availH);
		//screenLoc.x = cx - screenSize.x / 2;
		//screenLoc.y = cy - screenSize.y / 2;

		screenLoc.x = cx;// -(screenSize.x / 2) + (m_ui.lock()->GetLocation().x * scale) - (m_ui.lock()->GetPivot().x * scale);
		screenLoc.y = cy;// -(screenSize.y / 2) + (m_ui.lock()->GetLocation().y * scale) - (m_ui.lock()->GetPivot().y * scale);

		m_owner.get()->SetSize(screenSize);
		m_owner.get()->SetLocation(screenLoc);
		m_owner.get()->SetPivot({ ownerpivot.x * scale, ownerpivot.y * scale });
		// Safety check: is element outside safeRect?
		if (screenLoc.x < safeRect.x || screenLoc.y < safeRect.y ||
			screenLoc.x + screenSize.x > safeRect.x + screenSize.x ||
			screenLoc.y + screenSize.y > safeRect.y + screenSize.y) {
			// For demo we print; in real system we'd emit warning event
			std::cout << "[WARN] Element '" << m_Id << "' overlaps safe area.\n";
		}
		//Logger::Get()->Trace(std::format("\n{} Layout updated x {} | y {} \n width {} | height {}\n", m_Id, screenLoc.x, screenLoc.y, screenSize.x, screenSize.y));
		Logger::Get()->Trace(std::format("LayoutComponent::UpdateLayout() {}  Updated Successfully. ", GetId()));
		Logger::Get()->Trace(std::format("LayoutComponent::END ======== {} ========== \n", GetId()));

		onLayoutChange.Broadcast();
		m_isUpdated = true;
		return m_isUpdated;
		
    }

	bool LayoutComponent::HasParent()
	{		
		return !m_owner.get()->parent().expired();
	}

	void LayoutComponent::SetForUpdate()
	{
		m_isUpdated = false;

	}

	bool LayoutComponent::IsOwnerReady()
	{
		if (m_owner) {
			return m_owner.get()->IsReady();
		}
		return false;
	}

	bool LayoutComponent::IsUpdated()
	{		
		return m_isUpdated;
	}

   

}