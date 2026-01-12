#include "TextInput.h"
#include <stdexcept>
#include "AssetManager.h"
#include "Clock.h"
#include "MathUtility.h"
#include "Application.h"
#include "World.h"
#include "CARTjson.h"
#include "component/InputController.h"
#include "CARTjson.h"
#include "UICanvas.h"


namespace cart
{
#pragma region Construction & Initialization

    TextInput::TextInput(World* _owningworld, const std::string& _id)
        :Text{ _owningworld, _id },
        m_charLimit{ 300 },
        m_letterCount{ 0 },
        m_mouseOnText{ false },
        m_framesCounter{ 0 },
        //    lines{  },
        m_isBackspace{ false },
        m_keyWaitTimer{ 0 },
        m_backspacekeyWaitTimer{ 0 },
        m_textmargin{ 10.f },
        m_touch{ false },
        m_touchstartpos{},
        m_touchendpos{},
        m_lines{},
        m_curletterindex{ 0 },
        // m_lrange{},
        m_isLeftKey{ false },
        m_isRightKey{ false },
        m_isDeleteKey{ false },
        m_key{ 0 },
        m_hasUpated{ false },
        m_cursorLoc{ 0 },
        m_keydownWaitTimeMultiplyer{ 0.5f },
        m_keydownMulitiplyerDuration{ 1.f },
        m_keydownActionDuration{ 1.f },
        m_tempkeydownActionDuration{ 1.f },
        m_keydownActionMinDuration{ 0.015f },
        m_bMobileInput{ false },
        m_mobileinput{""}
	{
       /* Text::m_fontsize = 14.f;
        Text::m_fontspacing = 2.f;*/
	}
    void TextInput::Init()
    {
        Text::Init();
        m_owningworld->GetInputController()->RegisterUI(GetWeakRef());
       
       
    }
    void TextInput::Start()
    {
       
       
        Text::Start();
    }


#pragma endregion

#pragma region Clean Up


	TextInput::~TextInput()
	{
        m_infofnt.reset();
	}
    void TextInput::Destroy() {
        if (m_isPendingDestroy)return;
        m_owningworld->GetInputController()->RemoveUI(GetId());
        Logger::Get()->Trace("TextInput::Destroy()! ");
        if (m_isFocused)// hide mobile keyboard if open.
        {
            m_owningworld->GetApplication()->RemoveMobileInputListener(GetId());
            m_owningworld->GetApplication()->MobileKeyboardInterupt();
        }
        Text::Destroy();
    }

    
   
#pragma endregion

#pragma region  LOOP

	void TextInput::Update(float _deltaTime)
	{
        if (!m_visible)return;
        Text::Update(_deltaTime);
        Rectangle textBox = GetBounds();
        float scrnScale =  World::UI_CANVAS.get()->Scale();
        
        auto typeinbetween = [&](int& c, int key, char* s,  std::vector<std::pair<std::string, Vector2>>& l, int& d)
        {
            if (c + 1 >= MAX_INPUT_CHARS)return;
      
            size_t i = 0;
            for (i = c; i > d ; i--)
            {
                s[i] = s[i - 1];
            }
            s[d] = (char)key;
            c++;
            d++;
            s[c] = '\0';

            l.clear();
            l.push_back({ std::string{},  { (float)textBox.x + m_textmargin * scrnScale, (float)textBox.y + 8 } });

            float fsize = std::max(m_minfontsize, std::ceil(m_fontsize * World::UI_CANVAS.get()->Scale()));
            float fspace = std::max(m_minfontspacing, m_fontspacing * World::UI_CANVAS.get()->Scale());
            Vector2 fntmeasure = MeasureTextEx(*m_sharedfont, "W", fsize, fspace);
            for (int i = 0; i < c; i++)
            {

                l[l.size() - 1].first.append(std::string{ s[i] });
                if (MeasureTextEx(*m_sharedfont, l[l.size() - 1].first.c_str(), fsize,fspace).x >= textBox.width - (fntmeasure.x + m_textmargin * scrnScale))
                {
                    l.push_back({ std::string{},  { (float)textBox.x + m_textmargin * scrnScale, ((float)textBox.y + 8) + (fntmeasure.y * l.size() - 1) } });
                 
                }
            }
        };
        
        auto whilebackspace = [&](int &c, char * s, std::vector<std::pair<std::string, Vector2>>& l, int &d)
        {     
            if (d - 1 < 0)return;
            size_t i= 0;
            for (i = d - 1; i < c ; i++)
            {
                s[i] = s[i + 1];               
            }
            c--;
            d--;
            s[c] = '\0';
            l.clear();
            l.push_back({ std::string{},  { (float)textBox.x + m_textmargin * scrnScale, (float)textBox.y + 8 } });

            float fsize = std::max(m_minfontsize, std::ceil(m_fontsize * World::UI_CANVAS.get()->Scale()));
            float fspace = std::max(m_minfontspacing, m_fontspacing * World::UI_CANVAS.get()->Scale());
            Vector2 fntmeasure = MeasureTextEx(*m_sharedfont, "W", fsize, fspace);
            for (int i = 0; i < c; i++)
            {
 
                l[l.size() - 1].first.append(std::string{ s[i] });
                if (MeasureTextEx(*m_sharedfont, l[l.size() - 1].first.c_str(), fsize, fspace).x >= textBox.width - (fntmeasure.x + m_textmargin * scrnScale))
                {
                    l.push_back({ std::string{},  { (float)textBox.x + m_textmargin * scrnScale, ((float)textBox.y + 8) + (fntmeasure.y * m_lines.size() - 1) } });
                }
            }
           // r[r.size() - 1].first.second = c;
        };

        auto whileleftkey = [&](int& d) {
            d--;
            if (d < 0)d = 0;
        };
        
        auto whilerightkey = [&](int& d, const int & c) {
            d++;
            if (d > c)d = c;
        };

        auto whiledeletekey = [&](int& c, char* s, std::vector<std::pair<std::string, Vector2>>& l, int& d)
        {
            if (d == c)return;
           
            size_t i = 0;
            for (i = d ; i < c; i++)
            {
                s[i] = s[i + 1];
            }                        
            c--;            
            s[c] = '\0';

            l.clear();
            l.push_back({ std::string{},  { (float)textBox.x + m_textmargin * scrnScale, (float)textBox.y + 8 } });

            float fsize = std::max(m_minfontsize, std::ceil(m_fontsize * World::UI_CANVAS.get()->Scale()));
            float fspace = std::max(m_minfontspacing, m_fontspacing * World::UI_CANVAS.get()->Scale());
            Vector2 fntmeasure = MeasureTextEx(*m_sharedfont, "W", fsize, fspace);
            for (int i = 0; i < c; i++)
            {

                l[l.size() - 1].first.append(std::string{ s[i] });
                if (MeasureTextEx(*m_sharedfont, l[l.size() - 1].first.c_str(), fsize, fspace).x >= textBox.width - (fntmeasure.x + m_textmargin * scrnScale))
                {
                    l.push_back({ std::string{},  { (float)textBox.x + m_textmargin * scrnScale, ((float)textBox.y + 8) + (fntmeasure.y * l.size() - 1) } });
                
                }
            }
        };
        
        
      /*  if (CheckCollisionPointRec(GetMousePosition(), GetBounds())) m_mouseOnText = true;
        else m_mouseOnText = false;*/

        int tCount = GetTouchPointCount();
       
        m_mouseOnText = m_owningworld->GetInputController()->IsMouseOver(GetWeakRef());
        // if Input is currently Selected
        if (m_mouseOnText || m_hasUpated)
        {

#if defined(PLATFORM_ANDROID)
            tCount = GetTouchPointCount();
            /*
            Vector2 tData[10] = {};
            if (tCount > 0) {
                if (tCount > 10) tCount = 10;

                // Get touch points positions
                for (int i = 0; i < tCount; ++i) {
                    tData[i] = GetTouchPosition(i);// MULTI TOUCH
                }
            }
            */


            if (tCount > 0) {
                m_locmouse = GetTouchPosition(0);
                if (m_touch == false) {// NO TOUCH & NO CARD PICKED
                    if (TestMouseOver(m_locmouse) == true) {
                        ButtonDown();
                    }
                }
                m_touch = true;
            }
            else {

                if (m_touch == true) {
                    if (m_mouseOnText) {
                        ButtonUp();
                    }
                }
                m_touch = false;
            }

#endif  
            Vector2 tPos = { (float)GetMouseX(), (float)GetMouseY() };            
            if (IsMouseButtonPressed(0) || tCount > 0)
            {
                if (m_touch == true) {// focus is already on the input hence ignore
                    return;
                }
                m_owningworld->GetInputController()->SetFocus(GetId());

                m_touchstartpos = tPos;
                m_touch = true;
            }
            if (IsMouseButtonReleased(0))
            {
                if (m_touch == false) {
                    m_owningworld->GetInputController()->SetFocus(std::string{ "" });
                    return;
                }
                //  if (m_lines.size() == 0 || m_lines[0].first.length() == 0) return;
                
                m_touchendpos = tPos;
                // set current selected
                if (GetVectorLength(Direction(m_touchendpos, m_touchstartpos)) < 5) {
                    m_curletterindex = m_letterCount;
                    Vector2 fm = MeasureTextEx(*m_sharedfont, "W", (float)m_fontsize * scrnScale, m_fontspacing * scrnScale);
                    int tmpLtrCount = 0;
                    for (auto iter = m_lines.begin(); iter != m_lines.end(); ++iter) {
                        Rectangle b = GetBounds();
                        if (m_touchendpos.y > iter->second.y && m_touchendpos.y <= iter->second.y + fm.y)
                        {
                            int chr = 0;
                            while (chr < iter->first.length() - 1)
                            {
                                std::string nr = iter->first.substr(0, chr);
                                if (iter->second.x + MeasureTextEx(*m_sharedfont, nr.c_str(), (float)m_fontsize * scrnScale, m_fontspacing * scrnScale).x >= m_touchendpos.x)
                                {
                                    m_curletterindex = tmpLtrCount + nr.size();

                                    break;
                                }
                                chr++;
                            }
                        }
                        tmpLtrCount += iter->first.size();
                    }
                }
                m_touch = false;
            }
            else {
                m_isBackspace = false;
                SetMouseCursor(MOUSE_CURSOR_DEFAULT);
            }
        }

      
            if (IsKeyUp(KEY_BACKSPACE)) {
                m_tempkeydownActionDuration = m_keydownMulitiplyerDuration;
                m_isBackspace = false;
            }
            if (IsKeyUp(KEY_LEFT))
            {
                m_isLeftKey = false;
            }    
            if (IsKeyPressed(KEY_BACKSPACE))
            {
                if (!m_isBackspace) {
                    m_backspacekeyWaitTimer = Clock::Get().ElapsedTime();
                    m_keyWaitTimer = Clock::Get().ElapsedTime();
                    whilebackspace(m_letterCount, name, m_lines, m_curletterindex);
                    m_text =  name ;
                    PrepareInput(textBox);
                    m_isBackspace = true;
                }

            }
            if (IsKeyPressed(KEY_LEFT)) {
                m_keyWaitTimer = Clock::Get().ElapsedTime();
                m_isLeftKey = true;

            }
            if (IsKeyPressed(KEY_RIGHT)) {
                m_keyWaitTimer = Clock::Get().ElapsedTime();
                m_isRightKey = true;
            }
            if (IsKeyPressed(KEY_DELETE))
            {
                whiledeletekey(m_letterCount, name, m_lines, m_curletterindex);
               
                //  m_keyWaitTimer = Clock::Get().ElapsedTime();
                //  m_isDeleteKey = true;
            }
            // Get char pressed (unicode character) on the queue
            int key = GetCharPressed();
            // Check if more characters have been pressed on the same frame
            while (key > 0 )
            {
                float fsize = std::max(m_minfontsize, std::ceil(m_fontsize * World::UI_CANVAS.get()->Scale()));
                float fspace = std::max(m_minfontspacing, m_fontspacing * World::UI_CANVAS.get()->Scale());

                int  txtmargin = m_textmargin * scrnScale;
                int tx = textBox.x + 8;
                int ty = textBox.y + 8;
                // NOTE: Only allow keys in range [32..125]
                if ((key >= 32) && (key <= 125) && (m_letterCount < m_charLimit))
                {
                    Vector2 fntmeasure = MeasureTextEx(*m_sharedfont, "W", fsize, fspace);
                    Rectangle textBox = GetBounds();

                    if (m_curletterindex < m_letterCount)
                    {
                        typeinbetween(m_letterCount, key, name, m_lines, m_curletterindex);
                    }
                    else {
                        name[m_letterCount] = (char)key;
                        name[m_letterCount + 1] = '\0'; // Add null terminator at the end of the string.                 
                        m_curletterindex = m_letterCount + 1;
                    }

                    m_text =  name ;
                    m_hasUpated  = true;
                   
                    m_isBackspace = false;
                    m_isLeftKey = false;
                    m_isRightKey = false;
                    m_isDeleteKey = false;
                    m_letterCount++;
                }
                key = GetCharPressed();  // Check next character in the queue

            }
            // Set the window's cursor to the I-Beam
            if(m_isFocused && m_mouseOnText)
            SetMouseCursor(MOUSE_CURSOR_IBEAM);
      
        
        if (m_isBackspace) {
            double t = Clock::Get().ElapsedTime();
            if (t - m_keyWaitTimer > m_keydownMulitiplyerDuration) {
                m_tempkeydownActionDuration = std::max(m_keydownActionMinDuration, m_tempkeydownActionDuration * m_keydownWaitTimeMultiplyer);
                // Logger::Get()->Trace(std::format("backspace time {} | {}", m_tempkeydownActionDuration, m_keydownMulitiplyerDuration));
                m_keyWaitTimer = t;
            }
            //float keydownActionMulitiplyer = m_keydownMulitiplyerDuration;
            if (t - m_backspacekeyWaitTimer >= m_tempkeydownActionDuration)
            {
                if (m_letterCount >= 0) {
                    whilebackspace(m_letterCount, name, m_lines, m_curletterindex);
                    m_text = name;
                    m_hasUpated = true;
                }
                m_backspacekeyWaitTimer = t;
            }
        }
        if (m_isLeftKey)
        {
            double t = Clock::Get().ElapsedTime();
            if (t - m_keyWaitTimer >= 0.1f)
            {
                whileleftkey(m_curletterindex);
                m_keyWaitTimer = t;
            }

        }
        if (m_isRightKey)
        {
            double t = Clock::Get().ElapsedTime();
            if (t - m_keyWaitTimer >= 0.1f)
            {
                whilerightkey(m_curletterindex, m_letterCount);
                m_keyWaitTimer = t;
            }
            if (IsKeyUp(KEY_RIGHT))
                m_isRightKey = false;
        }
        if (m_isDeleteKey)
        {
            double t = Clock::Get().ElapsedTime();
            if (t - m_keyWaitTimer >= 0.1f)
            {
                whiledeletekey(m_letterCount, name, m_lines, m_curletterindex);
                m_text = name;
                m_keyWaitTimer = t;
                m_hasUpated = true;
            }
            if (IsKeyUp(KEY_DELETE))
                m_isDeleteKey = false;
        }

        if (m_hasUpated) {
            PrepareInput(textBox);            
        }
        if (m_owningworld->GetInputController()->HasFocus() &&
            m_owningworld->GetInputController()->GetFocusedId().compare(GetId()) == 0) m_framesCounter++;
        else m_framesCounter = 0;
	}

    void TextInput::Draw(float _deltaTime)
    {
        if (!m_visible)return;
        UIElement::Draw(_deltaTime);
        Rectangle textBox = GetBounds();
        float scrnScale =  World::UI_CANVAS.get()->Scale();

        // DrawRectangleRec(textBox, LIGHTGRAY);
        if (m_owningworld->GetInputController()->GetFocusedId().compare(GetId()) == 0)
        {            
           DrawRectangleLinesEx({ textBox.x, textBox.y, textBox.width, textBox.height }, 2.f, DARKGRAY);
        }
        //else {
        //    DrawRectangleLines((int)textBox.x, (int)textBox.y, (int)textBox.width, (int)textBox.height, DARKGRAY);
        //}
             
       // DrawTextEx(*infofnt, TextFormat("INPUT CHARS: %i/%i", m_letterCount, MAX_INPUT_CHARS), { (float)textBox.x , (float)textBox.y + textBox.height }, 14, 1.f, DARKGRAY);


        if (m_isFocused)       
            DrawInputCursor(textBox);
        
        TextLine();

        if (m_letterCount >= m_charLimit)
            ShowCharLimitWarning(textBox);
        else
            ShowRemainingCharCount(textBox);

        m_hasUpated = false;
	}
#pragma endregion

#pragma region  Helper
    void TextInput::SetText(const std::string& txt)
    {
      //  Logger::Get()->Trace(std::format("TextInput::SetText() txt {} ", txt));
        m_text = txt;
        for (size_t i = 0; i < m_letterCount; i++)
        {
            name[i] = i < (char)m_text.size()? m_text.at(i) : '\0';
        }
        m_curletterindex = m_text.size();
        m_hasUpated = true;
    }
    void TextInput::SetFontName(const std::string& strfnt)
    {
        Text::SetFontName(strfnt);
        m_hasUpated = true;
    }
    void TextInput::SetFontSize(float size)
    {
        Text::SetFontSize(size);
        m_hasUpated = true;
     
    }
    void TextInput::SetTextProperties(Text_Properties _props)
    {
        Text::SetTextProperties(_props);
        SetText(_props.text);
    }
    void TextInput::SetFocused(bool _flag)
    {
        Logger::Get()->Trace(std::format("TextInput::SetFocused()! default text {}", m_text));
       int useragent = CARTjson::GetEnvSettings()["useragent"];
       if (useragent > 0) {// if useragent is > 0 (mobile browser) show os keyboard
            if (_flag) {// currently is in focus
                std::string curtxt = name;
               m_owningworld->GetApplication()->RegisterListernerToMobileInput(GetId(), GetWeakRef(), &TextInput::OnMobileInput);
                m_owningworld->GetApplication()->ToggleMobileWebKeyboard(m_text, KeyboardType::Default, false, true, false, false, m_text.size() == 0 ? std::string{ "type here .." } : std::string{""}, m_charLimit);
            }else
            {
                m_owningworld->GetApplication()->RemoveMobileInputListener(GetId());
                m_owningworld->GetApplication()->MobileKeyboardInterupt();
            }
        }
        UIElement::SetFocused(_flag);
    }
    void TextInput::SetVisible(bool _flag)
    {
        Text::SetVisible(_flag);
        Logger::Get()->Trace("TextInput::SetVisible()! ");
        if (!_flag && m_isFocused)// hide mobile keyboard if open.
        {
            m_owningworld->GetApplication()->RemoveMobileInputListener(GetId());
            m_owningworld->GetApplication()->MobileKeyboardInterupt();
        }
    }
    std::string TextInput::GetInputText()
    {
        auto iter = m_lines.begin();
        std::string message = "";
        while (iter != m_lines.end())
        {
            if (iter->first.size() > 0)
            {
                message += {iter->first};
            }
            ++iter;
        }
        return message;
    } 
    void TextInput::UpdateLayout()
    {
        SetText(m_text);
        
    }

    /// <summary>
    /// TextInput::TextLine - Draw text on screen
    /// </summary>
    /// <param name="str"></param>
    void TextInput::TextLine()
    {
        auto iter = m_lines.begin();
        Rectangle textBox = GetBounds();
        float scrnScale = World::UI_CANVAS.get()->Scale();
        float fsize = std::max(m_minfontsize, std::ceil(m_fontsize * World::UI_CANVAS.get()->Scale()));
        float fspace = std::max(m_minfontspacing, m_fontspacing * World::UI_CANVAS.get()->Scale());

        int count = 0, l = 0;
        int txtmargin = m_textmargin * scrnScale;

        m_sharedfont = AssetManager::Get().LoadFontAsset(m_font, fsize);
        
        while (iter != m_lines.end())
        {
            if (iter->first.size() > 0)
            {
                DrawTextEx(*m_sharedfont, iter->first.c_str(), iter->second, fsize, fspace, m_textColor);

                count += iter->first.size();               
            }
            l++;
            ++iter;
        }

    }

    /// <summary>
    /// Prepare array of string line  and  its position relative to text box
    /// </summary>
    /// 
    void TextInput::PrepareInput(Rectangle bounds)
    {
        float scrnScale =  World::UI_CANVAS.get()->Scale();
        float fsize = std::max(m_minfontsize, std::ceil(m_fontsize * World::UI_CANVAS.get()->Scale()));
        float fspace = std::max(m_minfontspacing, m_fontspacing * World::UI_CANVAS.get()->Scale());
        float fmargin = m_textmargin * scrnScale;

        float startX = bounds.x + fmargin;
        float startY = bounds.y + fmargin;
        float linesacing = fsize * 0.75f;
        //clear all lines since text has changed
        m_lines.clear();
        // create data at 0 index for first line
        m_lines.push_back({ std::string{},  { (float)startX, (float)startY } });
        // get font
        m_sharedfont = AssetManager::Get().LoadFontAsset(m_font, fsize);
        
        // assuming 'W' is the larges of the letter get size 
        Vector2 fntmeasure = MeasureTextEx(*m_sharedfont, "W", fsize * scrnScale,fspace);

        // current letter count;
        m_letterCount = std::min((size_t)m_text.size(), (size_t)m_charLimit);
         
        // create line data by checking text width 
        for (int i = 0; i < m_letterCount; i++)
        {   
            // add char to the last line in lhe list
            m_lines[m_lines.size() - 1].first.append(std::string{ name[i] });
            // if chars width in line is larger than available Width - (magins + last char width)
            if (MeasureTextEx(*m_sharedfont, m_lines[m_lines.size() - 1].first.c_str(), fsize, fspace).x >= bounds.width - (fntmeasure.x + fmargin * 2))
            {
                // create new object at the end of list for new line 
                m_lines.push_back({ std::string{},  {startX , startY + (fntmeasure.y * m_lines.size()) + (linesacing * m_lines.size())} });
            }
        }
    }

    /// <summary>
    /// TextInput::ProcessInput -  Draw blinking underscore char
    /// </summary>
    void TextInput::DrawInputCursor(Rectangle bounds)
    {
        float scrnScale =  World::UI_CANVAS.get()->Scale();
        float fsize = std::max(m_minfontsize, std::ceil(m_fontsize * scrnScale));
        float fspace = std::max(m_minfontspacing, m_fontspacing * scrnScale);
        float margin = m_textmargin * scrnScale;
        int chrcount = 0, lc = -1, chrs = 0, s = 0, txtmargin, fntspace;

        int ltrcnt = 0;
        Vector2 fntsize, fntmeasure, cursorloc;
        fntsize = MeasureTextEx(*m_sharedfont, "W", fsize, fspace);
        fntmeasure = { 0,0 };
        txtmargin = m_textmargin * scrnScale;
        fntspace = m_fontspacing * scrnScale;
        cursorloc = { bounds.x + txtmargin, bounds.y + txtmargin };
        
        if (!m_infofnt)
        {
            std::string staticassetpath = m_owningworld->GetApplication()->GetStaticAssetsPath();
            json& configdata = CARTjson::GetAppData();
            std::string fntpath = configdata["cart"]["font"]["verdana"]["path"];
            m_infofnt = AssetManager::Get().LoadFontAsset(std::string{ staticassetpath + fntpath }, 14);
        }
        float startX = bounds.x + margin;
        float startY = bounds.y + margin;
        for (auto iter = m_lines.begin(); iter != m_lines.end(); ++iter)
        {            
            if (m_curletterindex >= ltrcnt && m_curletterindex <= ltrcnt + iter->first.size())
            {
                startY = iter->second.y;
                std::string s = iter->first;
                std::string sl = s.substr(0, m_curletterindex - ltrcnt);
                fntmeasure = MeasureTextEx(*m_sharedfont, sl.c_str(), fsize, fspace);
                startX +=  fntmeasure.x;
                break;
            }

             ltrcnt += iter->first.size();
        }

        
        if (((m_framesCounter / 10) % 2) == 0) DrawText("|", startX, startY, fsize + 2, GRAY);
    }

    void TextInput::ShowCharLimitWarning(Rectangle bounds)
    {
        float fsize = 12;
        float fspace = 1.4f;
        float margin = 5;

        std::string alert = { "Reached chararcter limit. Press BACKSPACE to delete chars..." };
        if (!m_infofnt)
        {
            std::string staticassetpath = m_owningworld->GetApplication()->GetStaticAssetsPath();
            json& configdata = CARTjson::GetAppData();
            std::string fntpath = configdata["cart"]["font"]["verdana"]["path"];
            m_infofnt = AssetManager::Get().LoadFontAsset(std::string{ staticassetpath + fntpath }, fsize);
        }
        Vector2 fntmeasure = MeasureTextEx(*m_infofnt, alert.c_str(), fsize, fspace);

        DrawTextEx(*m_infofnt, alert.c_str(), { bounds.x + margin, bounds.y + bounds.height - (fntmeasure.y + margin) }, fsize, fspace, BLACK);
       
    }

    void TextInput::ShowRemainingCharCount(Rectangle bounds)
    {

        float fsize = 12;
        float fspace = 1.4f;
        float margin = 5;
        int count = m_charLimit - m_letterCount;
        std::string alert = { "Remaining: "+ std::to_string(count)};
        if (!m_infofnt)
        {
            std::string staticassetpath = m_owningworld->GetApplication()->GetStaticAssetsPath();
            json& configdata = CARTjson::GetAppData();
            std::string fntpath = configdata["cart"]["font"]["verdana"]["path"];
            m_infofnt = AssetManager::Get().LoadFontAsset(std::string{ staticassetpath + fntpath }, fsize);
        }
        Vector2 fntmeasure = MeasureTextEx(*m_infofnt, alert.c_str(), fsize, fspace);

        DrawTextEx(*m_infofnt, alert.c_str(), { bounds.x +  margin, bounds.y +  bounds.height - (fntmeasure.y + margin) }, fsize, fspace, BLACK);

    }

#pragma endregion


#pragma region Event Listener

    void TextInput::OnScreenSizeChange()
    {
        UpdateLayout();
    }
    void TextInput::OnMobileInput(char* input)
    {
        std::string left, right, fs, tmpstr, backspace;
        m_mobileinput =  input ;// new string
     
        //tmpstr =  name;// current string
        //backspace =  "<backspace>" ;
     
        //left = tmpstr.substr(0, std::max(0,m_curletterindex));
        //right = tmpstr.substr(std::max(0, m_curletterindex), m_letterCount);

        //bool isBackspace = backspace.compare(m_mobileinput) == 0;

        //if (isBackspace) {
        //    m_curletterindex -= 1;
        //    left = left.substr(0, m_curletterindex);
        //    fs = left + right;
        //}
        //else {
        //    m_curletterindex += tmpstr.size();
        //    fs = { left + m_mobileinput + right };
        //}
        Logger::Get()->Trace(std::format("TextInput::OnMobileInput m_mobileinput{} | fs | {}  \0", m_mobileinput,  fs));
        m_curletterindex = m_mobileinput.size();
        SetText(m_mobileinput);
 /*       for (size_t i = 0; i < MAX_INPUT_CHARS; i++)
        {
            name[i] = '\0';
        }
        for (size_t i = 0; i < len; i++)
        {
            name[i] = (char)fs.at(i);
        }*/
       // Logger::Get()->Trace(std::format("TextInput::OnMobileInput  name | {}  \0", std::string{ name }));
        //int key = std::stoi(std::string{ input });
        //if ((key >= 32) && (key <= 125) && (m_letterCount < m_charLimit)) {
        //    m_letterCount = 0;
        //    std::string mutable_str{ input };
        //    for (size_t i = 0; i < MAX_INPUT_CHARS; i++)
        //    {
        //        name[i] = '\0';
        //    }
        //    while (mutable_str.size() > 0) {// transfre all chars to local char array
        //        std::string achar = mutable_str.substr(0, 1);
        //        name[m_letterCount] = (char)achar.c_str();
        //        mutable_str = mutable_str.substr(1);
        //        m_letterCount++;
        //    }
        //}
       

        //if (IsKeyPressed(KEY_BACKSPACE)) {
        //    if (m_letterCount > 0) {
        //        m_letterCount--;
        //        name[m_letterCount] = '\0';
        //    }
        //}
    }
#pragma endregion

}