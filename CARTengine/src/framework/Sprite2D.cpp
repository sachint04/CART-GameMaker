#include "Sprite2D.h"
#include <memory>
#include "AssetManager.h"
#include "Logger.h"
#include "Material.h"
#include "Delegate.h"
namespace cart {
#pragma region Constructor & Init


	Sprite2D::Sprite2D(World* _owningworld, const std::string& _id, bool isExcludedFromParentAutoControl)
		:UIElement{_owningworld, _id, isExcludedFromParentAutoControl },
		m_strTexture{},	
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
	void Sprite2D::Start()
	{
		UIElement::Start();
		
	}
#pragma endregion

#pragma region  LOOP


	void Sprite2D::Update(float _deltaTime)
	{
		if (!m_visible || m_pendingUpdate)return;
		UIElement::Update(_deltaTime);

		if (!m_strTexture.empty() ) {
			Rectangle rect = GetBounds();
			if (m_texturetype == TEXTURE_PART) {
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
			bool useShader = (m_material.lock() && m_material.lock()->IsMaterialActive() && m_material.lock()->IsReady());
			
			if (useShader) m_material.lock()->Apply();// start shader mode 

			if (m_texturetype == TEXTURE_PART) {// Render PART OF TEXTURE			
				DrawTextureRec(*m_texture2d, m_texturesource, m_textureLocation, m_textureColor);
			}
			else {	
				Rectangle rect = GetBounds();
				DrawTextureEx(*m_texture2d, m_textureLocation, m_rotation, m_scale, m_textureColor);
			}
			if (useShader) m_material.lock()->Detach(); // end shader mode 
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

		if (maskpixels)
			UnloadImageColors(maskpixels);

		maskpixels = LoadImageColors(m_screenMask);
		m_bMasked = true;

	}
	void Sprite2D::ReEvaluteTexture()
	{
		m_texture2d = AssetManager::Get().LoadTextureAsset(m_strTexture);
		if (m_texture2d) {
			ResizeImage();			
		}
	}
	void Sprite2D::SetTexture(std::string _texture)
	{
		m_strTexture = _texture;
		ReEvaluteTexture();
	}
	void Sprite2D::SetSize(Vector2 _size) {
		if (m_texturetype == TEXTURE_FULL) {
			UIElement::SetSize(_size);		
			m_bIsScaling = true;
			ResizeImage();
			if (m_bMasked) {
				UpdateMask();
			}
		}
	}
	void Sprite2D::SetScale(float _scale)
	{
		UIElement::SetScale(_scale);
	}
	void Sprite2D::SetLocation(Vector2 _location)
	{
		UIElement::SetLocation(_location);
		if (m_texturetype == TEXTURE_FULL) {
			Rectangle rect = GetBounds();
			m_textureLocation = { rect.x + (rect.width - m_textureSize.x) * 0.5f, rect.y + (rect.height - m_textureSize.y) * 0.5f };		
			if (m_bMasked) {
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
		if (m_texturetype == TEXTURE_FULL) {
			ResizeImage();
			SetLocation(_prop.location);
		}
	}
	Rectangle Sprite2D::GetTextureBounds() {
		//	return{ m_location.x - px, m_location.y - py, m_width * m_scale,m_height * m_scale };
		return{ m_textureLocation.x, m_textureLocation.y, m_textureSize.x,  m_textureSize.y };
	}
	void Sprite2D::UpdateMask() {

		m_texture2d = AssetManager::Get().LoadTextureAsset(m_strTexture, m_textureStatus);
		Image m_ImgCopy = LoadImageFromTexture(*m_texture2d);
		
		if ((m_ImgCopy.data == NULL) || (m_ImgCopy.width == 0) || (m_ImgCopy.height == 0)) {		
			Logger::Get()->Error("Sprite2D::UpdateMask() | ERROR! MASKED ELEMENT REQUIRED BASE IMAGE POINTER!"); 
			return;
		};
		if (!IsImageValid(m_screenMask) || m_screenMask.width != GetScreenWidth() || m_screenMask.height != GetScreenHeight())
		{			
			Logger::Get()->Error(" Sprite2D::UpdateMask() | ERROR! Screen Mask size does not match with screen size.");
			return;
		}
		int x, y, screenx, screeny, index;
		
		Color* imagepixel = LoadImageColors(m_ImgCopy);

		if(!maskpixels)
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
		
		UnloadImageColors(imagepixel);
		UnloadImage(m_ImgCopy);		
		
	}
	void Sprite2D::ResizeImage() {

		int width, height; float tmpscalex, tmpscaley, px, py;
		if (m_strTexture.empty()) {
			return;
		}
		Rectangle rect = GetBounds();	
		Image* img = AssetManager::Get().GetImage(m_strTexture);
		if (img) {

			Image copy = ImageCopy(*img);
			int w2 = rect.width;
			int h2 = rect.height;
			if (m_bAspectRatio) {
				float imgRatio = (float)copy.width / (float)copy.height;
				float rectRatio = rect.width / rect.height;
				if (rectRatio > imgRatio) {
					// The target box is too wide: constrain by height, scale down width
					w2 = rect.height * imgRatio;
				}
				else {
					// The target box is too tall: constrain by width, scale down height
					h2 = rect.width / imgRatio;
				}
			}
			ImageResize(&copy, w2, h2);
			AssetManager::Get().ReplaceTextureFromImage(m_strTexture, copy, false);
			UnloadImage(copy);
			m_textureLocation = { rect.x + (rect.width - w2) * 0.5f, rect.y + (rect.height - h2) * 0.5f };
			m_textureSize = { (float)w2, (float)h2};
			m_texture2d = AssetManager::Get().LoadTextureAsset(m_strTexture, m_textureStatus);
		}
		
	
	}
	void Sprite2D::UpdateLocation()
	{
	}
	bool Sprite2D::UpdateAspectRatio()
	{
	
		if(m_strTexture.size() == 0)return false;
		Rectangle rect = GetBounds();
		Image* img = AssetManager::Get().GetImage(m_strTexture);
		Image copy = ImageCopy(*img);
		int w2 = rect.width;
		int h2 = rect.height;
		float imgRatio = (float)copy.width / (float)copy.height;
		float rectRatio = rect.width / rect.height;
		if (rectRatio > imgRatio) {
			// The target box is too wide: constrain by height, scale down width
			w2 = rect.height * imgRatio;
		}
		else {
			// The target box is too tall: constrain by width, scale down height
			h2 = rect.width / imgRatio;
		}
		ImageResize(&copy, w2, h2);
		AssetManager::Get().ReplaceTextureFromImage(m_strTexture, copy, false);
		UnloadImage(copy);
		m_textureLocation = { rect.x + (rect.width - w2) * 0.5f, rect.y + (rect.height - h2) * 0.5f };
		m_textureSize = { (float)w2, (float)h2 };
		m_texture2d = AssetManager::Get().LoadTextureAsset(m_strTexture, m_textureStatus);
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

#pragma region Event Handlers
	void Sprite2D::OnLayoutChangeHandler()
	{
		UIElement::OnLayoutChangeHandler();

	//	ResizeImage();
	}
#pragma endregion


#pragma region CLEAN UP

	void Sprite2D::Destroy()
	{		
		if (m_isPendingDestroy)return;		
		delete maskpixels;
		m_texture2d.reset();
		UIElement::Destroy();
	}

	Sprite2D::~Sprite2D()
	{
	}
#pragma endregion
}