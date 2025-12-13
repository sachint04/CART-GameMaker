#include "UIElement.h"
#include "AssetManager.h"
#include "Text.h"
#include "UIButton.h"
#include <memory>
#include "World.h"
#include "Logger.h"
#include "UICanvas.h"


extern int CANVAS_X;
extern int CANVAS_Y;
extern float CANVAS_SCALE_X;
extern float CANVAS_SCALE_Y;
extern int CANVAS_WIDTH;
extern int CANVAS_HEIGHT;

namespace cart {
#pragma region  Constructors
	UIElement::UIElement(World* _owningworld, const std::string& _id, bool isExcludedFromParentAutoControl)
		:Actor{ _owningworld,  _id },
		m_pendingUpdate{ true },
		m_pivot{ 0, 0 },
		m_rawlocation{},
		m_defaultSize{},
		m_isExcludedFromParentAutoControl{ isExcludedFromParentAutoControl },
		m_shapeType{ SHAPE_TYPE::RECTANGLE },
		m_parent{ shared<UIElement>{nullptr} }

	{
		m_anchor = { 0.f, 0.f, 1.f, 1.f }; 
		m_pivot = { 0.f, 0.f };
	}

#pragma endregion

#pragma region  Initialization
	// VIRTUAL METHOD 
	void UIElement::Init()
	{
		UICanvas::Get().lock()->onScreenSizeChange.BindAction(GetWeakRef(), &UIElement::OnScreenSizeChange);
		for (auto iter = m_children.begin(); iter != m_children.end(); ++iter)
		{
			iter->get()->Init();
		}
		Actor::Init();
	}
	void UIElement::Start()
	{		
		if (m_children.size() == 0) {
			Actor::Start();// There are no childres hence set Ready
		}
		
		//if (m_parent.expired()) {
		//	UICanvas::Get().lock()->UpdateLayout();// Update Layout 
		//}
		//onReady.Broadcast(GetId());

	}
#pragma endregion

#pragma region GAME LOOP

	void UIElement::Update(float _deltaTime)
	{
		if (!m_active || !m_visible)return;
	}

	void UIElement::Draw(float _deltaTime)
	{
		if (!m_visible)return;
	
		DrawBGColor();
	}
	void UIElement::LateUpdate(float _deltaTime)
	{

	}
#pragma endregion

#pragma region Set Properties
	void UIElement::SetUIProperties(UI_Properties _prop)
	{
		SetLocation(_prop.location);
		m_shapeType = _prop.shapetype;
		m_defaultSize = _prop.size;
		m_scale = _prop.scale;
		m_color = _prop.color;
		m_pivot = _prop.pivot;
		m_anchor = _prop.anchor;
		SetSize(_prop.size);
		if (_prop.component != NO_COMPONENT)AddComponent(_prop.component);
	}
#pragma endregion

#pragma region  Helpers
	void UIElement::SetSize(Vector2 _size) {
		m_width = _size.x;
		m_height = _size.y;
		//UpdateLocation();
	}

	void UIElement::LoadAssets()
	{
		Actor::LoadAssets();
	}

	void UIElement::SetActive(bool _flag)
	{
		Actor::SetActive(_flag);
		for (auto iter = m_children.begin(); iter != m_children.end(); ++iter)
		{
			if (!iter->get()->IsExcludedFromParentAutoControl())
				iter->get()->SetActive(_flag);
		}
	}

	void UIElement::SetVisible(bool _flag)
	{
		std::vector<shared<UIElement>>::iterator iter;

		for (iter = m_children.begin(); iter != m_children.end();++iter)
		{
			if(!iter->get()->IsExcludedFromParentAutoControl())
				iter->get()->SetVisible(_flag);
			
		}
		Actor::SetVisible(_flag);
	}
	
	Rectangle UIElement::GetBounds() {
		//	return{ m_location.x - m_pivot.x, m_location.y - m_pivot.y, m_width * m_scale,m_height * m_scale };
		if (m_shapeType == SHAPE_TYPE::CIRCLE) {
			return { m_location.x - m_pivot.x - m_width * 0.5f, m_location.y - m_pivot.y - m_height * 0.5f, m_width * 2.f, m_height * 2.f };// shape size will change for cirle;
		}
		return{ m_location.x - m_pivot.x, m_location.y - m_pivot.y, m_width * m_scale,m_height * m_scale };
	}

	void UIElement::AddComponent(COMPONENT_TYPE type)
	{
		
		switch (type)
		{
			case LAYOUT_COMPONENT :
				if (!m_layout) {					
					shared<UIElement> owner = std::dynamic_pointer_cast<UIElement>(GetWeakRef().lock());
					LayoutComponent comp{ std::string{GetId() + "_layout"}, owner, {0.f, 1.f, 0.f, 1.f}, {m_location.x, m_location.y, m_width, m_height} };
					m_layout = std::make_shared<LayoutComponent>(comp);
					UICanvas::Get().lock()->RegisterComponent(m_layout);
					m_layout.get()->onLayoutChange.BindAction(GetWeakRef(), &UIElement::OnLayoutChange);
					Actor::AddComponent(std::string{ GetId() + "_layout" }, m_layout);
				}
				else {
					Logger::Get()->Error(std::format("UIElement::AddComponent() LAYOUT_COMPONENT is already exists in {}", GetId()));
				}
			break;
			case NO_COMPONENT :

				break;
		}
	}

	weak<LayoutComponent> UIElement::GetLayoutComponent()
	{
		return m_layout;
	}
	
	void UIElement::Notify(const std::string& strevent)
	{
	}

	void UIElement::SetScale(float _scale)
	{
		Actor::SetScale(_scale);

		for (auto iter = m_children.begin(); iter != m_children.end(); ++iter)
		{
			iter->get()->SetScale(_scale);
		}

		//UpdateLocation();
	}

	void UIElement::SetLocation(Vector2 _location)
	{
		Actor::SetLocation(_location);
	}

	void UIElement::SetPivot(Vector2 _pivot)
	{
		m_pivot = _pivot;
	}
	
	void UIElement::Offset(Vector2 _location)
	{
		Actor::Offset(_location);
	}

	void UIElement::SetAnchor(Rectangle rect)
	{
		m_anchor = rect;
	}

	void UIElement::DrawBGColor()
	{
		float scScale = UICanvas::Get().lock()->Scale();
		if (m_shapeType == SHAPE_TYPE::CIRCLE)
		{
			DrawCircle(m_location.x + m_width / 2.f  - m_pivot.x, m_location.y + m_width / 2.f  - m_pivot.y, m_width, m_color);
			//FOR TESTING
			//DrawRectangleLines(m_location.x - m_pivot.x - m_width * 0.5f, m_location.y - m_pivot.y - m_height * 0.5f, m_width * 2.f, m_height * 2.f, GREEN);

		}
		else if (m_shapeType == SHAPE_TYPE::ROUNDED_RECTANGLE)
		{
			DrawRectangleRounded({  m_location.x - m_pivot.x,  m_location.y - m_pivot.y,  m_width , m_height }, 0.1f * scScale, 2 * scScale, m_color);
		}
		else 
		{
			DrawRectangle(m_location.x - m_pivot.x, m_location.y - m_pivot.y, m_width, m_height, m_color);
			//DrawRectangle(m_location.x, m_location.y, m_width, m_height, m_color);
		}
	}

	/*void UIElement::UpdateLocation()
	{
		m_rawlocation = { m_location.x * m_scale, m_location.y * m_scale };
		m_location = { m_location.x - (m_pivot.x * m_scale) , m_location.y - (m_pivot.y * m_scale) };
	}*/

	Vector2 UIElement::GetPivot()
	{
		return { m_pivot.x * m_scale , m_pivot.y * m_scale };
	}

	Rectangle UIElement::GetAnchor()
	{
		return m_anchor;
	}
	
	void UIElement::SetExcludeFromParentAutoControl(bool _flag)
	{
		m_isExcludedFromParentAutoControl = _flag;
	}

	weak<UIElement> UIElement::parent()
	{
		return m_parent;
	}

	void UIElement::parent(weak<UIElement> parent)
	{
		m_parent = parent;
	}

	
	void UIElement::SetPendingUpdate(bool _flag)
	{
		m_pendingUpdate = _flag;
	}
	/*void UIElement::SetFlipH(bool fliph)
	{
		m_flipH = fliph;
	}
	void UIElement::SetFlipV(bool flipv)
	{
		m_flipV = flipv;
	}*/


#pragma endregion
	
#pragma region  Create Child Elements
	void UIElement::AddText(const std::string& id, Text_Properties _prop)
	{
		weak<Text> _txt = m_owningworld->SpawnActor<Text>(id);
		_prop.location = { m_location.x + _prop.location.x, m_location.y + _prop.location.y };
		_txt.lock()->SetTextProperties(_prop);
		Vector2 loc = { 0,0 };
		_txt.lock()->Init();
		_txt.lock()->SetVisible(true);
		AddChild(_txt);
	}

	weak<UIButton> UIElement::AddButton(const std::string& id, Btn_Text_Properties _prop)
	{
		weak<UIButton> _btn = m_owningworld->SpawnActor<UIButton>(id);
		AddChild(_btn);
		_btn.lock()->SetButtonProperties(_prop);
		_btn.lock()->SetVisible(true);
		_btn.lock()->Init();
		return _btn;
	}

	void UIElement::AddChild(weak<UIElement> _ui)
	{		
		_ui.lock()->onReady.BindAction(GetWeakRef(), &UIElement::OnChildReady);// listen to child ready event
		shared<UIElement> shared_ui = _ui.lock();
		weak<UIElement> self = std::dynamic_pointer_cast<UIElement>(GetWeakRef().lock());
		shared_ui.get()->parent(self);
		m_children.push_back(shared_ui);
		shared_ui.reset();
	
	}

	void UIElement::RemoveChild(const std::string& id)
	{
		for (auto iter = m_children.begin(); iter != m_children.end(); ++iter)
		{
			if (iter->get()->GetId().compare(id) == 0) {
				int cnt = iter->use_count();
				iter->reset();
				m_children.erase(iter);
				break;
			}
		}
	}

	void UIElement::UpdateLayout(int canvas_w, int canvas_h)
	{
	}
#pragma endregion

#pragma region EventHandler
	/// <summary>
	/// On Preload Page asssets
	/// </summary>
	void UIElement::AssetsLoadCompleted()
	{
		//UpdateLocation();
		m_pendingUpdate = false;
		Actor::AssetsLoadCompleted();
	}

	void UIElement::OnChildReady(const std::string& id)
	{
		Logger::Get()->Trace(std::format("UIElement child {} is ready.\n", id));
		for (auto iter = m_children.begin(); iter != m_children.end();)
		{
			if (!iter->get()->IsReady())
			{
				return;// child ements not ready. do nothing
			}
			++iter;
		}
		Actor::Start();
		
		if(m_layout)
		UICanvas::Get().lock()->UpdateLayout();// Update Layout 
		
	}

	void UIElement::OnScreenSizeChange()
	{
		// Add  Concrete Implemtation
	}

	void UIElement::OnLayoutChange()
	{
		// Add  Concrete Implemtation

		//m_layout.get()->onLayoutChange.BindAction(GetWeakRef(), &UIElement::OnLayoutChange);
	}


#pragma endregion

#pragma region  Cleanup
	void UIElement::Destroy() {
		if (m_isPendingDestroy)return;

		for (auto iter = m_children.begin(); iter != m_children.end();)
		{
			iter->get()->Destroy();
			iter = m_children.erase(iter);
		}
		SetVisible(false);
		
		Actor::Destroy();
	}

	UIElement::~UIElement()
	{
	}
#pragma endregion


}