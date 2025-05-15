#pragma once
#include "UIElement.h"
#include "World.h"
#include "component/controls/transformCntrl.h"
#include "Core.h"
namespace cart
{
    class UIButton;
    class PuzzleImageController: public UIElement
    {
    private:
       void ResetHandler(weak<Object> btn, Vector2 pos);
       void GoBackHandler(weak<Object> btn, Vector2 pos);
       void ImageScaleHandler(Vector2 loc, Vector2 size, Vector2 pivot);
       void ImageMoveHandler(Vector2 loc);


    int m_defaultWidth;
    int m_defaultHeight;
    int m_minWidth;
    int m_minHeight;
    int m_maxWidth;
    int m_maxHeight;
    int m_gridWidth;
    int m_gridHeight;
    std::string m_strImage;
    weak<UIElement> m_target; 
    weak<TransformCntrl> m_control;
    weak<UIButton> m_resetBtn;
    weak<UIButton> m_goBackBtn;

    public:
        PuzzleImageController(World* _owningworld, const std::string& id, const std::string& strimage, int defaultWidth, int defaultHeight, int minWidth, int minHeight, int maxWidth, int maxHeight, int gridWidth, int gridHeight);
       void Init()override;
       void Destroy() override;

       Delegate<> onGoBack;

       virtual ~PuzzleImageController();

        
    };
    


}