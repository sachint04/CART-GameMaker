#pragma once
#include "UIElement.h"
#include "Core.h"
#include "network/EM_Fetch.h"
namespace cart
{
    class UIButton;
    class Sprite2D;
    class World;
    class TransformCntrl;
    class PatternSprite;
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
    weak<PatternSprite> m_bgPatt; 
    weak<Sprite2D> m_pattern;
    weak<Sprite2D> m_target; 
    weak<Sprite2D> m_cannon; 
    weak<TransformCntrl> m_control;
    weak<UIButton> m_resetBtn;
    weak<UIButton> m_goBackBtn;
    void OnFetchDataLoad(std::string id, std::string status, const char* data, int size);
    void OnFetchAsyncLoadTexture(std::string url, ASYNC_CALLBACK_STATUS status, shared<Texture2D> texture, float progress);
    Texture2D cannontex;
    public:
        PuzzleImageController(World* _owningworld, const std::string& id, const std::string& strimage, int defaultWidth, int defaultHeight, int minWidth, int minHeight, int maxWidth, int maxHeight, int gridWidth, int gridHeight);
       void Init()override;
       void Destroy() override;
       void Draw(float _deltaTime)override;
       Delegate<> onGoBack;

       virtual ~PuzzleImageController();

        
    };
    


}