#include "Sprite2D.h"
#include "AssetManager.h"

namespace cart {
#pragma region Constructor & Init


	Sprite2D::Sprite2D(World* _owningworld, const std::string& _id, bool isExcludedFromParentAutoControl)
		:UIElement{_owningworld, _id, isExcludedFromParentAutoControl },
		m_strTexture{},
		m_bAspectRatio{ false },
		m_textureScaleX{ 1.f },
		m_textureScaleY{ 1.f },
		m_textureColor{ WHITE },
		m_bMasked{ false },
		m_screenMask{},
		m_texturetype{ TEXTURE_FULL },
		m_texturesource{},
		m_textureStatus{},
		m_textLoc{},
		m_ImgCopy{},
		m_textureLocation{}
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
		if (m_strTexture.size() > 0) {
			m_texture2d = AssetManager::Get().LoadTextureAsset(m_strTexture, m_textureStatus);
			m_textureLocation = m_calculatedLocation;
			if (m_bAspectRatio)
			{
				UpdateAspectRatio();
			}
			if (m_bMasked) {
				UpdateMask();
			}
		}
	}
	void Sprite2D::Draw(float _deltaTime)
	{
		if (!m_visible || m_pendingUpdate)return;
		UIElement::Draw(_deltaTime);
		if (m_texture2d) {
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
		if ((m_ImgCopy.data != NULL)) {
			UnloadImage(m_ImgCopy);		
		};
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
		if (m_texture2d) {
			UnloadTexture(*m_texture2d);
			m_texture2d.reset();
		}

		m_texture2d = _tex;
	}
	void Sprite2D::SetTexture(std::string& _texture)
	{
		if (m_texture2d) {
			AssetManager::Get().UnloadTextureAsset(m_strTexture);
			m_texture2d = NULL;
		}
		m_strTexture = _texture;

	}
	void Sprite2D::SetSize(Vector2 _size) {
		m_textureScaleX = _size.x / m_defaultSize.x;
		m_textureScaleY = _size.y / m_defaultSize.y;
		UIElement::SetSize(_size);
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
		return{ m_textLoc.x, m_textLoc.y, m_texture2d->width * m_textureScaleX,  m_texture2d->height * m_textureScaleX };
	}
	void Sprite2D::UpdateMask(){

		float tmpscale = std::min((float)m_width / (float)m_texture2d.get()->width, (float)m_height / (float)m_texture2d.get()->height);

		Image* imageref = AssetManager::Get().GetImage(m_strTexture);
		m_ImgCopy = ImageCopy(*imageref);
		if ((m_ImgCopy.data == NULL) || (m_ImgCopy.width == 0) || (m_ImgCopy.height == 0)) { LOG("ERROR! MASKED ELEMENT REQUIRED BASE IMAGE POINTER!"); return; };

		Color* imagepixel = LoadImageColors(m_ImgCopy);

		ImageFormat(&m_ImgCopy, 7);
		ImageResize(&m_ImgCopy, m_ImgCopy.width * tmpscale, m_ImgCopy.height * tmpscale);


		for (int i = 0, k = 3; i < m_ImgCopy.width * m_ImgCopy.height; i++, k += 4)
		{
			//if (i == image.width * image.height - 1) {
			int y = m_textLoc.y + ceil(i / m_ImgCopy.width);
			int x = m_textLoc.x + (i % m_ImgCopy.width);

			if (x >= 0 && x < GetScreenWidth() && y >= 0 && y < GetScreenHeight() - 1)
			{

				Color maskcol = GetImageColor(m_screenMask, x, y);
				imagepixel[i].a = maskcol.a;
				//	imagepixel[i].a = ((unsigned char*)m_screenMask.data)[(m_screenMask.width * y + x) * 4];
			}
		}
		UpdateTexture(*m_texture2d, imagepixel);
	}
	void Sprite2D::UpdateAspectRatio()
	{
		float tmpscale = std::min((float)m_width / (float)m_texture2d.get()->width, (float)m_height / (float)m_texture2d.get()->height);
		m_textureScaleX = tmpscale;
		m_textureScaleY = tmpscale;		
		
		float width = (m_texture2d.get()->width * m_textureScaleX);
		float height = (m_texture2d.get()->height * m_textureScaleY);

		
		m_texture2d.get()->width = width;
		m_texture2d.get()->height = height;
		m_textureLocation = { ((m_location.x + m_width / 2.f) - (m_texture2d.get()->width * tmpscale) / 2.f) - m_pivot.x,
							((m_location.y + m_height / 2.f) - (m_texture2d.get()->height * tmpscale) / 2.f) - m_pivot.y };

	}
#pragma endregion


#pragma region CLEAN UP

	void Sprite2D::Destroy()
	{
		UIElement::Destroy();
	}

	Sprite2D::~Sprite2D()
	{
	}
#pragma endregion
}