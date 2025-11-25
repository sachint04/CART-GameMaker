#pragma once
#include <raylib.h>
#include <vector>
#include <string>
#include "Core.h"
#include "Types.h"
namespace cart {

    class LayoutComponent;
	class UICanvas {
	public:           
        static  weak<UICanvas> Get();
        bool RegisterComponent(weak<LayoutComponent> comp, Vec2_short& offset, Vec2_short& size);
        bool RemoveComponent(const std::string& id);
        void UpdateLayout();       
        const Vector2 Size();
        void SafeRect(Rectangle rect);
        const Rectangle SafeRect()const { return m_safeRect; };
        float Scale();
        ~UICanvas();
	private:
        UICanvas();
        // Private static pointer to the single instance
        static shared<UICanvas> canvas_;
        Dictionary<std::string, weak<LayoutComponent>> elemlist;
        Rectangle m_safeRect;
	};

}