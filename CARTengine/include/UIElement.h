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
		virtual void DrawBGColor();
		virtual void DrawBGTexture();
		virtual void SetVisible(bool _flag) override;
		virtual ~UIElement();

		void SetTexture(std::string& _texture);
		void AddText(weak<UIElement> _txt);
		void AddText(const std::string & id, Text_Properties _txt);
		void AddButton(weak<UIElement> _btn);
		void AddUIElement(weak<UIElement> _ui);
		bool IsPendingUpdate() { return m_pendingUpdate; };
		void SetPendingUpdate(bool _flag);
	protected:
		Vector2 m_rawlocation;
		shared<Texture2D> m_texture2d;
		Vector2 m_size;
		Vector2 m_pivot;
		std::string m_texture;
		bool m_pendingUpdate;
		std::vector<weak<UIElement>> m_children = {};
		std::vector <weak<UIButton>> m_slides = {};
		virtual void UpdateLocation();
		

	};

}