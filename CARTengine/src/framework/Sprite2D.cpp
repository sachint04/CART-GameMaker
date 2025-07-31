#include "Sprite2D.h"
#include <memory>
#include "AssetManager.h"
namespace cart {
#pragma region Constructor & Init


	Sprite2D::Sprite2D(World* _owningworld, const std::string& _id, bool isExcludedFromParentAutoControl)
		:UIElement{_owningworld, _id, isExcludedFromParentAutoControl },
		m_strTexture{},
		m_bAspectRatio{ false },	
		m_textureColor{ WHITE },
		m_bMasked{ false },
		m_screenMask{},
		m_texturetype{ TEXTURE_FULL },
		m_texturesource{},
		m_textureStatus{},
		m_textureLocation{},
		m_ImgCopy{NULL, 0,0},
		m_textureSize{},
		m_bIsScaling{false}
	{
	}

	void Sprite2D::Init()
	{
		UIElement::Init();
	}
#pragma endregion

#pragma region  LOOP


	void Sprite2D::Update(float _deltaTime)
	{
		if (!m_visible || m_pendingUpdate)return;
		UIElement::Update(_deltaTime);

		if (m_strTexture.size() > 0 ) {
			//m_textureLocation = m_calculatedLocation;
		
			m_texture2d = AssetManager::Get().LoadTextureAsset(m_strTexture, m_textureStatus);
			
			if (m_texture2d == nullptr) return;

			if (m_bAspectRatio)
			{
				UpdateAspectRatio();
			}
			else {
				
				ResizeImage();
			}
		}
		if (m_bMasked && !m_bIsScaling) {
			UpdateMask();
		}
		m_bIsScaling = false;
 	}
	void Sprite2D::Draw(float _deltaTime)
	{
		

		if (!m_visible || m_pendingUpdate)return;
		UIElement::Draw(_deltaTime);

		if (m_strTexture.size() > 0) {
			
			m_texture2d = AssetManager::Get().LoadTextureAsset(m_strTexture, m_textureStatus);

			if (m_texture2d == nullptr) return;

			if (m_texturetype == TEXTURE_PART) {// Render PART OF TEXTURE			
				DrawTextureRec(*m_texture2d, m_texturesource, m_textureLocation, m_textureColor);
			}
			else {
				
				DrawTextureEx(*m_texture2d, m_textureLocation, m_rotation, 1.f, m_textureColor);
			}
		}
	}

	void Sprite2D::LateUpdate(float _deltaTime)
	{
		if (!m_visible || m_pendingUpdate)return;
		UIElement::LateUpdate(_deltaTime);
		/*if ((m_ImgCopy.data != NULL)) {
			UnloadImage(m_ImgCopy);
		};*/
	}


#pragma endregion

#pragma region Helper
	void Sprite2D::SetScreenMask(const Image& strmask)
	{
		m_screenMask = strmask;
		m_bMasked = true;

		//	LOG("mask width %d height %d", m_screenMask.width, m_screenMask.height);

	}
	void Sprite2D::SetTexture(shared<Texture2D> _tex)
	{
		if (m_texture2d != nullptr) {
			UnloadTexture(*m_texture2d);
			m_texture2d.reset();
		}

		m_texture2d = _tex;
	}
	void Sprite2D::ReEvaluteTexture()
	{
		
		//ImageResize(imageref, m_textureSize.x, m_textureSize.y);
	}

	void Sprite2D::SetTexture(std::string& _texture)
	{
		if (m_texture2d) {
			AssetManager::Get().UnloadTextureAsset(m_strTexture);
			m_texture2d = nullptr;
		}
		m_strTexture = _texture;

	}
	void Sprite2D::SetSize(Vector2 _size) {
		UIElement::SetSize(_size);	
		m_textureSize = _size;
		m_bIsScaling = true;
	}
	
	void Sprite2D::SetLocation(Vector2 _location)
	{
		UIElement::SetLocation(_location);
	}
	void Sprite2D::UpdateLocation()
	{
		UIElement::UpdateLocation();

	}
	void Sprite2D::SetUIProperties(UI_Properties _prop)
	{
		UIElement::SetUIProperties(_prop);
		m_strTexture = _prop.texture;
		m_textureColor = _prop.textureColor;
		m_texturetype = _prop.texturetype;
		m_texturesource = _prop.texturesource;
		m_textureStatus = _prop.texturestatus;
	}
	Rectangle Sprite2D::GetTextureBounds() {
		//	return{ m_location.x - m_pivot.x, m_location.y - m_pivot.y, m_width * m_scale,m_height * m_scale };
		return{ m_textureLocation.x, m_textureLocation.y, m_textureSize.x,  m_textureSize.y };
	}
	void Sprite2D::UpdateMask() {

		Image* imageref = AssetManager::Get().GetImage(m_strTexture);
		m_ImgCopy = ImageCopy(*imageref);
		ImageResize(&m_ImgCopy, m_textureSize.x, m_textureSize.y);
		ImageFormat(&m_ImgCopy, 7);
		
		if ((m_ImgCopy.data == NULL) || (m_ImgCopy.width == 0) || (m_ImgCopy.height == 0)) {
			
			LOG("ERROR! MASKED ELEMENT REQUIRED BASE IMAGE POINTER!"); return; 
		
		};


		if (m_screenMask.width != GetScreenWidth() || m_screenMask.height != GetScreenHeight())
			LOG("ERROR! Screen Mask size does not match with screen size.");
		
		if(imagepixel)delete imagepixel;

		if (maskpixels)delete maskpixels;

		imagepixel = LoadImageColors(m_ImgCopy);
		maskpixels = LoadImageColors(m_screenMask);

		Rectangle rect = GetBounds();
		for (int i = 0; i < m_ImgCopy.width * m_ImgCopy.height; i++) {
			int y = ceil(i / m_ImgCopy.width);
			int x = (i % (int)m_ImgCopy.width) + 1;
			int screenx = m_textureLocation.x + x;
			int screeny = m_textureLocation.y + y;
			if (screenx < 0 || screenx >= GetScreenWidth() || screeny < 0 || screeny >= GetScreenHeight())continue;

			int index = (screeny * GetScreenWidth()) + screenx;
			Color maskcol = maskpixels[index - 1];
			imagepixel[i].a = maskcol.a;
		}
		
		AssetManager::Get().UpdateTextureFromImage(m_strTexture, {0,0, (float)m_ImgCopy.width, (float)m_ImgCopy.height }, imagepixel);
		
		
		UnloadImage(m_ImgCopy);		
		
	}
	void Sprite2D::ResizeImage() {
		m_texture2d = AssetManager::Get().LoadTextureAsset(m_strTexture, m_textureStatus);

		float tmpscalex = (float)m_width / (float)m_texture2d.get()->width;
		float tmpscaley = (float)m_height / (float)m_texture2d.get()->height;

		m_textureLocation = m_calculatedLocation;

		if (tmpscalex == 1 && tmpscaley == 1)return;

		int width = (m_texture2d.get()->width * tmpscalex);
		int height = (m_texture2d.get()->height * tmpscaley);

		m_textureSize = { (float)width, (float)height };
		if (m_textureStatus == LOCKED) {
			AssetManager::Get().ResizeImage(m_strTexture, width, height);
		}
		else {
			m_texture2d.get()->width = width;
			m_texture2d.get()->height = height;
		}
	
	}
	void Sprite2D::UpdateAspectRatio()
	{
		m_texture2d = AssetManager::Get().LoadTextureAsset(m_strTexture, m_textureStatus);
		float tmpscale = std::min(m_width / m_texture2d.get()->width, m_height / m_texture2d.get()->height);

		int width = (m_texture2d.get()->width * tmpscale);
		int height = (m_texture2d.get()->height * tmpscale);
		
		if (tmpscale != 1.f) {
			AssetManager::Get().ResizeImage(m_strTexture, width, height);
		}



		m_textureSize = { (float)width, (float)height };	
		m_textureLocation = { (GetBounds().x + GetBounds().width / 2.f) - (width / 2.f),
							(GetBounds().y + GetBounds().height  / 2.f) - (height / 2.f)};

	}
#pragma endregion


#pragma region CLEAN UP

	void Sprite2D::Destroy()
	{		
		m_texture2d.reset();
		UIElement::Destroy();
	}

	Sprite2D::~Sprite2D()
	{
	}
#pragma endregion
}