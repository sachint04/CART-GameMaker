#include "UIElement.h"
#include "AssetManager.h"
#include "Text.h"
#include "UIButton.h"
#include <memory>
#include "World.h"
#include "Logger.h"
#include "UICanvas.h"
#include "component/LayoutComponentFactory.h"

extern int DEFAULT_CANVAS_WIDTH;
extern int DEFAULT_CANVAS_HEIGHT;
extern int SCREEN_WIDTH;
extern int SCREEN_HEIGHT;
extern float CANVAS_STRECH_X;
extern float CANVAS_STRECH_Y;

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
		m_parent{ shared<UIElement>{nullptr} },
		m_roundness{0.5f},
		m_borderwidth{0},
		m_borderColor{ GRAY },
		m_texturetype{ TEXTURE_FULL },
		m_roundnessSegments{36},
		m_isFocused{false},
		m_style{},
		m_ui_comp_factory{},
		m_layoutSize{}
	{
		m_anchor = { 0.f, 0.f, 1.f, 1.f }; 
		m_pivot = { 0.f, 0.f };
	}

#pragma endregion

#pragma region  Initialization
	// VIRTUAL METHOD 
	void UIElement::Init()
	{
		Actor::Init();
	}
	void UIElement::Start()
	{		
		if (m_children.size() == 0) {
			m_areChildrenReady = true;
			Actor::Start();// There are no childres hence set Ready
		}
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
		m_rawlocation = _prop.location;
		m_shapeType = _prop.shapetype;
		m_defaultSize = _prop.defaultSize;
		m_scale = _prop.scale;
		m_color = _prop.color;
		m_pivot = _prop.pivot;
		m_anchor = _prop.anchor;
		m_roundness = _prop.roundness;
		m_borderwidth = _prop.borderwidth;
		m_borderColor = _prop.bordercol;
		m_roundnessSegments = _prop.roundnessSegments;
		m_isLockedScale = _prop.blockscale;
		SetSize(_prop.size);
		m_rawWidth = _prop.size.x;
		m_rawHeight = _prop.size.y;
		if (_prop.component != NO_LAYOUT)AddUIComponent(_prop.component, _prop.layout_props);
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

	bool UIElement::HasTexture()
	{
		return false;
	}

	TEXTURE_TYPE UIElement::GetTextureType()
	{
		return m_texturetype;
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

		float x = m_location.x, y = m_location.y , w = m_width * m_scale, h = m_height * m_scale, px, py;
			Rectangle  pr;
		if (m_parent.expired()) {
			pr = { 0 ,0,(float)SCREEN_WIDTH, (float)SCREEN_HEIGHT };
		}
		else {
			pr = m_parent.lock().get()->GetBounds();
		}
		if (m_anchor.x != m_anchor.width) {
			float min = (m_anchor.x * pr.width);
			float max = (m_anchor.width * pr.width);
			w = max - (min + m_location.x + m_width);
			x = pr.x +  min + m_location.x;
		}
		else {
			x = pr.x + (m_anchor.x * pr.width) - (m_pivot.x * w) + m_location.x;
		}

		if (m_anchor.y != m_anchor.height) {
			float min = (m_anchor.y * pr.height);
			float max = (m_anchor.height * pr.height);
			h = max - (min + m_location.y + m_height);
			y = pr.y + min + m_location.y;
		}
		else {
			y = pr.y + m_anchor.y * pr.height -  m_pivot.y * h + m_location.y;
		}
	

		if (m_shapeType == SHAPE_TYPE::CIRCLE) {
			px = (m_pivot.x * w);
			py = (m_pivot.y * h);
			//	return{ m_location.x - px, m_location.y - m_pivot.y, m_width * m_scale,m_height * m_scale };
			return { x - px - w * 0.5f, y - m_pivot.y - h * 0.5f, w * 2.f, h * 2.f };// shape size will change for cirle;
		}
		
		return{  x,  y , w , h };
	}

	void UIElement::AddUIComponent(Layout_Component_Type type, UI_Layout_Properties layout_props)
	{
		shared<UIElement> owner = std::dynamic_pointer_cast<UIElement>(GetWeakRef().lock());
		weak<IComponent> comp;
		switch (type)
		{
			case LAYOUT :	
				comp = m_ui_comp_factory.make(LAYOUT, std::string{ GetId() + "_layout" });								
				break;
			case V_LAYOUT:
				comp = m_ui_comp_factory.make(V_LAYOUT, std::string{ GetId() + "_v_layout" });
				break;
			case H_LAYOUT:				
				comp = m_ui_comp_factory.make(H_LAYOUT, std::string{ GetId() + "_h_layout" });
				break;			
		}
		if (!comp.expired()) {
			comp.lock()->Init(owner, { 0.f, 1.f, 0.f, 1.f }, { m_location.x, m_location.y, m_width, m_height }, layout_props);
		}
	}

	float UIElement::GetDefaultWidth()
	{
		return m_defaultSize.x;
	}

	float UIElement::GetDefaultHeight()
	{
		return m_defaultSize.y;
	}

	void UIElement::SetStyle(UI_Style _style)
	{
		m_style = _style;
	}

	void UIElement::SetLayoutLocation(Vector2 _loc)
	{
		m_layoutlocation = _loc;
	}

	void UIElement::SetLayoutSize(Vector2 _size)
	{
		m_layoutSize = _size;
	}

	std::vector<weak<UIElement>> UIElement::Children()
	{
		std::vector<weak<UIElement>> weak_vec;
		std::transform(
			m_children.begin(),
			m_children.end(),
			std::back_inserter(weak_vec),
			[](const shared<UIElement>& sp) 
			{
				return weak<UIElement>(sp);
			}
		);
		return weak_vec;
	}

	bool UIElement::UpdateLayout()
	{
		return m_ui_comp_factory.UpdateUI();
	}

	bool UIElement::IsLayoutUpdated()
	{
		if (m_ui_comp_factory.HasComponents()) {
			return m_ui_comp_factory.IsAllCompUpdated();
		}
		return true;
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

	void UIElement::SetDefaultLocation(Vector2 _location)
	{
		m_rawlocation =  _location;
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
		float scScale =  World::UI_CANVAS.get()->Scale();
		
		Rectangle parentRect, rect = GetBounds();
		/*if (m_parent.expired())
		{			
			parentRect = { 0, 0, World::UI_CANVAS.get()->Size().x, World::UI_CANVAS.get()->Size().y };
		}
		else {
			parentRect = m_parent.lock()->GetBounds();
		}*/
		if (m_shapeType == SHAPE_TYPE::CIRCLE)
		{
			DrawCircle(rect.x + rect.width / 2.f, rect.y + rect.width / 2.f, m_width, m_color);
			//FOR TESTING
			//DrawRectangleLines(m_location.x - px - m_width * 0.5f, m_location.y - m_pivot.y - m_height * 0.5f, m_width * 2.f, m_height * 2.f, GREEN);

		}
		else if (m_shapeType == SHAPE_TYPE::ROUNDED_RECTANGLE)
		{
			DrawRectangleRounded({ rect.x,  rect.y ,  rect.width , rect.height}, m_roundness * scScale, 36 * scScale, m_color);

			if (m_borderwidth > 0)
			{
				int bw = std::max((int)(m_borderwidth * scScale), 1);
				DrawRectangleRoundedLinesEx(GetBounds(), m_roundness * scScale, m_roundnessSegments * scScale, (float)bw, m_borderColor);

			}
		}
		else 
		{
			DrawRectangle(rect.x, rect.y, rect.width, rect.height, m_color);
			if (m_borderwidth > 0)
			{
				int bw = std::max((int)(m_borderwidth* scScale), 1);
				DrawRectangleRoundedLinesEx(GetBounds(), 0, 0, (float)bw, m_borderColor);
				//DrawRectangleLinesEx(GetBounds(), (float)bw, m_borderColor);

			}
			//DrawRectangle(m_location.x, m_location.y, m_width, m_height, m_color);
		}
	}

	/*void UIElement::UpdateLocation()
	{
		m_rawlocation = { m_location.x * m_scale, m_location.y * m_scale };
		m_location = { m_location.x - (px * m_scale) , m_location.y - (m_pivot.y * m_scale) };
	}*/

	Vector2 UIElement::GetPivot()
	{
		return m_pivot;
	}

	Rectangle UIElement::GetAnchor()
	{
		return m_anchor;
	}

	void UIElement::SetFocused(bool _flag)
	{
		m_isFocused = _flag;
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

	weak<IComponent> UIElement::GetComponentById(const std::string& id)
	{
		if (m_ui_comp_factory.HasComponent(id)) {
			return m_ui_comp_factory.GetComponent(id);
		}
		return  shared<IComponent>{nullptr};
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

	bool UIElement::HasLayoutComponent(Layout_Component_Type type)
	{		
		return m_ui_comp_factory.HasComponent(type);
	}

	std::string UIElement::type()
	{
		return std::string{"UIElement"};
	}

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
		for (auto iter = m_children.begin(); iter != m_children.end();)
		{
			if (!iter->get()->IsReady())
			{
				//Logger::Get()->Trace(std::format("In [{}] , child [{}] is ready. but child [{}] is not ready yet!\n", GetId(), id, iter->get()->GetId()));
				return;// child ements not ready. do nothing
			}
			++iter;
		}
		m_areChildrenReady = true;

		Actor::Start();
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
		m_ui_comp_factory.Destroy();
		SetVisible(false);
		Actor::Destroy();
	}

	UIElement::~UIElement()
	{
	}
#pragma endregion


}