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
		m_Id{id}
	{
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
		float scaleX = (float)SCREEN_WIDTH / (float)DEFAULT_CANVAS_WIDTH;
		float scaleY = (float)SCREEN_HEIGHT / (float)DEFAULT_CANVAS_HEIGHT;
		UpdateLayout(World::UI_CANVAS.get()->Size(), scaleX, scaleY, World::UI_CANVAS.get()->SafeRect());
    }

    bool LayoutComponent::UpdateLayout(Vector2 size, float scaleX, float scaleY, const Rectangle& safeRect)
    {
		if (!m_isEnabled)return false; // do nothing;


		auto resize = [](float& s, float& p, // out size [width/height], out location [x/y]
			float a1, float a2, float& pr1, float pr2,  // anchor [x/y], anchor [width/height], parent location[x/y],  parent size [width/height] 
			float p1, float p2,  // Raw position [left/top], size [width/heigth],
			float ods, float pds, // default size [width/height], parent default size [width/height]
			float mis, float str, bool csr, // minimum scale, max streach [x/y], can streach
			bool csc // can Scale
			) {
			float min = a1 * pr2; // min [left / top]
			float max = a2 * pr2; // max [right / bottom ]
			if (min != max)// anchor is a rectangle area
			{
				p = pr1 + min + p1; // parent start + anchor min + cur left/top [x/y] 

				s = pr1 + max - p2; // parent start + anchor max - right/botton [width/height]
			}
			else {	// anchor is a point
				float fscale = csr? (ods / pds) * str : mis;// MAXIMUM OF -> minimum scale and  max streach 
				s = (csr) ?pr2 * fscale : mis * ods ; // updated size 
				p = pr1 + min + p1 * (csr? (ods/s) : mis); // parent start + anchor min + (current pos [x/y] * scale)
			}
		};


		//Logger::Get()->Trace(std::format("LayoutComponent::UpdateLayout() ======== {} ========== ", GetId()));
		
		float left, right, top, bottom, anchorLeft, anchorRight, anchorTop, anchorBottom, w, h, cx, cy, parentx, parenty, parentw, parenth, anchorx, anchory, availW, availH, pixelW, pixelH, defaultW, defaultH, parentDefaultW, parentDefaultH ;

		Vector2 canvas_size, ownerpivot, midAnchor, rawPosition; 
		Rectangle parentRect, anchor;
		auto parent = m_owner.get()->parent();
		ownerpivot = m_owner.get()->GetPivot();
		rawPosition = m_owner.get()->GetRawLocation();
		defaultW = m_owner.get()->GetDefaultWidth();
		defaultH = m_owner.get()->GetDefaultHeight();
		if (!parent.expired()) {
			// owner is ready
			if (!parent.lock()->IsLayoutUpdated()) 
			{
				// owner not updated
				if (parent.lock()->UpdateLayout())// Update Parent first
				{
					// update parent first
					parentx = parent.lock()->GetLocation().x - (parent.lock()->GetPivot().x * parent.lock()->GetSize().x);
					parenty = parent.lock()->GetLocation().y - (parent.lock()->GetPivot().y * parent.lock()->GetSize().y);
					parentw = parent.lock()->GetSize().x;
					parenth = parent.lock()->GetSize().y;
					parentRect = { (float)parentx, (float)parenty, (float)parentw, (float)parenth };
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
				parentx = parent.lock()->GetLocation().x - (parent.lock()->GetPivot().x * parent.lock()->GetSize().x);
				parenty = parent.lock()->GetLocation().y - (parent.lock()->GetPivot().y * parent.lock()->GetSize().y);
				parentw = parent.lock()->GetSize().x;
				parenth = parent.lock()->GetSize().y;
				parentRect = { (float)parentx, (float)parenty, (float)parentw, (float)parenth };
				parentDefaultW = parent.lock().get()->GetDefaultWidth();
				parentDefaultH = parent.lock().get()->GetDefaultHeight();
			}
				
		}
		else {
			
			canvas_size = World::UI_CANVAS.get()->Size();
			parentx = 0;
			parenty = 0;
			parentw = canvas_size.x;
			parenth = canvas_size.y;
			parentRect = { (float)parentx, (float)parenty, (float)parentw, (float)parenth};
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
		
		resize(w, cx, anchor.x, anchor.width, parentRect.x, parentRect.width, rawPosition.x, m_Rect.width, defaultW, parentDefaultW, std::min(scaleX, scaleY), World::UI_CANVAS.get()->StrechX(), scaleX > scaleY, isScaleLocked);

		resize(h, cy, anchor.y, anchor.height, parentRect.y, parentRect.height, rawPosition.y, m_Rect.height, defaultH, parentDefaultH, std::min(scaleX, scaleY), World::UI_CANVAS.get()->StrechY(), scaleX < scaleY, isScaleLocked);
	
		m_owner.get()->SetSize({ w, h });
		m_owner.get()->SetLocation({ cx, cy });
		// element pixel size based on design size and scale
		//isScaleLocked = false;// To be Deleted;
		/*if (isScaleLocked) {
			pixelW = std::max(1.f, m_owner.get()->GetDefaultWidth());
			pixelH = std::max(1.f, m_owner.get()->GetDefaultHeight());
		}
		else {

			pixelW = std::max(1.f, m_owner.get()->GetDefaultWidth() * std::min(scaleX, 1.f));
			pixelH = std::max(1.f, m_owner.get()->GetDefaultHeight() * std::min(scaleY, 1.f));
		}

		anchorLeft = parentRect.width * anchor.x;
		anchorRight = parentRect.width * anchor.width;
		anchorx = parentRect.x + anchorLeft + (anchorRight - anchorLeft) / 2.f;

		anchorTop = parentRect.height * anchor.y;
		anchorBottom = parentRect.height * anchor.height;
		anchory = parentRect.y + anchorTop + (anchorBottom - anchorTop) / 2.f;*/

		//if (std::abs(anchor.width - anchor.x) > 0)
		//{
		//	float left  = parentRect.x + anchor.x * parentRect.width; // base width
		//	float right = left + anchor.width * parentRect.width;
		//	w = right - left ;			
		//	cx = left + m_Rect.x;
		//}
		//else{
		//
		//	w = std::min(pixelW, availW);
		//	if (!isScaleLocked) {
		//		if (scaleX > scaleY) {
		//			int dw = m_owner.get()->GetDefaultWidth();
		//			int pw = parent.expired() ? World::UI_CANVAS.get()->GetDefaultCanvasSize().x : parent.lock().get()->GetDefaultWidth();
		//			float parentratioW = (float)dw / (float)pw;
		//			scaleX = parentratioW * World::UI_CANVAS.get()->StrechX();
		//			w = std::max((float)w, parentRect.width * scaleX);
		//			cx = parent.expired() ? World::UI_CANVAS.get()->GetDefaultCanvasSize().x * anchor.x : parentRect.x + anchor.x * parentRect.width;
		//		}
		//	}
		//}

		//if (std::abs(anchor.height- anchor.y) > 0)
		//{
		//	
		//	float top = anchor.y * parentRect.height; // base width
		//	float bottom = anchor.height * parentRect.height;
		//	h = bottom - (top + m_Rect.y);
		//	cy = parentRect.y + top + m_Rect.y;
		//}
		//else {
		//h = std::min(pixelH, availH);
		//	if (!isScaleLocked) {
		//		if (scaleX < scaleY) {
		//			int dh = m_owner.get()->GetDefaultHeight();
		//			int ph = parent.expired() ? World::UI_CANVAS.get()->GetDefaultCanvasSize().y : parent.lock().get()->GetDefaultHeight();
		//			float parentratioH = (float)dh / (float)ph;
		//			scaleY = parentratioH * World::UI_CANVAS.get()->StrechY();
		//			h = std::max((float)h, parentRect.height * scaleY);
		//		}
		//	}
		//}
		// place element at left top of anchor rect with clamping
//#pragma region  commented for now
//		anchorLeft = parentRect.width * anchor.x;
//		anchorRight = parentRect.width * anchor.width ;
//		anchorTop = parentRect.height * anchor.y;
//		anchorBottom = parentRect.height * anchor.height ;
//
//		anchorx = parentRect.x + anchorLeft + (anchorRight - anchorLeft) / 2.f;
//		anchory = parentRect.y + anchorTop + (anchorBottom - anchorTop) / 2.f;
//		midAnchor = {	(float)anchorx, (float)anchory};
//
//		// Size
//		
//		w = std::min(pixelW, availW);
//		h = std::min(pixelH, availH);
//		if (!isScaleLocked) {
//
//			if (scaleX < scaleY) {
//				int dh = m_owner.get()->GetDefaultHeight();
//				int ph = parent.expired() ? World::UI_CANVAS.get()->GetDefaultCanvasSize().y : parent.lock().get()->GetDefaultHeight();
//				float parentratioH = (float)dh / (float)ph;
//				scaleY = parentratioH * World::UI_CANVAS.get()->StrechY();
//				h = std::max((float)h, parentRect.height * scaleY);
//			}
//			else if(scaleX > scaleY){
//				int dw = m_owner.get()->GetDefaultWidth();
//				int pw = parent.expired() ? World::UI_CANVAS.get()->GetDefaultCanvasSize().x : parent.lock().get()->GetDefaultWidth();
//				float parentratioW = (float)dw / (float)pw;
//				scaleX = parentratioW * World::UI_CANVAS.get()->StrechX();
//				w = std::max((float)w, parentRect.width * scaleX);
//			}
//		}
//#pragma endregion	
		//ownerpivot = m_owner.get()->GetPivot();
		/*	m_owner.get()->SetSize({(float)w, (float)h});
			Vector2 rawLoc = m_owner.get()->GetRawLocation();*/
	/*	if (std::abs(anchor.width - anchor.x) == 0) {
			midAnchor = { (float)anchorx, (float)anchory };
			if (isScaleLocked) {

				cx = midAnchor.x + rawLoc.x;
				
			}
			else {
				cx =  midAnchor.x + (float)(rawLoc.x * scaleX);
			}
			m_owner.get()->SetLocation({(float)cx, (float)cy});
		

		}*/

		//if (std::abs(anchor.height - anchor.y) == 0) {
		//	midAnchor = { (float)anchorx, (float)anchory };
		//	// Location
		//	if (isScaleLocked) {
		//		cy = midAnchor.y + rawLoc.y;
		//	}
		//	else {
		//		cy = midAnchor.y + (float)(rawLoc.y * scaleY);
		//	}
		//	m_owner.get()->SetLocation({ (float)cx, (float)cy });


		//}
		
	

		// Safety check: is element outside safeRect?
		if (cx < safeRect.x || cy < safeRect.y ||
			cx + w > safeRect.x + w ||
			cy + h > safeRect.y + h) {
			// For demo we print; in real system we'd emit warning event
		//	std::cout << "[WARN] Element '" << m_Id << "' overlaps safe area.\n";
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