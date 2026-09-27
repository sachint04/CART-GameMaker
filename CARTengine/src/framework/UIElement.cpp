#include <memory>
#include "World.h"
#include "AssetManager.h"
#include "UIElement.h"
#include "UICanvas.h"
#include "Logger.h"
#include "component/LayoutComponentFactory.h"
#include "component/InputController.h"
#include "UIButton.h"
#include "ImageButton.h"
#include "Text.h"



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
		m_borderwidth{0},
		m_borderColor{ GRAY },
		m_texturetype{ TEXTURE_FULL },
		m_roundness{0.8f},
		m_roundnessSegments{36},
		m_isFocused{false},
		m_style{},
		m_ui_comp_factory{},
		m_layoutSize{},
		m_layoutlocation{},
		m_bAspectRatio{ false }
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
		m_owningworld->GetHUD().lock()->onModalClose.BindAction(GetWeakRef(), &UIElement::OnModalClose);
	}
#pragma endregion

#pragma region GAME LOOP

	void UIElement::Update(float _deltaTime)
	{
		if (!m_active || !m_visible)return;

		for (auto& child : m_children) {
			if (child) {
			//	Logger::Get()->Trace(std::format("UIElement::Update() child {} ", child.get()->GetId()));
				if (!child->IsPendingDestroy())
				{
					child->Update(_deltaTime);
				}
			}
		}
		Actor::Update(_deltaTime);
	}

	void UIElement::Draw(float _deltaTime)
	{
		if (!m_visible)return;
	
		DrawBGColor();
		

		for (auto& child : m_children) {
			if (child && !child->IsPendingDestroy()) {
				child->Draw(_deltaTime);
			}
		}
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
	//	m_defaultSize = _prop.defaultSize.x && _prop.defaultSize.y  != -1.f? _prop.defaultSize : _prop.size ;
		m_rawWidth = _prop.size.x;
		m_rawHeight = _prop.size.y;
		m_padding = _prop.padding;
		SetDefaultSize();
		if (_prop.component != NO_LAYOUT)AddUIComponent(_prop.component, _prop.layout_props);
	}
#pragma endregion

#pragma region  Helpers

	void UIElement::SetSize(Vector2 _size) {
		m_width = _size.x;
		m_height = _size.y;
		//UpdateLocation();
	}

	void UIElement::SetDefaultSize()
	{
		if (m_anchor.x != m_anchor.width || m_anchor.y != m_anchor.height)
		{
			Rectangle rect = {};
			weak<UIElement> parent = std::dynamic_pointer_cast<UIElement>(m_parentObj.lock());
			if (auto lock = parent.lock()) {
				rect = lock->GetBounds();
			}
			else {
				rect = World::UI_CANVAS.get()->GetBounds();
			}
			m_defaultSize = { (rect.width * m_anchor.width) - (rect.width * m_anchor.x),
								(rect.height * m_anchor.height) - (rect.height * m_anchor.y) };
		}
		else {
			m_defaultSize = { m_rawWidth, m_rawHeight };

		}
	}


	void UIElement::LoadAssets_async()
	{
		Actor::LoadAssets_async();
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
		
		float x = m_location.x, y = m_location.y , w = m_width, h = m_height, px, py;
			Rectangle  pr;
		weak<UIElement> parent = std::dynamic_pointer_cast<UIElement>(m_parentObj.lock());
		if (auto lock = parent.lock()) 
		{
			pr = parent.lock().get()->GetBounds();
		}
		else {
			pr = { 0 ,0,(float)SCREEN_WIDTH, (float)SCREEN_HEIGHT };
		}
		// Set width from Anchor position
		if (m_anchor.x != m_anchor.width) {
			float min = (m_anchor.x * pr.width);
			float max = (m_anchor.width * pr.width);
			w = max - (min + m_location.x + m_width);
			x = pr.x +  min + m_location.x;
		}
		else {
			if (m_ui_comp_factory.HasComponents())
				x = pr.x + (m_anchor.x * pr.width) - m_pivot.x * w  + m_location.x;
			else
				x = m_location.x - m_pivot.x * w;

		}
		// Set Height from Anchor position
		if (m_anchor.y != m_anchor.height) {
			float min = (m_anchor.y * pr.height);
			float max = (m_anchor.height * pr.height);
			h = max - (min + m_location.y + m_height);
			y = pr.y + min + m_location.y;
		}
		else {
			if (m_ui_comp_factory.HasComponents())
				y = pr.y + m_anchor.y * pr.height - m_pivot.y * h + m_location.y;
			else
				y = m_location.y - m_pivot.y * h;
		}

		if (m_bAspectRatio && m_anchor.x == m_anchor.width && m_anchor.y == m_anchor.height) {
			float r = m_rawHeight < m_rawWidth ?m_rawHeight / m_rawWidth : m_rawWidth / m_rawHeight;
			
			float tmpW = h < w ? h / r : w;
			float tmpH = w < h ? w * r : h;

			w = tmpW; h = tmpH;


			if (m_ui_comp_factory.HasComponents())
				y = pr.y + m_anchor.y * pr.height - m_pivot.y * h  + m_location.y;
			else
				y = m_location.y - m_pivot.y * h;

			if (m_ui_comp_factory.HasComponents())
				x = pr.x + (m_anchor.x * pr.width) - m_pivot.x * w + m_location.x;
			else
				x = m_location.x - m_pivot.x * w;
		}
	

		if (m_shapeType == SHAPE_TYPE::CIRCLE) {
			px = (m_pivot.x * w);
			py = (m_pivot.y * h);
			//	return{ m_location.x - px, m_location.y - m_pivot.y, m_width * m_scale,m_height * m_scale };
			return { x - px - w * 0.5f, y - m_pivot.y - h * 0.5f, w * 2.f, h * 2.f };// shape size will change for cirle;
		}
		
		return{  x ,  y , w , h };
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
			case NO_LAYOUT :
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

	void UIElement::OnModalClose()
	{
		m_owningworld->GetHUD().lock()->onModalClose.RemoveActionsByObjectId(GetId());
	}

	void UIElement::SetLayoutLocation(Vector2 _loc)
	{
		m_layoutlocation = _loc;
	}

	void UIElement::SetLayoutSize(Vector2 _size)
	{
		m_layoutSize = _size;
	}

	weak<UIElement> UIElement::GetUIParent()
	{
		weak<UIElement> p = std::dynamic_pointer_cast<UIElement>(m_parentObj.lock());
		return p;
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

			if (m_borderwidth > 0)
			{
				//int bw = m_borderwidth;// std::max((int)(m_borderwidth * scScale), 1);
				DrawRectangleRounded({ rect.x - m_borderwidth, rect.y - m_borderwidth, rect.width + m_borderwidth * 2, rect.height + m_borderwidth * 2 }, m_roundness, m_roundnessSegments, m_borderColor);
				//DrawRectangleRoundedLinesEx(rect, m_roundness , m_roundnessSegments , m_borderwidth, m_borderColor);

			}
			DrawRectangleRounded(rect, m_roundness, m_roundnessSegments, m_color);
		}
		else 
		{
			int bw = m_borderwidth;// std::max((int)(m_borderwidth * scScale), 1);
			if (m_borderwidth > 0)
			{
				//int bw = std::max((int)(m_borderwidth), 1);
				DrawRectangle(rect.x - m_borderwidth, rect.y - m_borderwidth, rect.width + m_borderwidth * 2, rect.height + m_borderwidth * 2, m_borderColor);
			//	DrawRectangleRoundedLinesEx({ rect.x - bw, rect.y - bw, rect.width + bw * 2, rect.height + bw * 2 }, 0, 0, (float)bw, m_borderColor);
				//DrawRectangleLinesEx(GetBounds(), (float)bw, m_borderColor);
			}
			DrawRectangle(rect.x, rect.y, rect.width, rect.height, m_color);
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

	/*weak<UIElement> UIElement::parent()
	{
		return m_parent;
	}

	void UIElement::parent(weak<UIElement> parent)
	{
		m_parent = parent;
		GetParent(m_parent);
	}*/

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
	bool UIElement::HasComponents()
	{
		return m_ui_comp_factory.HasComponents();
	}
	
	bool UIElement::HasLayoutComponent(Layout_Component_Type type)
	{		
		return m_ui_comp_factory.HasComponent(type);
	}

	std::string UIElement::type()
	{
		return std::string{"UIElement"};
	}

	bool UIElement::IsUI()
	{
		return true;
	}

	void UIElement::MaintainAspectRatio(bool _flag)
	{
		m_bAspectRatio = _flag;
	}
#pragma endregion
	
#pragma region  Create Child Elements
	weak<Text> UIElement::AddText(const std::string& id, Text_Properties _prop)
	{
		weak<Text> _txt = m_owningworld->SpawnActor<Text>(id);
		AddChild(_txt);
		if (auto lock = _txt.lock())
		{
			lock->SetTextProperties(_prop);
			lock->Init();
			lock->SetVisible(true);
		}
		return _txt;
	}

	weak<Object> UIElement::SortChildrenByZindex()
	{
		std::sort(m_children.begin(), m_children.end(), [](const shared<UIElement>& a, const shared<UIElement>& b) {
			if (!a || !b) return a < b; // Move nulls to the start
			return a->GetZindex() < b->GetZindex();
		});
		m_owningworld->GetInputController()->SortChildrenByZindex();
		return Actor::SortChildrenByZindex();
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

	weak<Object> UIElement::Child(const std::string& id)
	{
		
		auto find = std::find_if(m_children.begin(), m_children.end(), [&id](shared<UIElement>& elem)
		{
			std::string elemId = elem->GetId();
			return id == elemId;
		
		});
		if (find != m_children.end())
		{
			return *find;
		}
		return {};
	}

	bool UIElement::AddButtonElement(weak<UIButton> _btn)
	{
		return	AddChild(_btn);
	}

	bool UIElement::AddButtonElement(weak<ImageButton> _btn)
	{
		return	AddChild(_btn);
	}

	bool UIElement::AddChild(weak<Actor> elem)
	{		
		shared<UIElement> shared_ui = std::dynamic_pointer_cast<UIElement>(elem.lock());
		if (shared_ui) {
			shared_ui->onReady.BindAction(GetWeakRef(), &UIElement::OnChildReadyHandler);// listen to child ready event			
			shared_ui->SetParent(GetWeakRef());
			shared_ui->SetZindex(m_zIndex +  m_children.size(), false);
			m_children.push_back(shared_ui);
			return true;
		}
		return false;
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

	void UIElement::SetZindex(int index, bool sort)
	{
		Actor::SetZindex(index, sort);		
		if (sort) {
			auto parentlock = m_parentObj.lock();
			if (parentlock) {
				weak<UIElement> ui = std::dynamic_pointer_cast<UIElement>(parentlock);
				auto uilock = ui.lock();
				if (uilock) {
					uilock->SortChildrenByZindex();
				}
			}
			else {
				m_owningworld->SortChildrenByZindex();			
			}
			/*float cnt = 1;
			for (auto& child : m_children) {
				child->SetZindex(index + cnt);
				cnt++;
			}*/
		}
	}


	void UIElement::OnChildReadyHandler(const std::string& id)
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
		
		if(m_areChildrenReady)
		{
			UpdateLayout();
		}
		Actor::Start();
	}

	void UIElement::OnScreenSizeChangeHandler()
	{
		// Add  Concrete Implemtation
	}

	void UIElement::OnLayoutChangeHandler()
	{
		onLayoutChanged.Broadcast();
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
		m_owningworld->GetHUD().lock()->onModalClose.RemoveActionsByObjectId(GetId());
		m_ui_comp_factory.Destroy();
		onLayoutChanged.Destroy();

		SetVisible(false);
		Actor::Destroy();
	}



	UIElement::~UIElement()
	{
	}
#pragma endregion


}