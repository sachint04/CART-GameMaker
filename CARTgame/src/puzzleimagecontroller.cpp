#include "puzzleimagecontroller.h"
#include <raylib.h>
#include "Types.h"
#include "GameStage.h"
#include "UIButton.h"
#include "Sprite2D.h"
#include "World.h"
#include "component/controls/transformCntrl.h"
#include "Logger.h"
#include "AssetManager.h"

namespace cart{

    PuzzleImageController::PuzzleImageController(World* owningworld, const std::string &id, const std::string& strimage, int defaultWidth, int defaultHeight, int minWidth, int minHeight, int maxWidth, int maxHeight, int gridWidth, int gridHeight)
    :UIElement(owningworld, id),
    m_strImage{strimage},
    m_defaultWidth{defaultWidth},
    m_defaultHeight{defaultHeight},
    m_minWidth{minWidth},
    m_minHeight{minHeight},
    m_maxWidth{maxWidth},
    m_maxHeight{maxHeight},
    m_gridWidth{gridWidth},
    m_gridHeight{gridHeight},
        cannontex{}
    {
    }

    PuzzleImageController::~PuzzleImageController()
    {
    }

   

#pragma region  INITIALIZATION
void PuzzleImageController::Init()
{
   
    uint16_t scrW = GetScreenWidth();
    uint16_t scrH = GetScreenHeight();

    float rectsize = 400.f;
    Rectangle bgrect = { (scrW - rectsize) / 2.f, (scrH - rectsize) / 2.f, rectsize,rectsize };
   // Rectangle bgrect = { scrW / 2.f - 250.f, scrH / 2 - 200.f,  rectsize, rectsize };


    float xpos = scrW / 2;// -twidth;
    float ypos = scrH / 2;// -12;

    UI_Properties ui = {};
    ui.size = { rectsize, rectsize };
    ui.location = { xpos, ypos};
    ui.pivot = { rectsize/2.f, rectsize /2.f};
    ui.texture = m_strImage;
    ui.color = LIGHTGRAY;
    
    

    m_target = m_owningworld->SpawnActor<Sprite2D>(std::string{"imgeditor"});
    m_target.lock()->SetUIProperties(ui);
    m_target.lock()->MaintainAspectRatio(true);
    m_target.lock()->Init();
    m_target.lock()->SetVisible(true);
    AddChild(m_target);

   std::string cntrlid  = "pcntrl";

   m_control = m_owningworld->SpawnActor<TransformCntrl>(cntrlid, Vector2{ (float)m_minWidth, (float)m_minHeight }, Vector2{ (float)m_maxWidth, (float)m_maxHeight } , m_target.lock()->GetBounds());
  //  m_control.lock()->SetLocation(ui.location);
    m_control.lock()->SetVisible(true);
    m_control.lock()->Init();
    m_control.lock()->onScaled.BindAction(GetWeakRef(), &PuzzleImageController::ImageScaleHandler);
    m_control.lock()->onMoved.BindAction(GetWeakRef(), &PuzzleImageController::ImageMoveHandler);
    AddChild(m_control);

    Btn_Text_Properties txtbtnui = {};

    txtbtnui.btncol = GRAY;
    txtbtnui.overcol = ORANGE;
    txtbtnui.downcol = WHITE;
    txtbtnui.size = { 100.f , 30.f };
    txtbtnui.textcolor = WHITE;
    txtbtnui.text = std::string{ "Reset" };
    txtbtnui.fontsize = 16;
    txtbtnui.fontspace = 1.8;
    txtbtnui.font = FONT_NAME;
    txtbtnui.location = { bgrect.x + bgrect.width - 110.f, bgrect.y - 50.f };
    txtbtnui.textAlign = ALIGN::CENTER;
    txtbtnui.pivot = { 0, 0 };

    
    m_resetBtn = m_owningworld->SpawnActor<UIButton>(std::string{"btncontinue"});
    m_resetBtn.lock()->SetButtonProperties(txtbtnui);
    m_resetBtn.lock()->Init();
    m_resetBtn.lock()->SetVisible(true);
    m_resetBtn.lock()->onButtonUp.BindAction(GetWeakRef(), &PuzzleImageController::ResetHandler);
    AddChild(m_resetBtn);
    
    txtbtnui.location = {bgrect.x + 10.f, bgrect.y - 50.f };
    txtbtnui.text = "Go Back";
    m_goBackBtn = m_owningworld->SpawnActor<UIButton>(std::string{"btngoback"});
    m_goBackBtn.lock()->SetButtonProperties(txtbtnui);
    m_goBackBtn.lock()->Init();
    //m_goBackBtn.lock()->SetVisible(true);
    m_goBackBtn.lock()->onButtonUp.BindAction(GetWeakRef(), &PuzzleImageController::GoBackHandler);
    AddChild(m_goBackBtn);

    
    // TEST EM_Fetch class

// Will Work in __EMSCRIPTEN__ Mode
   bool success =  AssetManager::Get().LoadTextureAsync(std::string{ "/cannon1.png" }, GetWeakRef(), &PuzzleImageController::OnFetchAsyncLoadTexture, TEXTURE_DATA_STATUS::LOCKED);
   

   txtbtnui = {};

   ui = {};

//#endif //  __EMSCRIPTEN__
}



void PuzzleImageController::ResetHandler(weak<Object> btn, Vector2 pos)
{
    m_control.lock()->Reset();
}
void PuzzleImageController::GoBackHandler(weak<Object> btn, Vector2 pos)
{
    onGoBack.Broadcast();
}
void PuzzleImageController::ImageScaleHandler(Vector2 loc, Vector2 size, Vector2 pivot)
{
    m_target.lock()->SetSize(size);
    m_target.lock()->SetPivot(pivot);
    m_target.lock()->SetLocation(loc);
}
void PuzzleImageController::ImageMoveHandler(Vector2 loc)
{
    m_target.lock()->SetLocation(loc);
}
#pragma endregion

#pragma region TEST EM_Fetch class
void PuzzleImageController::OnFetchDataLoad(std::string id, std::string status, const char* data, int size)
{
   
    if (status.compare("ok") == 0) {      
        unsigned char* modifiable = reinterpret_cast<unsigned char*>(const_cast<char*>(data));            
        Image cannon = LoadImageFromMemory(".png", (unsigned char*)modifiable, size);
        ImageResize(&cannon, 400, 400);
        Logger::Get()->Push(std::format("Image width {} | height {} size {}", cannon.width, cannon.height, size));
        cannontex = LoadTextureFromImage(cannon);
        UnloadImage(cannon);
    }
}

void PuzzleImageController::OnFetchAsyncLoadTexture(std::string url, ASYNC_CALLBACK_STATUS status, shared<Texture2D> texture, float progress)
{
    if (status == FAILED) {
        Logger::Get()->Push(std::format("ERROR!! , Failed to Load Texture from {}", url));
        return;
    }
    else if (status == PROGRESS)
    {
        Logger::Get()->Push(std::format("OnFetchAsyncLoadTexture()  progress {}",  progress));    
            return;
    }
    Logger::Get()->Push(std::format("OnFetchAsyncLoadTexture()  sccusses {}", url));

    UI_Properties  ui = {};
    ui.color = WHITE;
    ui.texture = std::string{ "/cannon.png" };
    ui.location = { 300.f, 0 };
    ui.size = { 300.f, 300.f };
    m_cannon = m_owningworld->SpawnActor<Sprite2D>(std::string{ "cannon" });
    m_cannon.lock()->SetUIProperties(ui);
    m_cannon.lock()->MaintainAspectRatio(true);
    m_cannon.lock()->Init();
    m_cannon.lock()->SetVisible(true);
    std::string path = { "/cannon.png" };
    AssetManager::Get().ResizeTexture(path, 300, 300);
    AddChild(m_cannon);
  
}

#pragma endregion

#pragma region CLEANUP
void PuzzleImageController::Destroy() {

    UIElement::Destroy();
}
void PuzzleImageController::Draw(float _deltaTime)
{
    UIElement::Draw(_deltaTime);

}
#pragma endregion

}