#include "UICanvas.h"
#include <algorithm>
#include <cctype>
#include "Logger.h"
#include "Application.h"
#include "World.h"
#include "component/LayoutComponent.h"


extern int DEFAULT_CANVAS_WIDTH;
extern int DEFAULT_CANVAS_HEIGHT;
extern int SCREEN_WIDTH;
extern int SCREEN_HEIGHT;
extern float CANVAS_STRECH_X;
extern float CANVAS_STRECH_Y;
namespace cart {
    shared<UICanvas> UICanvas::canvas_{ nullptr };

    // Private constructor to prevent direct instantiation
    UICanvas::UICanvas() :m_safeRect{}{
    }

    weak<UICanvas> UICanvas::Get() {
        if (!canvas_) {
            canvas_ = shared<UICanvas>{ new UICanvas };
        }
        return canvas_;
    }

    bool UICanvas::RegisterComponent(weak<LayoutComponent> comp)// register to update layout when device size/res changes
    {
      //  std::string d = ui.lock()->GetID();
      //  std::transform(d.begin(), d.end(), d.begin(),
      //      [](unsigned char c) { return std::tolower(c); }); // to Lowercase
        auto found = elemlist.find(comp.lock()->GetId());
        if (found != elemlist.end())// check if key exists
        {
            Logger::Get()->Error(std::format("UICanvas::AddElement() element with id name is already exists!"));
            return false;
        }
        elemlist.insert({comp.lock()->GetId(), comp });
        return true;
    }

    bool UICanvas::RemoveComponent(const std::string& id)// remove component from list
    {
        auto found = elemlist.find(id);
        if (found != elemlist.end())// check if key exists
        {
            if (!found->second.expired())
            {
                found->second.reset();
            }
            elemlist.erase(found);   
            return true;
        }
        return false;
    }

    void UICanvas::UpdateLayout()// explicit ca
    {     
        for (auto iter = elemlist.begin(); iter != elemlist.end();)
        {
            if (!iter->second.expired())
            {
                iter->second.lock()->SetForUpdate();
                ++iter;

            }
        }
        float scaleX = (float)SCREEN_WIDTH / (float)DEFAULT_CANVAS_WIDTH;
        float scaleY = (float)SCREEN_HEIGHT / (float)DEFAULT_CANVAS_HEIGHT;
        float scale = std::min(scaleX, scaleY);
        
        if (scaleX <= scaleY)scaleY = std::max(scale, CANVAS_STRECH_Y);
        if (scaleX > scaleY)scaleX = std::max(scale, CANVAS_STRECH_X);

       /* float scaleX = std::max(CANVAS_STRECH_X, (float)SCREEN_WIDTH / (float)DEFAULT_CANVAS_WIDTH);
        float scaleY = std::max(CANVAS_STRECH_Y, (float)SCREEN_HEIGHT / (float)DEFAULT_CANVAS_HEIGHT);*/

        for (auto iter = elemlist.begin(); iter != elemlist.end();)
        {         
            if (iter->second.expired())
            {
                iter = elemlist.erase(iter);
            }
            else {
                if (iter->second.lock()->IsOwnerReady() && !iter->second.lock()->IsUpdated())
                {

                    iter->second.lock()->UpdateLayout({ (float)SCREEN_WIDTH, (float)SCREEN_HEIGHT },
                        scaleX, scaleY, m_safeRect);
                  
                }
                ++iter;
            }
        }
         
        onScreenSizeChange.Broadcast();
    }

    const Vector2 UICanvas::Size()
    {
        return { (float)SCREEN_WIDTH, (float)SCREEN_HEIGHT };
    }

    void UICanvas::SafeRect(Rectangle rect)
    {
        m_safeRect = rect;
    }

    float UICanvas::Scale()
    {
        return std::min((float)SCREEN_WIDTH / (float)DEFAULT_CANVAS_WIDTH,
            (float)SCREEN_HEIGHT / (float)DEFAULT_CANVAS_HEIGHT);
    }
    Vector2 UICanvas::GetDefaultCanvasSize() {
        return { (float)DEFAULT_CANVAS_WIDTH, (float)DEFAULT_CANVAS_HEIGHT };
    }
    UICanvas::~UICanvas() {
    }
}