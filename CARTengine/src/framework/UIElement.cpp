#include "UIElement.h"
#include "AssetManager.h"
#include "Text.h"
#include "UIButton.h"
#include <memory>

namespace cart {
#pragma region  Constructors
	UIElement::UIElement(World* _owningworld, const std::string& _id)
		:Actor{ _owningworld,  _id },
		m_strTexture{},
		m_pendingUpdate{ true },
		m_pivot{0, 0},
		m_rawlocation{},
		m_flipH{false},
		m_flipV{false},
		m_bAspectRatio{false},
		m_defaultSize{},
		m_textureScaleX{1.f},
		m_textureScaleY{1.f},
		m_textureColor{WHITE},
		m_bMasked{false},
		tmptex{},
		m_screenMask{},
		m_texturetype{TEXTURE_FULL},
		m_texturesource{},
		m_isExcludedFromParentAutoControl{false},
		m_shapeType{SHAPE_TYPE::RECTANGLE}
	{

	}

	UIElement::UIElement( World* _owningworld, const std::string& _id, Vector2 _size)
		:Actor{ _owningworld,  _id },
		m_strTexture{},
		m_pendingUpdate{true},
		m_pivot{ 0, 0 },
		m_rawlocation{},
		m_flipH{ false },
		m_flipV{ false },
		m_bAspectRatio{ false },
		m_defaultSize{},
		m_textureScaleX{ 1.f },
		m_textureScaleY{ 1.f },
		m_textureColor{ WHITE},
		m_bMasked{false},
		tmptex{},
		m_screenMask{},
		m_texturetype{ TEXTURE_FULL },
		m_texturesource{},
		m_isExcludedFromParentAutoControl{false},
		m_shapeType{ SHAPE_TYPE::RECTANGLE }

	{
		m_defaultSize = _size;
		Actor::m_width = _size.x;
		Actor::m_height = _size.y;
	}

	UIElement::UIElement(World* _owningworld, const std::string& _id, bool isExcludedFromParentAutoControl)
		:Actor{ _owningworld,  _id },
		m_strTexture{},
		m_pendingUpdate{ true },
		m_pivot{ 0, 0 },
		m_rawlocation{},
		m_flipH{ false },
		m_flipV{ false },
		m_bAspectRatio{ false },
		m_defaultSize{},
		m_textureScaleX{ 1.f },
		m_textureScaleY{ 1.f },
		m_textureColor{ WHITE },
		m_bMasked{ false },
		tmptex{},
		m_screenMask{},
		m_texturetype{ TEXTURE_FULL },
		m_texturesource{},
		m_isExcludedFromParentAutoControl{ isExcludedFromParentAutoControl },
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
		if (!m_active)return;
	//	LOG("UIElement::::: %s ::::UPDATE()", GetID().c_str());
	}

	void UIElement::Draw(float _deltaTime)
	{
		if (m_visible == false)return;
	//	LOG("UIElement::::: %s ::::DRAW()", GetID().c_str());
		if (m_strTexture.size() > 0) {
			DrawBGColor();
			DrawBGTexture();
		}
		else {
			DrawBGColor();
		}
	}
#pragma endregion


#pragma region Set Properties
	void UIElement::SetUIProperties(UI_Properties _prop)
	{
		m_location = _prop.location;
		m_rawlocation = _prop.location;
		m_scale = _prop.scale;
		m_color = _prop.color;

		m_strTexture = _prop.texture;
		m_pivot = _prop.pivot;
		m_defaultSize = _prop.size;
		m_width = _prop.size.x,
			m_height = _prop.size.y;
		m_textureColor = _prop.textureColor;
		m_texturetype = _prop.texturetype;
		m_texturesource = _prop.texturesource;
		m_shapeType = _prop.shapetype;

		UpdateLocation();
	}
#pragma endregion

#pragma region  Helpers
	void UIElement::SetSize(Vector2 _size) {
		m_textureScaleX =  _size.x / m_defaultSize.x;
		m_textureScaleY =  _size.y / m_defaultSize.y;
		
		//Vector2 pivot = { m_pivot.x - m_pivot.x * m_textureScaleX, m_pivot.y - m_pivot.y * m_textureScaleY};
		//m_pivot = pivot;
		//LOG("SetSize() pivot  x -  %.2f y - %.2f  scale  x - %.2f y - %.2f   ", m_pivot.x, m_pivot.y, m_textureScaleX, m_textureScaleY);
		
		m_width = _size.x;
		m_height = _size.y;
		UpdateLocation();
	}

	void UIElement::SetTexture(std::string& _texture)
	{
		m_strTexture = _texture;
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

	void UIElement::DrawBGTexture()
	{
		if (!m_texture2d) {
			 m_texture2d = AssetManager::Get().LoadTextureAsset(m_strTexture);
		//	 LOG("UIElement DrawTexture() usecount %lu ", m_texture2d.use_count());
		}
		
		if (m_texturetype == TEXTURE_PART) {
			DrawTextureRec(*m_texture2d,m_texturesource, m_calculatedLocation, m_textureColor);
		}
		else {
			if (m_bAspectRatio == true)
			{
				float tmpscale = std::min((float)m_width / (float)m_texture2d.get()->width, (float)m_height / (float)m_texture2d.get()->height);

				Vector2 textLoc = { ((m_location.x + m_width / 2.f) - (m_texture2d.get()->width * tmpscale) / 2.f) - m_pivot.x,
									((m_location.y + m_height / 2.f) - (m_texture2d.get()->height * tmpscale) / 2.f) - m_pivot.y };
				//	LOG("DrawBGTexture x  %.2f  y %.2f scale %.2f ", textLoc.x, textLoc.y, tmpscale);
				if (m_bMasked)
				{

					Image* tmpImage = new Image{ LoadImageFromTexture(*m_texture2d) };
					ImageFormat(tmpImage, 7);
					ImageResize(tmpImage, tmpImage->width * tmpscale, tmpImage->height * tmpscale);


					for (int i = 0, k = 3; (i < tmpImage->width * tmpImage->height); i++, k += 4)
					{
						//if (i == image.width * image.height - 1) {
						int y = textLoc.y + ceil(i / tmpImage->width);
						int x = textLoc.x + (i % tmpImage->width);


						if (x >= 0 && x < GetScreenWidth() - 1 && y >= 0 && y < GetScreenHeight() - 1)
							((unsigned char*)tmpImage->data)[k] = ((unsigned char*)m_screenMask.data)[(m_screenMask.width * y + x) * 4];

					}


					UnloadTexture(tmptex);
					tmptex = LoadTextureFromImage(*tmpImage);
					DrawTextureEx(tmptex, textLoc, m_rotation, 1.f, m_textureColor);
					//UnloadTexture(*m_texture2d);
					UnloadImage(*tmpImage);
					delete tmpImage;


				}
				else
				{
					DrawTextureEx(*m_texture2d, textLoc, m_rotation, tmpscale, m_textureColor);
				}
			}
			else {

				m_texture2d.get()->width = m_width;
				m_texture2d.get()->height = m_height;
				SetTextureFilter(*m_texture2d, TEXTURE_FILTER_TRILINEAR);
				DrawTextureEx(*m_texture2d, m_calculatedLocation, m_rotation, 1.f, m_textureColor);
			}
		}
		

//			if (m_flipH) {
//				
//				/*Image img = LoadImageFromTexture(*m_texture2d);
//				Image* img_ptr = new Image{ img };
//				ImageFlipHorizontal(img_ptr);
//				Texture2D tmpTexture2d = LoadTextureFromImage(*img_ptr);
//				UnloadImage(img);
//				delete img_ptr;
//
//				DrawTextureV(tmpTexture2d, m_calculatedLocation, WHITE);*/
//				
//				LOG("image fliped!");
//			}			
//			else {
//				DrawTextureEx(*m_texture2d, imgCenterLoc, m_rotation, m_textureScale, WHITE);
//
////				DrawTextureV(*texture2d, m_calculatedLocation, WHITE);
//			}
	//		m_texture2d.reset();			
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

	void UIElement::SetScreenMask(const Image &strmask)
	{
		m_screenMask = strmask;
		m_bMasked = true;

	//	LOG("mask width %d height %d", m_screenMask.width, m_screenMask.height);
		
	}

	void UIElement::SetExcludeFromParentAutoControl(bool _flag)
	{
		m_isExcludedFromParentAutoControl = _flag;
	}

#pragma endregion
	
#pragma region  Create Child Elements
	void UIElement::AddText(const std::string& id, Text_Properties _prop)
	{
		weak<Text> _txt = m_owningworld->SpawnActor<Text>(id, _prop.size);
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
				LOG("Removing %s from child list. usecout is %ld", id.c_str(), iter->use_count());
				iter->reset();
				m_children.erase(iter);
				break;
			}
		}
	}
#pragma endregion

#pragma region  Cleanup
	void UIElement::SetPendingUpdate(bool _flag)
	{
		m_pendingUpdate = _flag;
	}
	void UIElement::SetFlipH(bool fliph)
	{
		m_flipH = fliph;
	}
	void UIElement::SetFlipV(bool flipv)
	{
		m_flipV = flipv;
	}
	void UIElement::ClearTexture() {
	
		if (m_texture2d) {
			UnloadTexture(*m_texture2d);
			m_texture2d.reset();
		}
	}
	
	void UIElement::Destroy() {
		LOG("UIElemente | %s | Destroyed!!", m_id.c_str());
		for (auto iter = m_children.begin(); iter != m_children.end();)
		{
			iter->get()->Destroy();
			iter->get()->SetVisible(false);
			iter = m_children.erase(iter);
		}
		m_children.clear();	
		

		ClearTexture();
		SetVisible(false);
		Actor::Destroy();
	}
	UIElement::~UIElement()
	{
		//LOG("%s UIElement deleted!", m_id.c_str());
	}
#pragma endregion


}