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
        m_textmargin{ 2.f },
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
        PrepareInput(GetBounds());
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
        Rectangle rect = GetBounds();
        float scrnScale =  World::UI_CANVAS.get()->Scale();
        
        auto typeinbetween = [&](int& c, int key, char* s, int& d)
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
        };
        
        auto whilebackspace = [&](int &c, char * s, int &d)
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

        auto whiledeletekey = [&](int& c, char* s, int& d)
        {
            if (d == c)return;
           
            size_t i = 0;
            for (i = d ; i < c; i++)
            {
                s[i] = s[i + 1];
            }                        
            c--;            
            s[c] = '\0';
        };

         int tCount = GetTouchPointCount();
        // Get char pressed (unicode character) on the queue
        int key = GetCharPressed();
        m_mouseOnText = m_owningworld->GetInputController()->IsMouseOver(GetWeakRef());
        // if Input is currently Selected
        if (m_mouseOnText)
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
                m_curletterindex = m_letterCount;
                if (GetVectorLength(Direction(m_touchendpos, m_touchstartpos)) < 5) {
                    Vector2 fm = MeasureTextEx(*m_sharedfont, "W", (float)m_fontsize * scrnScale, m_fontspacing * scrnScale);
                    int tmpLtrCount = 0;
                    int count = 0;                    
                    float linespacing = 2.0f;
                    float fsize = std::max(m_minfontsize * scrnScale, m_fontsize * scrnScale);
                    for (auto iter = m_lines.begin(); iter != m_lines.end(); ++iter) 
                    {
                        if (m_touchendpos.y > m_pos[count].y && m_touchendpos.y <= m_pos.at(count).y + fm.y + linespacing)
                        {
                            int chr = 0;
                            while (chr < iter->length() - 1)
                            {
                                std::string nr = iter->substr(0, chr);
                                if (m_pos[count].x + MeasureTextEx(*m_sharedfont, nr.c_str(), fsize, m_fontspacing).x >= m_touchendpos.x)
                                {
                                    m_curletterindex = tmpLtrCount + nr.size();     
                                    break;
                                }
                                chr++;
                            }
                        }
                        tmpLtrCount += iter->size();                        
                        count++;
                    }
                }
                CalculateCursor(rect);
                m_touch = false;
            }
            else {
                SetMouseCursor(MOUSE_CURSOR_DEFAULT);
            }
        }

        if (IsKeyReleased(KEY_BACKSPACE)) {
            m_tempkeydownActionDuration = m_keydownMulitiplyerDuration;
            m_isBackspace = false;
        }
        if (IsKeyReleased(KEY_LEFT))
        {
            m_isLeftKey = false;
        }           
        if (IsKeyPressed(KEY_BACKSPACE))
        {
            if (!m_isBackspace) {
                m_backspacekeyWaitTimer = Clock::Get().ElapsedTime();
                m_keyWaitTimer = Clock::Get().ElapsedTime();
                whilebackspace(m_letterCount, name, m_curletterindex);
                m_hasUpated = true;
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
            whiledeletekey(m_letterCount, name, m_curletterindex);
               
            //  m_keyWaitTimer = Clock::Get().ElapsedTime();
            //  m_isDeleteKey = true;
        }
  
        // Check if more characters have been pressed on the same frame
        while (key > 0 )
        {
            int  txtmargin = m_textmargin * scrnScale;
            int tx = rect.x + 8;
            int ty = rect.y + 8;
            // NOTE: Only allow keys in range [32..125]
            if ((key >= 32) && (key <= 125) && (m_letterCount < m_charLimit))
            {
                if (m_curletterindex < m_letterCount)
                {
                    typeinbetween(m_letterCount, key, name, m_curletterindex);
                }
                else {
                    name[m_letterCount] = (char)key;
                    name[m_letterCount + 1] = '\0'; // Add null terminator at the end of the string.                 
                    m_curletterindex = m_letterCount + 1;
                }

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
#ifdef _WIN32
            double t = Clock::Get().ElapsedTime();
            if (t - m_keyWaitTimer > m_keydownMulitiplyerDuration) {
                m_tempkeydownActionDuration = std::max(m_keydownActionMinDuration, m_tempkeydownActionDuration * m_keydownWaitTimeMultiplyer);
                m_keyWaitTimer = t;
            }
            //float keydownActionMulitiplyer = m_keydownMulitiplyerDuration;
            if (t - m_backspacekeyWaitTimer >= m_tempkeydownActionDuration)
            {
             //   Logger::Get()->Trace(std::format("backspace action {}", m_letterCount));
                if (m_letterCount >= 0) {
                    whilebackspace(m_letterCount, name, m_curletterindex);
                    m_hasUpated = true;
                }
                m_backspacekeyWaitTimer = t;
            }
#endif // _WIN32
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
                whiledeletekey(m_letterCount, name, m_curletterindex);
                m_text = name;
                m_keyWaitTimer = t;
                m_hasUpated = true;
            }
            if (IsKeyUp(KEY_DELETE))
                m_isDeleteKey = false;
        }
        if (m_hasUpated) {
            m_text = name;
            PrepareInput(rect);
            CalculateCursor(rect);
            m_hasUpated = false;
        }
        if (m_owningworld->GetInputController()->HasFocus() &&
            m_owningworld->GetInputController()->GetFocusedId().compare(GetId()) == 0) m_framesCounter++;
        else m_framesCounter = 0;
	}

    void TextInput::Draw(float _deltaTime)
    {
        if (!m_visible)return;
       // UIElement::Draw(_deltaTime);
        Rectangle rect = GetBounds();
        
        // DrawRectangleRec(textBox, LIGHTGRAY);
        if (m_isFocused)
        {            
           DrawRectangleLinesEx({ rect.x - 1, rect.y - 1, rect.width + 1, rect.height + 1}, 2.f, DARKGRAY);
            float scrnScale = World::UI_CANVAS.get()->Scale();
            if (((m_framesCounter / 10) % 2) == 0) DrawText("|", m_cursorLoc.x, m_cursorLoc.y, std::max(m_minfontsize * scrnScale, m_fontsize * scrnScale) + 4, GRAY);
        }
        else {
            DrawRectangleLines( rect.x, rect.y, rect.width, rect.height, LIGHTGRAY);
        }
        
        TextLine(rect);

        if (m_letterCount >= m_charLimit)
            ShowCharLimitWarning(rect);
        else
            ShowRemainingCharCount(rect);


	}
#pragma endregion

#pragma region  Helper
    void TextInput::SetText(const std::string& txt)
    {
      //  Logger::Get()->Trace(std::format("TextInput::SetText() txt {} ", txt));
        m_text = txt;
        m_letterCount = std::min(m_charLimit,  (int)m_text.size());
        for (size_t i = 0; i < m_letterCount; i++)
        {
            name[i] = i < (char)m_text.size()? m_text.at(i) : '\0';
        }
        m_curletterindex = m_letterCount;
        m_hasUpated = true;
    }
    void TextInput::SetFontName(const std::string& strfnt)
    {
        Text::SetFontName(strfnt);
       // m_hasUpated = true;
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
      //  Logger::Get()->Trace(std::format("TextInput::SetFocused()! default text {}", m_text));
       int useragent = CARTjson::GetEnvSettings()["useragent"];
       if (useragent > 0) {// if useragent is > 0 (mobile browser) show os keyboard
            if (_flag) {// currently is in focus
                std::string curtxt = name;
                m_owningworld->GetApplication()->RegisterListernerToMobileInput(GetId(), GetWeakRef(), &TextInput::OnMobileInput);
                std::string s = { "" };
                m_owningworld->GetApplication()->ToggleMobileWebKeyboard(s, KeyboardType::Default, false, true, false, false, std::string{""}, m_charLimit);
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
            if (iter->size() > 0)
            {
                message += *iter ;
            }
            ++iter;
            Logger::Get()->Trace(message);
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
    void TextInput::TextLine(Rectangle rect)
    {
        float scrnScale = World::UI_CANVAS.get()->Scale();
        float fsize = std::max(m_minfontsize * scrnScale, m_fontsize * scrnScale);
        float fspace = m_fontspacing;// std::max(m_minfontspacing * scrnScale, m_fontspacing * scrnScale);
        m_sharedfont = AssetManager::Get().LoadFontAsset(m_font, fsize);

        int count = 0;
        for (auto iter = m_lines.begin(); iter != m_lines.end(); ++iter)
        {
               DrawTextEx(*m_sharedfont, iter->c_str(), m_pos.at(count), fsize, fspace, m_textColor);
               count++;
        }
       
    }

    /// <summary>
    /// Prepare array of string line  and  its position relative to text box
    /// </summary>
    /// 
    void TextInput::PrepareInput(Rectangle rect)
    {

        float scrnScale =  World::UI_CANVAS.get()->Scale();
        float fsize = std::max(m_minfontsize * scrnScale, m_fontsize * scrnScale);
        float fspace = m_fontspacing;// std::max(m_minfontspacing * scrnScale, m_fontspacing * scrnScale);        
        // get font
        
        m_sharedfont = AssetManager::Get().LoadFontAsset(m_font, fsize);
       
#pragma region  Draw 
        m_lines.clear();
        m_pos.clear();
        std::string strcopy = m_text;
        std::string spacedelimiter = " ";
        float theight = 0;
        std::string line = "";
        float linespacing = 2.0f;
        Vector2 msize = { 0,0 };
        int al = m_align;
        int va = m_valign;
        int count = 0;
        Vector2 chrsize = MeasureTextEx(*m_sharedfont, " ", fsize, fspace);
        float maxw = rect.width - (m_textmargin * 2);
        int charperline = maxw / chrsize.x;
       
        for (size_t i = 0; i < strcopy.size(); i++)
        {
            std::string chr = strcopy.substr(0, i + 1);
            Vector2 s = MeasureTextEx(*m_sharedfont, chr.c_str(), fsize, fspace);
            if (s.x >= maxw) {
                auto find = chr.find_last_of(spacedelimiter);
                if (find != std::string::npos) {
                    chr = chr.substr(0, find + 1);
                }
                m_lines.push_back(chr);
                i = 0;
                strcopy = strcopy.substr(chr.size());
                Logger::Get()->Trace(std::format("Prepare input {}", i));
             }
        }

        if (strcopy.size() > 0) {
            m_lines.push_back(strcopy);
        }
        for (size_t i = 0; i < m_lines.size(); i++)
        {
            m_pos.push_back(MeasureTextEx(*m_sharedfont, m_lines[i].c_str(), fsize, fspace));
            theight += m_pos[i].y + linespacing;

        }

        float sy = rect.y + m_textmargin;
        if (va == 1) {
            sy = rect.y + ((rect.height  - (theight + m_textmargin)) * 0.5f);
        }
        else if (va == 2) {
            sy = rect.y + (rect.height - (theight + m_textmargin) );
        }


       for (auto iter = m_lines.begin(); iter != m_lines.end(); ++iter)
       {
           float sx = rect.x + m_textmargin;
          //  Align
            msize = MeasureTextEx(*m_sharedfont, iter->c_str(), (float)fsize, fspace);
           if (al == 1) {
                sx += (rect.width - (msize.x + m_textmargin) ) * 0.5f;
            }
            else if (al == 2) {               
                sx += (rect.width - (msize.x + m_textmargin));
            }
            m_pos.at(count) = { sx, sy };
            sy += msize.y + linespacing;
            count++;
        }
#pragma endregion
    
    }
    /// <summary>
    /// TextInput::ProcessInput -  Draw blinking underscore char
    /// </summary>
    void TextInput::CalculateCursor(Rectangle rect)
    {
        float scrnScale =  World::UI_CANVAS.get()->Scale();
        float fsize = std::max(m_minfontsize * scrnScale, m_fontsize * scrnScale);
        float fspace = m_fontspacing;// std::max(m_minfontspacing * scrnScale, m_fontspacing * scrnScale);
        float linespacing = 2.0f;
        int lcount = 0;
       int count = 0;
       int len = 0;

       m_cursorLoc.x = rect.x + m_textmargin;
       m_cursorLoc.y = rect.y + m_textmargin;

       for (size_t i = 0; i < m_lines.size(); i++)
       {
           len += m_lines[i].size();
           if (m_curletterindex <= len) {
               len -= m_lines[i].size();
               m_cursorLoc.y = m_pos[i].y;
               for (size_t n = 0; n < m_lines[i].size(); n++)
               {
                   if (m_curletterindex == len + 1) {
                        std::string sl = m_lines[i].substr(0, n + 1);
                        Vector2 fntmeasure = MeasureTextEx(*m_sharedfont, sl.c_str(), fsize, fspace);
                        m_cursorLoc.x = m_pos[i].x + fntmeasure.x;
                        Logger::Get()->Trace(std::format(" Set current Index! {} count {}", m_curletterindex, m_letterCount));
                        break;
                   }                   
                   len++;
               }
              break;
           }
       } 
    }

    void TextInput::ShowCharLimitWarning(Rectangle rect)
    {
        float fsize = 8 * World::UI_CANVAS.get()->Scale();
        std::string alert = { "Reached chararcter limit.\nPress BACKSPACE to delete chars..." };
        DrawText(alert.c_str(),  rect.x , rect.y + rect.height + 5 , fsize, DARKGRAY);
       
    }
    void TextInput::ShowRemainingCharCount(Rectangle rect)
    {

        float fsize = 8 * World::UI_CANVAS.get()->Scale();
        int count = m_charLimit - m_letterCount;
        std::string alert = { "Remaining: "+ std::to_string(count)};      
        DrawText(alert.c_str(),  rect.x, rect.y +  rect.height + 5, fsize, DARKGRAY);
    }    
    void TextInput::SetAligned(ALIGN _align)
    {
        Text::SetAligned(_align);
        m_hasUpated = true;
    }
    void TextInput::SetVAligned(V_ALIGN _valign)
    {
        Text::SetVAligned(_valign);
        m_hasUpated = true;
    }

#pragma endregion


#pragma region Event Listener

    void TextInput::OnScreenSizeChange()
    {
        UpdateLayout();
        m_hasUpated = true;
    }
    void TextInput::OnMobileInput(char* input, int isBackspace)
    {

      //  Logger::Get()->Trace(std::format("TextInput::OnMobileInput() input {} | Backspace {} ", std::string{ input }, isBackspace));
        // Check for Backspace evet
        if (isBackspace == 1) {
            //if (!m_isBackspace) {
                Rectangle textBox = GetBounds();
                float scrnScale = World::UI_CANVAS.get()->Scale();
                auto whilebackspace = [&](int& c, char* s, int& d)
                {
                    if (d - 1 < 0)return;
                    size_t i = 0;
                    for (i = d - 1; i < c; i++)
                    {
                        s[i] = s[i + 1];
                    }
                    c--;
                    d--;
                    s[c] = '\0';
                };
                whilebackspace(m_letterCount, name, m_curletterindex);
                m_text = name;
                m_hasUpated = true;
        }
        else {
            int key = std::stoi(input);
            if ((key >= 32) && (key <= 125) && (m_letterCount < m_charLimit))
            {
                Rectangle textBox = GetBounds();
                float scrnScale = World::UI_CANVAS.get()->Scale();

                auto typeinbetween = [&](int& c, int key, char* s,int& d)
                {
                    if (c + 1 >= MAX_INPUT_CHARS)return;

                    size_t i = 0;
                    for (i = c; i > d; i--)
                    {
                        s[i] = s[i - 1];
                    }
                    s[d] = (char)key;
                    c++;
                    d++;
                    s[c] = '\0';
                };
                              
                if (m_curletterindex < m_letterCount)
                {
                    typeinbetween(m_letterCount, key, name, m_curletterindex);
                }
                else {
                    std::string s(1, static_cast<char>(key));
                    name[m_letterCount] = s[0];
                    name[m_letterCount + 1] = '\0'; // Add null terminator at the end of the string.                 
                    m_curletterindex = m_letterCount + 1;
                }

                m_text = name;
                m_hasUpated = true;

                m_isBackspace = false;
                m_isLeftKey = false;
                m_isRightKey = false;
                m_isDeleteKey = false;
                m_letterCount++;
            }
        }

    }
#pragma endregion

}