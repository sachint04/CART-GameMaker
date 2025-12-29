#include "component/LayoutComponent.h"
#include "World.h"
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

    bool LayoutComponent::UpdateLayout(Vector2 size, float scaleX, float scaleY, const Rectangle& safeRect)
    {
		if (!m_isEnabled)return false; // do nothing;
		//Logger::Get()->Trace(std::format("LayoutComponent::UpdateLayout() ======== {} ========== ", GetId()));
		
		float left, right, top, bottom, anchorLeft, anchorRight, anchorTop, anchorBottom, w, h, cx, cy, parentx, parenty, parentw, parenth, anchorx, anchory, availW, availH, pixelW, pixelH;

		Vector2 canvas_size, ownerpivot, midAnchor;
		Rectangle parentRect, anchor;
		auto parent = m_owner.get()->parent();
	
		if (!parent.expired()) {
			// has owner
			if (parent.lock()->IsReady())
			{
			// owner is ready
				if (!parent.lock()->GetLayoutComponent().lock()->IsUpdated()) 
				{
					// owner not updated
					if (parent.lock()->GetLayoutComponent().lock()->UpdateLayout(size, scaleX, scaleY, safeRect))// Update Parent first
					{
						// update parent first
						parentx = parent.lock()->GetLocation().x - (parent.lock()->GetPivot().x * parent.lock()->GetSize().x);
						parenty = parent.lock()->GetLocation().y - (parent.lock()->GetPivot().y * parent.lock()->GetSize().y);
						parentw = parent.lock()->GetSize().x;
						parenth = parent.lock()->GetSize().y;
						parentRect = { (float)parentx, (float)parenty, (float)parentw, (float)parenth };
					}
					else {
						// owner failed to update
						Logger::Get()->Warn(std::format("LayoutComponent::UpdateLayout() Error!! owner {} is Read  but failed to update.\n", m_owner.get()->GetId()));
						return false;// failed to update parent, no point  goint further
					}

				}
				else {
				// owner is already updated!!!
					parentx = parent.lock()->GetLocation().x - (parent.lock()->GetPivot().x * parent.lock()->GetSize().x);
					parenty = parent.lock()->GetLocation().y - (parent.lock()->GetPivot().y * parent.lock()->GetSize().y);
					parentw = parent.lock()->GetSize().x;
					parenth = parent.lock()->GetSize().y;
					parentRect = { (float)parentx, (float)parenty, (float)parentw, (float)parenth };
				}
				
			}
			else {
				// owner not ready
				Logger::Get()->Warn(std::format("LayoutComponent::UpdateLayout() owner {} Not ready \n", m_owner.get()->GetId()));
				return false;
			}
		}
		else {
			
			canvas_size = World::UI_CANVAS.get()->Size();
			parentx = 0;
			parenty = 0;
			parentw = canvas_size.x;
			parenth = canvas_size.y;
			parentRect = { (float)parentx, (float)parenty, (float)parentw, (float)parenth};
		}
			
		anchor = m_owner.get()->GetAnchor();

		left = parentRect.x;
		right = parentRect.x + int(parentRect.width);
		top = parentRect.y ;
		bottom = parentRect.y + int(parentRect.height);
		availW = std::max(0.f, (float)right - left);
		availH = std::max(0.f, (float)bottom - top);

		// element pixel size based on design size and scale
		if ((m_owner.get()->HasTexture() && m_owner.get()->GetTextureType() == TEXTURE_PART) || m_owner.get()->IsScaleLocked()) {
			pixelW = std::max(1.f, m_Rect.width );
			pixelH = std::max(1.f, m_Rect.height);
		}
		else {

			pixelW = std::max(1.f, m_Rect.width * std::min(scaleX, 1.f));
			pixelH = std::max(1.f, m_Rect.height * std::min(scaleY, 1.f));
		}

		// place element at left top of anchor rect with clamping
		anchorLeft = parentRect.width * anchor.x;
		anchorRight = parentRect.width * anchor.width ;
		anchorTop = parentRect.height * anchor.y;
		anchorBottom = parentRect.height * anchor.height ;

		anchorx = parentRect.x + anchorLeft + (anchorRight - anchorLeft) / 2.f;
		anchory = parentRect.y + anchorTop + (anchorBottom - anchorTop) / 2.f;
		midAnchor = {	(float)anchorx, (float)anchory};

		// Size

		w = std::min(pixelW, availW);
		h = std::min(pixelH, availH);
		
		if (scaleX < scaleY) {
			int dh = m_owner.get()->GetDefaultHeight();
			int ph = parent.expired() ? World::UI_CANVAS.get()->GetDefaultCanvasSize().y : parent.lock().get()->GetDefaultHeight();
			float parentratioH = (float)dh / (float)ph;
			scaleY = parentratioH * World::UI_CANVAS.get()->StrechY();
			h = std::max((float)h, parentRect.height * scaleY);
		}
		else if(scaleX > scaleY){
			int dw = m_owner.get()->GetDefaultWidth();
			int pw = parent.expired() ? World::UI_CANVAS.get()->GetDefaultCanvasSize().x : parent.lock().get()->GetDefaultWidth();
			float parentratioW = (float)dw / (float)pw;
			scaleX = parentratioW * World::UI_CANVAS.get()->StrechX();
			w = std::max((float)w, parentRect.width * scaleX);
		}

		m_owner.get()->SetSize({(float)w, (float)h});
		Vector2 rawLoc = m_owner.get()->GetRawLocation();
		// Location
		if ((m_owner.get()->HasTexture() && m_owner.get()->GetTextureType() == TEXTURE_PART) || m_owner.get()->IsScaleLocked()) {
			cx = midAnchor.x + m_Rect.x;
			cy = midAnchor.y + m_Rect.y;
		}
		else {
			cx =  midAnchor.x + (float)(rawLoc.x * scaleX);
			cy =  midAnchor.y + (float)(rawLoc.y * scaleY);
		}
		m_owner.get()->SetLocation({(float)cx, (float)cy});
		
	

		// Safety check: is element outside safeRect?
		if (cx < safeRect.x || cy < safeRect.y ||
			cx + w > safeRect.x + w ||
			cy + h > safeRect.y + h) {
			// For demo we print; in real system we'd emit warning event
			std::cout << "[WARN] Element '" << m_Id << "' overlaps safe area.\n";
		}
		//Logger::Get()->Trace(std::format("\n{} Layout updated x {} | y {} \n width {} | height {}\n", m_Id, screenLoc.x, screenLoc.y, screenSize.x, screenSize.y));
	//	Logger::Get()->Trace(std::format("LayoutComponent::UpdateLayout() {}  Updated Successfully. ", GetId()));
	//	Logger::Get()->Trace(std::format("LayoutComponent::END ======== {} ========== \n", GetId()));

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