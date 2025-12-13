#pragma once
#include "UIElement.h"
namespace cart {
	class Sprite2D : public UIElement
	{
	public:
		Sprite2D(World* _owningworld, const std::string& _id, bool isExcludedFromParentAutoControl = false);
		~Sprite2D();

		void Init()override;
		void Update(float _deltaTime)override;
		void Draw(float _deltaTime)override;
		void LateUpdate(float _deltaTime)override;
		virtual void SetUIProperties(UI_Properties _prop) override;
		virtual void SetSize(Vector2 _size) override;
		virtual void SetLocation(Vector2 _location) override;
		//virtual void UpdateLocation() override;
		virtual void Destroy() override;

		void MaintainAspectRatio(bool _flag) { m_bAspectRatio = _flag; };
		bool IsAspectRatio() { return m_bAspectRatio; };
		Color GetTextureColor() { return m_textureColor; };
		void SetTextureColor(Color col) { m_textureColor = col; };
		void ReEvaluteTexture();
		std::string GetTexturePath()const{ return m_strTexture; };
		void SetTexture(std::string _texture);
		virtual Rectangle GetTextureBounds();
		void SetScreenMask(const Image& strmask);
		void TransformIntrupted();


	protected:
		void UpdateMask();
		void ResizeImage();
		bool UpdateAspectRatio();
		TEXTURE_TYPE  m_texturetype = TEXTURE_FULL;
		std::string m_imgstr;
		std::string m_strTexture;
		Image m_ImgCopy;
		shared<Texture2D> m_texture2d;	
		Color m_textureColor;
		Image m_screenMask;		
		Rectangle m_texturesource;
		TEXTURE_DATA_STATUS m_textureStatus;
		Vector2 m_textureLocation;
		Vector2 m_textureSize;
		bool m_bMasked;
		bool m_bIsScaling;
		bool m_bAspectRatio;
		Color* imagepixel = nullptr;
		Color* maskpixels = nullptr;
	};
}
