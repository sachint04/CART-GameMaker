#include "component/LayoutComponent.h"
#include "World.h"
#include "Actor.h"
#include "UIElement.h"
#include "UICanvas.h"
#include "Logger.h"
#include "Types.h"
#include "component/LayoutComponent.h"

extern int DEFAULT_CANVAS_WIDTH;
extern int DEFAULT_CANVAS_HEIGHT;
extern int SCREEN_WIDTH;
extern int SCREEN_HEIGHT;
extern float CANVAS_STRECH_X;
extern float CANVAS_STRECH_Y;
namespace cart
{
#pragma region Constructor & Init


	LayoutComponent::LayoutComponent(std::string& id ) 
		:IComponent{  },
		m_anchorMinX{},
		m_anchorMaxX{},
		m_anchorMinY{},
		m_anchorMaxY{},
		m_isUpdated{false},
		m_Rect{},
		m_isEnabled{true},		
		m_type{NO_LAYOUT},
		m_Id{id},
		m_margin{},
		m_padding{},
		m_valign{}
	{
		m_type = LAYOUT;
	}

	LayoutComponent::~LayoutComponent()
	{
	}

	void LayoutComponent::Init(shared<UIElement> ui, Rectangle Anchor, Rectangle rect, UI_Layout_Properties props)
	{
		m_owner = ui;
		m_anchorMinX = Anchor.x;
		m_anchorMaxX = Anchor.y;
		m_anchorMinY = Anchor.width;
		m_anchorMaxY =  Anchor.height; 
		m_isUpdated = false;
		m_Rect = rect;
		m_align = props.align;
		m_valign = props.valign;
		m_margin = props.margin;
		m_padding = props.padding;
	}

#pragma endregion
	
#pragma region Helpers
	void LayoutComponent::Update()
    {	
		UpdateLayout();
    }

    bool LayoutComponent::UpdateLayout()
    {
		if (!m_isEnabled)return false; // do nothing;


		auto resize = [](float& s, float& p, // out size [width/height], out location [x/y]
			float a1, float a2, float& pr1, float pr2,  // anchor [x/y], anchor [width/height], parent location[x/y],  parent size [width/height] 
			float p1, float p2,  // Raw position [left/top], size [width/heigth],
			float ods, float pds, // default size [width/height], parent default size [width/height]
			float mis, float str, bool csr, // minimum scale, max streach [x/y], can streach
			float pv,  bool csc // Pivot,  can Scale
			) {
			float min = a1 * pr2; // min [left / top]
			float max = a2 * pr2; // max [right / bottom ]
			if (min != max)// anchor is a rectangle area
			{
				float fscale = csr ? (pr2 / pds) * str : mis;// MAXIMUM OF -> minimum scale and  max streach 
				p = pr1 + min + (csr)? p1* fscale : p1 * mis; // parent start + anchor min + cur left/top [x/y] 
				float re = (csr) ? ods * fscale : ods * mis;
				s = p2 * fscale;// -(min + p + re); // parent start + anchor max - right/botton [width/height]
			}
			else {	// anchor is a point
				float fscale = csr? (pr2 / pds) * str : mis;// MAXIMUM OF -> minimum scale and  max streach 
				s = (csr) ?ods * fscale : mis * ods ; // updated size 
				p =  ((csr) ? p1 * fscale : p1 * mis) ; // parent start + anchor min + (current pos [x/y] * scale)
			}
		};


		//Logger::Get()->Trace(std::format("LayoutComponent::UpdateLayout() ======== {} ========== ", GetId()));
		
		float left, right, top, bottom, anchorLeft, anchorRight, anchorTop, anchorBottom, w, h, cx, cy, anchorx, anchory, availW, availH, pixelW, pixelH, defaultW, defaultH, parentDefaultW, parentDefaultH ;

		Vector2 canvas_size, ownerpivot, midAnchor, rawPosition; 
		Rectangle parentRect, anchor;
		auto parent = m_owner.get()->parent();
		ownerpivot = m_owner.get()->GetPivot();
		rawPosition = m_owner.get()->GetRawLocation();
		defaultW = m_owner.get()->GetDefaultWidth();
		defaultH = m_owner.get()->GetDefaultHeight();
		float scaleX = World::UI_CANVAS.get()->ScaleX();
		float scaleY = World::UI_CANVAS.get()->ScaleY();
		if (!parent.expired()) {
			// owner is ready
			if (!parent.lock()->IsLayoutUpdated()) 
			{
				// owner not updated
				if (parent.lock()->UpdateLayout())// Update Parent first
				{
					// update parent first
					/*parentx = parent.lock()->GetLocation().x - (parent.lock()->GetPivot().x * parent.lock()->GetSize().x);
					parenty = parent.lock()->GetLocation().y - (parent.lock()->GetPivot().y * parent.lock()->GetSize().y);
					parentw = parent.lock()->GetSize().x;
					parenth = parent.lock()->GetSize().y;*/
					parentRect = parent.lock().get()->GetBounds(); // { (float)parentx, (float)parenty, (float)parentw, (float)parenth };
					parentDefaultW = parent.lock().get()->GetDefaultWidth();
					parentDefaultH = parent.lock().get()->GetDefaultHeight();
				}
				else {
					// owner failed to update
					Logger::Get()->Warn(std::format("LayoutComponent::UpdateLayout() Error!! owner {} is Read  but failed to update.\n", m_owner.get()->GetId()));
					return false;// failed to update parent, no point  goint further
				}

			}
			else {
			// owner is already updated!!!
				/*parentx = parent.lock()->GetLocation().x - (parent.lock()->GetPivot().x * parent.lock()->GetSize().x);
				parenty = parent.lock()->GetLocation().y - (parent.lock()->GetPivot().y * parent.lock()->GetSize().y);
				parentw = parent.lock()->GetSize().x;
				parenth = parent.lock()->GetSize().y;*/
				parentRect = parent.lock().get()->GetBounds();
				parentDefaultW = parent.lock().get()->GetDefaultWidth();
				parentDefaultH = parent.lock().get()->GetDefaultHeight();
			}
				
		}
		else {
			
			canvas_size = World::UI_CANVAS.get()->Size();
			parentRect = { 0, 0, canvas_size.x, canvas_size.y};
			parentDefaultW = World::UI_CANVAS.get()->GetDefaultWidth();
			parentDefaultH = World::UI_CANVAS.get()->GetDefaultHeight();

		}
			
		anchor = m_owner.get()->GetAnchor();

		left = parentRect.x;
		right = parentRect.x + int(parentRect.width);
		top = parentRect.y ;
		bottom = parentRect.y + int(parentRect.height);
		availW = std::max(0.f, (float)right - left);
		availH = std::max(0.f, (float)bottom - top);


		bool isScaleLocked = (m_owner.get()->HasTexture() && m_owner.get()->GetTextureType() == TEXTURE_PART) || m_owner.get()->IsScaleLocked();
		

		//m_Rect = m_owner.get()->GetBounds();
		Vector2 raws = m_owner.get()->GetRawSize();
		resize(w, cx, anchor.x, anchor.width, parentRect.x, parentRect.width, rawPosition.x, m_owner.get()->GetSize().x, raws.x, parentDefaultW, std::min(scaleX, scaleY), World::UI_CANVAS.get()->StrechX(), scaleX > scaleY, m_owner.get()->GetPivot().x, isScaleLocked);

		resize(h, cy, anchor.y, anchor.height, parentRect.y, parentRect.height, rawPosition.y, m_owner.get()->GetSize().y, raws.y, parentDefaultH, std::min(scaleX, scaleY), World::UI_CANVAS.get()->StrechY(), scaleX < scaleY, m_owner.get()->GetPivot().y, isScaleLocked);
	
		m_owner.get()->SetSize({ w, h });
		m_owner.get()->SetLocation({ cx, cy });
		
		m_owner.get()->OnLayoutChange();
		m_isUpdated = true;
		return m_isUpdated;
		
    }

	bool LayoutComponent::HasParent()
	{		
		return !m_owner.get()->parent().expired();
	}

	

	weak<IComponent> LayoutComponent::GetWeakRef()
	{
		return shared_from_this();
	}

	std::string LayoutComponent::GetId() const
	{
		return m_Id;
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
	
#pragma endregion

#pragma region Cleanup
	void LayoutComponent::Destroy()
	{
		m_owner.reset();
		m_isEnabled = false;
	}
#pragma endregion
}