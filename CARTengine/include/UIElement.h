#pragma once
#include <raylib.h>
#include <string>
#include "Actor.h"
#include "Core.h"
#include "Types.h"

namespace cart {
	class World;
	class UIButton;
    class Text;


	class UIElement : public Actor{
		
	public:
		
		UIElement(World* _owningworld, const std::string& _id, Vector2 _size);

		UIElement(World* _owningworld, const std::string& _id);
		
		// UIElement virtual function
		virtual void Init();  
		virtual void Draw(float _deltaTime) override;	
		virtual void Update(float _deltaTime) override;
		virtual void SetUIProperties(UI_Properties _prop);
        virtual void SetSize(Vector2 _size);		
		virtual Rectangle GetBounds();

		virtual void SetScale(float _scale) override;
		virtual void SetActive(bool _flag) override;
		virtual void SetLocation(Vector2 _location)override;		
		virtual void Offset(Vector2 _location)override;
		virtual void DrawBGColor();
		virtual void DrawBGTexture();
		virtual void SetVisible(bool _flag) override;
		virtual ~UIElement();

		void SetTexture(std::string& _texture);
		void AddText(const std::string & id, Text_Properties _txt);
		weak<UIButton> AddButton(const std::string& id, Btn_Text_Properties _btn);
		void AddChild(weak<UIElement> _btn);
		bool IsPendingUpdate() { return m_pendingUpdate; };
		void SetPendingUpdate(bool _flag);
		void SetFlipH(bool fliph);
		void SetFlipV(bool flipv);
		bool GetFlipH() { return m_flipH; };
		bool GetFlipV() { return m_flipV; };
		void MaintainAspectRatio(bool _flag) { m_bAspectRatio = _flag; };
		bool IsAspectRatio() { return m_bAspectRatio; };
		Vector2 GetPivot() { return m_pivot; };
		void Destroy()override;
		void ClearTexture();
	protected:
		Vector2 m_rawlocation;	
		Vector2 m_pivot;
		std::string m_strTexture;
		bool m_bAspectRatio;
		bool m_pendingUpdate;
		std::vector<shared<UIElement>> m_children = {};
		std::vector <shared<UIButton>> m_slides = {};
		virtual void UpdateLocation();		
		bool m_flipH;
		bool m_flipV;
		float m_TextureScale;
		shared<Texture2D> m_texture2d;
	};

}