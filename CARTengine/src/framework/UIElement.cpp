#include "UIElement.h"
#include "AssetManager.h"
#include "Text.h"
#include "UIButton.h"
#include <memory>
#include "World.h"
#include "Logger.h"
namespace cart {
#pragma region  Constructors
	UIElement::UIElement(World* _owningworld, const std::string& _id, bool isExcludedFromParentAutoControl)
		:Actor{ _owningworld,  _id },
		m_pendingUpdate{ true },
		m_pivot{ 0, 0 },
		m_rawlocation{},
		m_defaultSize{},
		m_isExcludedFromParentAutoControl{ false },
		m_shapeType{ SHAPE_TYPE::RECTANGLE }
	{

	}


#pragma endregion

#pragma region  Initialization
	// VIRTUAL METHOD 
	void UIElement::Init()
	{
		for (auto iter = m_children.begin(); iter != m_children.end(); ++iter)
		{
			iter->get()->Init();
		}
		UpdateLocation();
		m_pendingUpdate = false;
	}
#pragma endregion

#pragma region GAME LOOP

	void UIElement::Update(float _deltaTime)
	{
		if (!m_active || !m_visible)return;
	//	Logger::Get()->Push("UIElement::::: %s ::::UPDATE()", GetID().c_str());
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
		m_location = _prop.location;
		m_rawlocation = _prop.location;
		m_scale = _prop.scale;
		m_color = _prop.color;

		m_pivot = _prop.pivot;
		m_defaultSize = _prop.size;
		SetSize(_prop.size);
		m_shapeType = _prop.shapetype;
	}
#pragma endregion

#pragma region  Helpers
	void UIElement::SetSize(Vector2 _size) {
		m_width = _size.x;
		m_height = _size.y;
		UpdateLocation();
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
		return{ m_calculatedLocation.x, m_calculatedLocation.y, m_width * m_scale,m_height * m_scale };
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

		UpdateLocation();
	}

	void UIElement::SetLocation(Vector2 _location)
	{
		Vector2 offset = { _location.x - m_location.x , _location.y - m_location.y };
		Actor::SetLocation(_location);
		for (auto iter = m_children.begin(); iter != m_children.end(); ++iter)
		{		
			Vector2 loc = iter->get()->GetLocation();
			Vector2 newloc = { loc.x + offset.x, loc.y + offset.y };
			iter->get()->SetLocation(newloc);
		}
		UpdateLocation();
	}

	void UIElement::SetPivot(Vector2 _pivot)
	{
		m_pivot = _pivot;
	}
	
	void UIElement::Offset(Vector2 _location)
	{
		Actor::Offset(_location);
	}

	void UIElement::DrawBGColor()
	{
		if (m_shapeType == SHAPE_TYPE::CIRCLE)
		{
			DrawCircle(m_calculatedLocation.x + m_width / 2.f, m_calculatedLocation.y + m_width / 2.f, m_width, m_color);

		}
		else if (m_shapeType == SHAPE_TYPE::ROUNDED_RECTANGLE)
		{
			DrawRectangleRounded({ m_calculatedLocation.x, m_calculatedLocation.y, m_width, m_height }, 0.2f, 2, m_color);
		}
		else 
		{
			DrawRectangle(m_calculatedLocation.x, m_calculatedLocation.y, m_width * m_scale, m_height * m_scale, m_color);
			//DrawRectangle(m_calculatedLocation.x, m_calculatedLocation.y, m_width, m_height, m_color);
		}
	}

	void UIElement::UpdateLocation()
	{
		m_rawlocation = { m_location.x * m_scale, m_location.y * m_scale };

		//m_calculatedLocation = { m_location.x - (m_pivot.x * m_scale) , m_location.y - (m_pivot.y * m_scale) };
		m_calculatedLocation = { m_location.x - (m_pivot.x * m_scale), m_location.y - (m_pivot.y * m_scale) };

	}

	Vector2 UIElement::GetPivot()
	{
		return { m_pivot.x * m_scale , m_pivot.y * m_scale };
	}

	

	void UIElement::SetExcludeFromParentAutoControl(bool _flag)
	{
		m_isExcludedFromParentAutoControl = _flag;
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
		_btn.lock()->SetButtonProperties(_prop);
		_btn.lock()->SetVisible(true);
		_btn.lock()->Init();
		AddChild(_btn);
		return _btn;
	}

	void UIElement::AddChild(weak<UIElement> _ui)
	{
		shared<UIElement> shared_ui = _ui.lock();
		m_children.push_back(shared_ui);
		shared_ui.reset();
	
	}

	void UIElement::RemoveChild(const std::string& id)
	{
		for (auto iter = m_children.begin(); iter != m_children.end(); ++iter)
		{
			if (iter->get()->GetID().compare(id) == 0) {
				int cnt = iter->use_count();
			//	Logger::Get()->Push(std::format("Removing {} from child list. usecout is {}", id, cnt));
				iter->reset();
				m_children.erase(iter);
				break;
			}
		}
	}
#pragma endregion

#pragma region  Cleanup



	
	
	void UIElement::Destroy() {
	//	Logger::Get()->Push(std::format("UIElemente | {} | Destroyed!!", m_id));
		for (auto iter = m_children.begin(); iter != m_children.end();)
		{
			iter->get()->Destroy();
			iter->get()->SetVisible(false);
			iter = m_children.erase(iter);
		}
		m_children.clear();	
		
		SetVisible(false);
		Actor::Destroy();
	}

	UIElement::~UIElement()
	{
		//Logger::Get()->Push("{}IElement deleted!", m_id);
	}
#pragma endregion


}