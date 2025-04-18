#include "UIElement.h"
#include "AssetManager.h"
#include "Text.h"
#include "UIButton.h"

namespace cart {

	UIElement::UIElement(World* _owningworld, const std::string& _id)
		:Actor{ _owningworld,  _id },
		m_size(),
		m_texture{},
		m_pendingUpdate{ true },
		m_pivot{0, 0},
		m_rawlocation{}
	{

	}

	UIElement::UIElement( World* _owningworld, const std::string& _id, Vector2 _size)
		:Actor{ _owningworld,  _id },
		m_size(_size),
		m_texture{},
		m_pendingUpdate{true},
		m_pivot{ 0, 0 },
		m_rawlocation{}
	{

	}

	// VIRTUAL METHOD 
	void UIElement::Init()
	{
		if (m_texture.size() > 0) {
			m_texture2d = AssetManager::Get().LoadTextureAsset(m_texture);
			if (m_texture2d) {
				m_texture2d->width = m_size.x;
				m_texture2d->height = m_size.y;
			}
		}

		for (size_t i = 0; i < m_children.size(); i++)
		{
			m_children[i].lock()->Init();
		}
		UpdateLocation();
		m_pendingUpdate = false;
	}

	Rectangle UIElement::GetBounds() {
		return{m_location.x, m_location.y, m_size.x * m_scale, m_size.y * m_scale };
	}

    void UIElement::SetSize(Vector2 _size){
        m_size = _size;		
		UpdateLocation();
    }

	void UIElement::SetUIProperties(UI_Properties _prop)
	{
		m_location = _prop.location;
		m_calculatedLocation = _prop.location;
		m_scale = _prop.scale;
		m_color = _prop.color;
		m_scale = _prop.scale;
		m_texture = _prop.texture;
		m_size = _prop.size;
		m_pivot = _prop.pivot;
		UpdateLocation();
	}

	void UIElement::Update(float _deltaTime)
	{
		if (m_active == false)return;
		for (int i = 0; i<m_children.size(); i++)
		{
			bool isActive = m_children[i].lock()->IsActive();
			if (isActive == true) {
				
				m_children[i].lock()->Update(_deltaTime);
			}
		}
	}

	void UIElement::Draw(float _deltaTime)
	{
		if (m_visible == false)return;

		if (m_texture.size() > 0) {
			DrawBGColor();
			DrawBGTexture();
		}
		else {			
			DrawBGColor();
		}

		for (int i = 0; i < m_children.size(); i++)
		{
			m_children[i].lock()->Draw(_deltaTime);

		}

	}

	void UIElement::DrawBGColor()
	{
		DrawRectangle(m_calculatedLocation.x, m_calculatedLocation.y, m_size.x * m_scale, m_size.y * m_scale, m_color);
	}

	void UIElement::DrawBGTexture()
	{
		shared<Texture2D> texture2d = AssetManager::Get().LoadTextureAsset(m_texture);

        if (texture2d) {
			texture2d->width = m_size.x * m_scale;
			texture2d->height = m_size.y * m_scale;
		//	DrawTextureEx(*texture2d, m_location, m_rotation, m_scale, m_color);
			DrawTextureV(*texture2d, m_calculatedLocation, WHITE);
		}
	}

	void UIElement::SetTexture(std::string& _texture)
	{
		m_texture = _texture;
	}

	void UIElement::SetVisible(bool _flag)
	{
		for (int i = 0; i < m_children.size(); i++)
		{
			m_children[i].lock()->SetVisible(_flag);

		}
		Actor::SetVisible(_flag);
	}

	void UIElement::SetScale(float _scale)
	{
		Actor::SetScale(_scale);

        for (size_t i = 0; i < m_children.size(); i++)
        {
            m_children[i].lock()->SetScale(_scale);
        }

		UpdateLocation();
    }

	void UIElement::SetActive(bool _flag)
	{
		Actor::SetActive(_flag);
		for (size_t i = 0; i < m_children.size(); i++)
		{
			m_children[i].lock()->SetActive(_flag);
		}
	}

	void UIElement::SetLocation(Vector2 _location)
	{
		Vector2 offset = { _location.x - m_location.x , _location.y - m_location.y };
		Actor::SetLocation(_location);
		for (auto iter : m_children) {
			Vector2 loc = iter.lock()->GetLocation();
			Vector2 newloc = { loc.x + offset.x, loc.y + offset.y };
			iter.lock()->SetLocation(newloc);
		}
		UpdateLocation();
	}
	void UIElement::AddText(const std::string& id, Text_Properties _prop)
	{
		weak<Text> _txt = m_owningworld->SpawnActor<Text>(id, _prop.size);
		_prop.location = { m_location.x + _prop.location.x, m_location.y + _prop.location.y };
		_txt.lock()->SetTextProperties(_prop);
		Vector2 loc = { 0,0 };
		_txt.lock()->Init();
		_txt.lock()->SetVisible(true);
		AddUIElement(_txt);
	}
	void UIElement::AddText(weak<UIElement> _txt)
	{
		AddUIElement(_txt);
	}

	void UIElement::AddButton(weak<UIElement> _btn)
	{
		AddUIElement(_btn);		
	}

	void UIElement::AddUIElement(weak<UIElement> _ui)
	{
		m_children.push_back(_ui);
	}

	
	void UIElement::UpdateLocation()
	{
		m_rawlocation = { m_location.x * m_scale, m_location.y * m_scale };
		
		//m_calculatedLocation = { m_location.x - (m_pivot.x * m_scale) , m_location.y - (m_pivot.y * m_scale) };
		m_calculatedLocation = { m_location.x - (m_pivot.x * m_scale), m_location.y - (m_pivot.y * m_scale) };

	}
		
	void UIElement::SetPendingUpdate(bool _flag)
	{
		m_pendingUpdate = _flag;
	}

	

	UIElement::~UIElement()
	{
		for (size_t i = 0; i < m_children.size(); i++)
		{
			m_children[i].lock()->Destroy();
		}
		//LOG("%s UIElement deleted!", m_id.c_str());
	}
}