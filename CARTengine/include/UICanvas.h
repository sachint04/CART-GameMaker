#pragma once
#include <raylib.h>
#include <vector>
#include <string>
#include "Core.h"
#include "Types.h"
#include "Delegate.h"
#include "UIElement.h"
namespace cart {

    class IComponent;
	class UICanvas : public UIElement {
	public:           
        virtual void Init() override;
        virtual void Start() override;
        virtual void Update(float _deltaTime) override;
        virtual void Draw(float _deltaTime) override;
        virtual Rectangle GetBounds()override;

        bool RegisterComponent(weak<IComponent> comp);
        bool RemoveComponent(const std::string& id);
        void UpdateLayout();       
        void SafeRect(Rectangle rect);
        const Rectangle SafeRect()const { return m_safeRect; };
        const Vector2 Size();
        float StrechX();
        float StrechY();
        float Scale();
        float ScaleX();
        float ScaleY();

        Vector2 GetDefaultCanvasSize();
        Delegate<> onScreenSizeChange;

        UICanvas(World* _owningworld, const std::string& _id);
        ~UICanvas();
	private:

        Dictionary<std::string, weak<IComponent>> elemlist;
        Rectangle m_safeRect;
	};

}