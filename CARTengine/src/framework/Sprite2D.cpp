#include "Sprite2D.h"
#include <memory>
#include "AssetManager.h"
#include "Logger.h"
namespace cart {
#pragma region Constructor & Init


	Sprite2D::Sprite2D(World* _owningworld, const std::string& _id, bool isExcludedFromParentAutoControl)
		:UIElement{_owningworld, _id, isExcludedFromParentAutoControl },
		m_strTexture{},
		m_bAspectRatio{ false },	
		m_textureColor{ WHITE },
		m_bMasked{ false },
		m_screenMask{},
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
			//m_textureLocation = m_location;
			m_texture2d = AssetManager::Get().LoadTextureAsset(m_strTexture, m_textureStatus);
			Rectangle rect = GetBounds();
			int px = (m_pivot.x * rect.width);
			int py = (m_pivot.y * rect.height);
			if (m_texturetype == TEXTURE_FULL) {
				
				/*if (m_bMasked && !m_bIsScaling) {
					UpdateMask();
				}*/
				if (m_texture2d) {

					if (m_bAspectRatio) {

						UpdateAspectRatio();
					
						int w = m_texture2d.get()->width;
						int h = m_texture2d.get()->height;
						m_textureLocation = {
									(rect.x + rect.width / 2.f) - ((float)w / 2.f),
									(rect.y + rect.height / 2.f) - ((float)h / 2.f)
						};
					}
					else {
						m_texture2d.get()->width = rect.width;
						m_texture2d.get()->height = rect.height;
						m_textureLocation = { rect.x , rect.y  };
					}
				}
				/*if (m_bAspectRatio)
				{
					UpdateAspectRatio();
				}
				else {

					ResizeImage();
				}*/
				
			}
			else if (m_texturetype == TEXTURE_PART) {
				m_textureLocation = { rect.x, rect.y };
			}
		}
		m_bIsScaling = false;
 	}

	void Sprite2D::Draw(float _deltaTime)
	{
		if (!m_visible || m_pendingUpdate)return;
		UIElement::Draw(_deltaTime);

		if (m_strTexture.size() > 0) {
			
			//if (!m_texture2d) {
				m_texture2d = AssetManager::Get().LoadTextureAsset(m_strTexture, m_textureStatus);
			//}

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

	}
	void Sprite2D::ReEvaluteTexture()
	{
		
		//ImageResize(imageref, m_textureSize.x, m_textureSize.y);
	}
	void Sprite2D::SetTexture(std::string _texture)
	{
		/*if (m_texture2d) {
			if (m_textureStatus == LOCKED) {
				AssetManager::Get().SetTextureStatus(m_strTexture, UNLOCKED);
				AssetManager::Get().UnloadTextureAsset(m_strTexture);
			}
			m_texture2d = nullptr;
		}*/
		m_strTexture = _texture;
		/*if (m_strTexture.size() > 0 && !m_pendingUpdate && m_visible ) {
			m_texture2d = AssetManager::Get().LoadTextureAsset(m_strTexture, m_textureStatus);
		}*/

	}
	void Sprite2D::SetSize(Vector2 _size) {
		if (m_texturetype == TEXTURE_FULL) {
			UIElement::SetSize(_size);	
			m_textureSize = _size;
			m_bIsScaling = true;
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
	}
	void Sprite2D::SetLocation(Vector2 _location)
	{
		UIElement::SetLocation(_location);
		if (m_texturetype == TEXTURE_FULL) {
			if (m_bAspectRatio)
			{
				UpdateAspectRatio();
			}
			else {

				ResizeImage();
			}
			if (m_bMasked && !m_bIsScaling) {
				UpdateMask();
			}
		}
		
	}
	/*void Sprite2D::UpdateLocation()
	{
		UIElement::UpdateLocation();

	}*/
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
		//	return{ m_location.x - px, m_location.y - py, m_width * m_scale,m_height * m_scale };
		return{ m_textureLocation.x, m_textureLocation.y, m_textureSize.x,  m_textureSize.y };
	}
	void Sprite2D::UpdateMask() {

		Image* imageref = AssetManager::Get().GetImage(m_strTexture);
		m_ImgCopy = ImageCopy(*imageref);
		ImageResize(&m_ImgCopy, m_textureSize.x, m_textureSize.y);
		ImageFormat(&m_ImgCopy, 7);
		
		if ((m_ImgCopy.data == NULL) || (m_ImgCopy.width == 0) || (m_ImgCopy.height == 0)) {		
			Logger::Get()->Error("Sprite2D::UpdateMask() | ERROR! MASKED ELEMENT REQUIRED BASE IMAGE POINTER!"); 
			return;
		
		};


		if (m_screenMask.width != GetScreenWidth() || m_screenMask.height != GetScreenHeight())
		{			
			Logger::Get()->Error(" Sprite2D::UpdateMask() | ERROR! Screen Mask size does not match with screen size.");
			return;
		}
		int x, y, screenx, screeny, index;
		if(imagepixel)delete imagepixel;

		if (maskpixels)delete maskpixels;

		imagepixel = LoadImageColors(m_ImgCopy);
		maskpixels = LoadImageColors(m_screenMask);

		Rectangle rect = GetBounds();
		for (int i = 0; i < m_ImgCopy.width * m_ImgCopy.height; i++) {
			y = ceil(i / m_ImgCopy.width);
			x = (i % (int)m_ImgCopy.width) + 1;
			screenx = m_textureLocation.x + x;
			screeny = m_textureLocation.y + y;
			if (screenx < 0 || screenx >= GetScreenWidth() || screeny < 0 || screeny >= GetScreenHeight())continue;

			index = (screeny * GetScreenWidth()) + screenx;
			Color maskcol = maskpixels[index - 1];
			imagepixel[i].a = maskcol.a;
		}
		
		AssetManager::Get().UpdateTextureFromData(m_strTexture, {0,0, (float)m_ImgCopy.width, (float)m_ImgCopy.height }, imagepixel);
		
		
		UnloadImage(m_ImgCopy);		
		
	}
	void Sprite2D::ResizeImage() {
		if (m_strTexture.size() > 0) {
			m_texture2d = AssetManager::Get().LoadTextureAsset(m_strTexture, m_textureStatus);
		}
		if (!m_texture2d)return;
		int width, height; float tmpscalex, tmpscaley, px, py;
		Rectangle rect = GetBounds();
		tmpscalex = rect.width / (float)m_texture2d.get()->width;
		tmpscaley = rect.height / (float)m_texture2d.get()->height;
		px = (m_pivot.x * rect.width);
		py = (m_pivot.y * rect.height);
		m_textureLocation = { rect.x - px, rect.y - py };

		if (tmpscalex == 1 && tmpscaley == 1)return;

		width = (m_texture2d.get()->width * tmpscalex);
		height = (m_texture2d.get()->height * tmpscaley);

		m_textureSize = { (float)width, (float)height };
		if (m_textureStatus == LOCKED) {
			AssetManager::Get().ResizeImage(m_strTexture, width, height);
		}
		else {
			m_texture2d.get()->width = width;
			m_texture2d.get()->height = height;
		}
	
	}
	bool Sprite2D::UpdateAspectRatio()
	{
		m_texture2d = AssetManager::Get().LoadTextureAsset(m_strTexture, m_textureStatus);
		if (!m_texture2d) {
			return false;
		}
		int bw, bh, w, h;
		float stageratio, targetratio, diffratio;
		Rectangle rect = GetBounds();
		Image* img = AssetManager::Get().GetImage(m_strTexture);
		stageratio = rect.width / rect.height;

		bw = (stageratio > 1.f) ? rect.width / stageratio : rect.width ;
		bh = (stageratio <  1.f) ? rect.height * stageratio : rect.height;
		Vector2 stagesquare = { (float)bw , (float)bh};

		
		targetratio = (img->width	> img->height)? stagesquare.x / img->width : stagesquare.x / img->height;
		w = img->width * targetratio;
		h = img->height * targetratio;

		diffratio = std::min( m_width /(float)w  , m_height / (float)h );
		if (diffratio > 1) {
			w *= diffratio;
			h *= diffratio;
		}
			m_textureSize = { (float)w , (float)h };

		if (m_texture2d->width != (int)m_textureSize.x || m_texture2d->height != (int)m_textureSize.y) {
			Image copy = ImageCopy(*img);
			ImageResize(&copy, m_textureSize.x, m_textureSize.y);
			if (AssetManager::Get().ReplaceTextureFromImage(m_strTexture, copy))
			{
				//Logger::Get()->Trace(" Sprite2D::UpdateAspectRatio() Image Resized maintained Aspect Ratio");
			}
			else {
				Logger::Get()->Warn(" Sprite2D::UpdateAspectRatio() FAILED!! Image Resized maintained Aspect Ratio");
			}
			m_textureLocation = {
								(rect.x + rect.width / 2.f) - (m_textureSize.x / 2.f),
								(rect.y + rect.height  / 2.f) - (m_textureSize.y / 2.f)
								};
			UnloadImage(copy);
		}
		return true;
	}
	void Sprite2D::TransformIntrupted() {
		if (m_bMasked) {
			UpdateMask();
		}
		m_bIsScaling = false;

	}

	bool Sprite2D::HasTexture()
	{
		return m_strTexture.size() > 0;
	}
	
#pragma endregion


#pragma region CLEAN UP

	void Sprite2D::Destroy()
	{		
		if (m_isPendingDestroy)return;

		m_texture2d.reset();
		UIElement::Destroy();
	}

	

	Sprite2D::~Sprite2D()
	{
	}
#pragma endregion
}