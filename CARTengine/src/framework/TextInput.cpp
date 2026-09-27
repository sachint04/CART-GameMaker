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
        m_hasUpated{ true },
        m_cursorLoc{ 0 },
        m_keydownWaitTimeMultiplyer{ 0.5f },
        m_keydownMulitiplyerDuration{ 1.f },
        m_keydownActionDuration{ 1.f },
        m_tempkeydownActionDuration{ 1.f },
        m_keydownActionMinDuration{ 0.015f },
        m_bMobileInput{ false },
        m_bPreparingInput{false},
        m_bnewline{false},
        m_textoutofbound{false}
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
        Logger::Get()->Trace("TextInput::Start()");
       
        Text::Start();        
      //  PrepareInput(GetBounds());
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
//        Logger::Get()->Trace("TextInput::Update() started");

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
            if (d <= 0)return;
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
                    SetCursorAt(m_touchendpos);
                    CalculateCursor(rect);
                }
                m_touch = false;
            }
            
        }
       

        if (IsKeyReleased(KEY_ENTER)) {          
            if (m_letterCount < m_charLimit)
            {
                int currentLen = (int)strlen(m_chr);
                if (m_curletterindex < m_letterCount) {
                    int moveCount = m_letterCount - m_curletterindex;
                    std::memmove(&m_chr[m_curletterindex + 1], &m_chr[m_curletterindex], moveCount + 1);

                    m_chr[m_curletterindex] = '\n';
                }
                else {
                    m_chr[m_curletterindex] = '\n';
                }
                m_bnewline = true;
                m_chr[currentLen + 1] = '\0'; // Add null terminator at the end of the string.                 
                m_curletterindex++;
                m_letterCount++;
                m_hasUpated = true;
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
                whilebackspace(m_letterCount, m_chr, m_curletterindex);
                m_hasUpated = true;
                m_isBackspace = true;
            //    Logger::Get()->Trace(std::format("TextInput::Update() Backspace Release - letter count {} | chr index {}", m_letterCount, m_curletterindex));
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
            whiledeletekey(m_letterCount, m_chr, m_curletterindex);
               
            //  m_keyWaitTimer = Clock::Get().ElapsedTime();
            //  m_isDeleteKey = true;
        }

        // Check if more characters have been pressed on the same frame
        while (key > 0 )
        {
      //      Logger::Get()->Trace("TextInput::Update() key listener started");
      //            int  txtmargin = m_textmargin * scrnScale;
            int tx = rect.x + 8;
            int ty = rect.y + 8;
            // NOTE: Only allow keys in range [32..125]
            if ((key >= 32) && (key <= 125) && (m_letterCount < m_charLimit))
            {
                if (m_curletterindex < m_letterCount)
                {
                    typeinbetween(m_letterCount, key, m_chr, m_curletterindex);
                }
                else {
                    m_letterCount = std::strlen(m_chr);
                    m_chr[m_letterCount] = (char)key;
                    m_letterCount++;
                    m_chr[m_letterCount] = '\0'; // Add null terminator at the end of the string.                 
                    m_curletterindex = m_letterCount ;
                  //  Logger::Get()->Trace(std::format("TextInput::Update() Key  - letter count {} | chr index {}", m_letterCount, m_curletterindex));
                }

                m_hasUpated  = true;
                   
                m_isBackspace = false;
                m_isLeftKey = false;
                m_isRightKey = false;
                m_isDeleteKey = false;
               // m_letterCount++;
             //   Logger::Get()->Trace(std::format("TextInput::Update() letter count {} ", m_letterCount));
            }
            key = GetCharPressed();  // Check next character in the queue
          //  Logger::Get()->Trace("TextInput::Update() key listener ended");
        }
        // Set the window's cursor to the I-Beam
         SetMouseCursor(m_isFocused&& m_mouseOnText ? MOUSE_CURSOR_IBEAM : MOUSE_CURSOR_DEFAULT);

      
        if (m_isBackspace) {
//#ifdef _WIN32
            double t = Clock::Get().ElapsedTime();
            if (t - m_keyWaitTimer > m_keydownMulitiplyerDuration) {
                m_tempkeydownActionDuration = std::max(m_keydownActionMinDuration, m_tempkeydownActionDuration * m_keydownWaitTimeMultiplyer);
                m_keyWaitTimer = t;
            }
            //float keydownActionMulitiplyer = m_keydownMulitiplyerDuration;
            if (t - m_backspacekeyWaitTimer >= m_tempkeydownActionDuration)
            {
             //   Logger::Get()->Trace(std::format("backspace action {}", m_letterCount));
                if (m_letterCount > 0) {
                    whilebackspace(m_letterCount, m_chr, m_curletterindex);
                    m_hasUpated = true;
                }
                m_backspacekeyWaitTimer = t;
            }     
         //   Logger::Get()->Trace(std::format("TextInput::Update On Backspace continue letter count {} | chr index {}", m_letterCount, m_curletterindex));
//#endif // _WIN32
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
                whiledeletekey(m_letterCount, m_chr, m_curletterindex);
                m_text = m_chr;
                m_keyWaitTimer = t;
                m_hasUpated = true;
            }
            if (IsKeyUp(KEY_DELETE))
                m_isDeleteKey = false;
        }
        if (m_hasUpated) {
            m_text = m_chr;
           // if (!m_bPreparingInput)
        //    {
                PrepareInput();
       //     }
            m_hasUpated = false;
        }
        if (m_owningworld->GetInputController()->HasFocus() &&
            m_owningworld->GetInputController()->GetFocusedId().compare(GetId()) == 0) m_framesCounter++;
        else m_framesCounter = 0;

      //  Logger::Get()->Trace("TextInput::Update() ended");
	}

    void TextInput::Draw(float _deltaTime)
    {
        if (!m_visible)return;
       // Logger::Get()->Trace("TextInput::Draw() started");
       // UIElement::Draw(_deltaTime);
        Rectangle rect = GetBounds();
        
        m_textoutofbound = false;
        TextLine(rect);
       
        // DrawRectangleRec(textBox, LIGHTGRAY);
        if (m_isFocused)
        {            
            if (m_textoutofbound) {
                 if (((m_framesCounter / 10) % 2) == 0) DrawRectangleLinesEx({ rect.x - 1, rect.y - 1, rect.width + 1, rect.height + 1 }, 2.f, RED);
            }
            else {
               DrawRectangleLinesEx({ rect.x - 1, rect.y - 1, rect.width + 1, rect.height + 1}, 2.f, DARKGRAY );

            }
            float scrnScale = World::UI_CANVAS.get()->Scale();
           // if (((m_framesCounter / 10) % 5) == 0) DrawText("|", m_cursorLoc.x, m_cursorLoc.y, std::max(m_minfontsize * scrnScale, m_fontsize * scrnScale) + 4, GRAY);
            DrawText("|", m_cursorLoc.x, m_cursorLoc.y, std::max(m_minfontsize * scrnScale, m_fontsize * scrnScale) + 4, GRAY);

        }
        else {
            DrawRectangleLines( rect.x, rect.y, rect.width, rect.height, LIGHTGRAY);
        }

        if (m_letterCount >= m_charLimit)
            ShowCharLimitWarning(rect);
        else
            ShowRemainingCharCount(rect);

      //  Logger::Get()->Trace("TextInput::Draw() ended");
	}
#pragma endregion

#pragma region  Helper
    void TextInput::SetText(const std::string& txt)
    {
      //  Logger::Get()->Trace(std::format("TextInput::SetButtonText() txt {} ", txt));
        m_letterCount = (txt.size() > m_charLimit)? m_charLimit : txt.size();
        for (size_t i = 0; i < m_letterCount; i++)
        {
            m_chr[i] = m_text.at(i);
        }
        m_chr[m_letterCount] = '\0';
        m_text = m_chr;
        m_curletterindex = m_letterCount;
        if (m_isReady)PrepareInput();
        Logger::Get()->Trace(std::format("TextInput::SetText() letter count {} ", m_letterCount));
    }
    std::string TextInput::GetFontName()
    {
        std::string staticassetpath = m_owningworld->GetApplication()->GetStaticAssetsPath();
        auto find = m_font.find_first_of(staticassetpath);
        if (find != std::string::npos) {
            std::string strfnt = m_font.substr(find + staticassetpath.size());
            return strfnt;
        }
        return m_font;
    }
    void TextInput::SetText(const char* chrt)
    {
        SetText(std::string{ chrt });         
    }
    void TextInput::SetFontName(const std::string& strfnt)
    {
        Text::SetFontName(strfnt);
        if (m_isReady)PrepareInput();
    }
    void TextInput::SetFontSize(float size)
    {
        Text::SetFontSize(size);
        if (m_isReady)PrepareInput();
     
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
                std::string curtxt = m_chr;
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
      /*  auto iter = m_lines.begin();
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
        return message;*/
        return m_text;
    } 
    void TextInput::UpdateLayout()
    {
        Text::UpdateLayout();
       // SetButtonText(m_text);
    }
  
    /// <summary>
    /// TextInput::TextLine - Draw text on screen
    /// </summary>
    /// <param name="str"></param>
    void TextInput::TextLine(Rectangle rect)
    {
        float scrnScale = World::UI_CANVAS.get()->Scale();
        float fsize =  m_fontsize * scrnScale;
        float fspace = m_fontspacing;// std::max(m_minfontspacing * scrnScale, m_fontspacing * scrnScale);
        m_sharedfont = AssetManager::Get().LoadFontAsset(m_font, fsize);

        int count = 0;
       /* for (auto iter = m_lines.begin(); iter != m_lines.end(); ++iter)
        {
               DrawTextEx(*m_sharedfont, iter->c_str(), m_pos.at(count), fsize, fspace, m_textColor);
               count++;
        }*/
        
        float safeareapadding = 10.f;
        for (auto i = 0; i < m_strlines.size(); i++)
        {
            
            bool validline = m_strlines.at(i).size.y >= (rect.y - safeareapadding) && m_strlines.at(i).size.y + m_strlines.at(i).size.height <= (rect.y + rect.height + safeareapadding);
            if (!validline)m_textoutofbound = true;
            std::string text = TextSubtext(m_strlines.at(i).text.data(), 0, m_strlines.at(i).text.length());
            DrawTextEx(*m_sharedfont, text.c_str(), { m_strlines.at(i).size.x, m_strlines.at(i).size.y }, fsize, fspace, validline ? m_textColor : ColorAlpha(m_textColor, 0.5f));
        }
        
       
    }
    /// <summary>
    /// Prepare array of string line  and  its position relative to text box
    /// </summary>
    /// 
    void TextInput::PrepareInput()
    {
        Rectangle rect = GetBounds();
        FormatInput(rect);
        CalculateCursor(rect);
    }
    void TextInput::FormatInput(Rectangle rect)
    {
        float uiScale = World::UI_CANVAS.get()->Scale();
        float scaleY = World::UI_CANVAS.get()->ScaleY();
        float fsize = m_fontsize * uiScale;
      
     //   float fsize = std::max(m_minfontsize, m_fontsize * uiScale);
        float fspace = std::max(m_minfontspacing, m_fontspacing * uiScale);
#pragma region Wrap Text new logic
        Vector2 msize = { 0,0 };
        float linespacing = m_linespace;
        float maxwidth = rect.width;

        m_sharedfont = AssetManager::Get().LoadFontAsset(m_font, fsize);
        
        Text::WrapText(m_text, maxwidth, linespacing, m_strlines, m_sharedfont, fsize, fspace, linespacing);
        //Remove \n from text
       /* int count = std::count(m_text.begin(), m_text.end(), '\n');
        m_text.erase(std::remove(m_text.begin(), m_text.end(), '\n'), m_text.end());*/
       /* if (count > 0) {
            size_t lengthToCopy = std::min(m_text.length(), (size_t)MAX_INPUT_CHARS);
            std::memcpy(m_chr, m_text.c_str(), lengthToCopy);
            m_chr[lengthToCopy] = '\0';
        }*/
      //  m_letterCount = m_text.size();
        //m_curletterindex -= count;
#pragma endregion
#pragma region Calculate Start position based on Alignment
        float theight = 0;
        auto find = m_strlines.begin();

        if (find == m_strlines.end()) {
            Vector2 emptysize = MeasureTextEx(*m_sharedfont, " ", fsize, fspace);
            m_strlines.push_back({ "" , {rect.x, rect.y, 0.f, emptysize.y * linespacing} });
        }
        for (auto& line : m_strlines)
        {
            theight += line.size.height;
        }

        int al = m_align, va = m_valign;

        float sy = rect.y;
        if (va == 1) {
            sy = rect.y + ((rect.height - theight) * 0.5f);
            if (sy > rect.y + rect.height)sy = rect.y + rect.height;
        }
        else if (va == 2) {
            sy = rect.y + (rect.height - theight);
            if (sy < rect.y)sy = rect.y;
        }

        for (auto iter = m_strlines.begin(); iter != m_strlines.end(); ++iter)
        {
            float sx = rect.x;
            Vector2 linesize = { iter->size.width, iter->size.height };
            //  Align
            if (al == 0)
            {
                sx +=  m_margin;
            }
            else if (al == 1) {
                sx += (rect.width - (linesize.x)) * 0.5f;
            }
            else if (al == 2) {
                sx += (rect.width - (linesize.x + m_margin));
            }
            
            iter->size.x = sx;
            iter->size.y = sy;
            sy += linesize.y  + linespacing;
        }
#pragma endregion
    }
    /// <summary>
    /// TextInput::ProcessInput -  Draw blinking underscore char
    /// </summary>
    void TextInput::CalculateCursor(Rectangle rect)
    {
       // Logger::Get()->Trace("TextInput::CalculateCursor() started {} ");
        float scrnScale =  World::UI_CANVAS.get()->Scale();
        float fsize = std::max(m_minfontsize * scrnScale, m_fontsize * scrnScale);
        float fspace = m_fontspacing;// std::max(m_minfontspacing * scrnScale, m_fontspacing * scrnScale);
        float linespacing = 2.0f;
        int lcount = 0;
       int count = 0;
       int len = 0;

       auto begin = m_strlines.begin();
       m_cursorLoc.x = begin->size.x;
       m_cursorLoc.y = begin->size.y;
       bool success = false;
       int tmpletterindex = m_curletterindex;
       if (m_strlines.empty())
       {
           return;
       }
       else  if (m_curletterindex == m_letterCount)
       {
           WrappedLine last = m_strlines[m_strlines.size() - 1];
           std::string str = TextSubtext(last.text.data(), 0, last.text.size());

           if (str.empty()) {
               m_cursorLoc.y = last.size.y;
               return;
           }
           else {
                Vector2 fntmeasure = MeasureTextEx(*m_sharedfont, str.c_str(), fsize, fspace);
               m_cursorLoc.y =  last.size.y;
               m_cursorLoc.x = last.size.x + last.size.width;
           }
       }
       else {

           for (size_t i = 0; i < m_strlines.size(); i++)
           {
               int charcount = m_strlines[i].text.size();
               if (tmpletterindex <= len + charcount) {
               
                   m_cursorLoc.y = m_strlines[i].size.y;
                   m_cursorLoc.x = m_strlines[i].size.x;
                   int tmplen = len + charcount;
                   if (tmpletterindex == tmplen) {
                       std::string str = TextSubtext(m_strlines[i].text.data(), 0, m_strlines[i].text.size());
                       Vector2 fntmeasure = MeasureTextEx(*m_sharedfont, str.c_str(), fsize, fspace);
                       m_cursorLoc.x = m_strlines[i].size.x + fntmeasure.x;
                   }
                   else {
                       for (size_t n = 0; n < charcount; n++)
                       {
                           if (tmpletterindex == (len + n)) {
                               std::string str = TextSubtext(m_strlines[i].text.data(), 0, m_strlines[i].text.size());
                                std::string sl = str.substr(0, n);
                                Vector2 fntmeasure = MeasureTextEx(*m_sharedfont, sl.c_str(), fsize, fspace);
                                m_cursorLoc.x = m_strlines[i].size.x + fntmeasure.x;
                                success = true;                       
                                break;
                           }                   
                       } 
                   }
               
                  break;
               }
               len += (charcount + 1); // added extra index for new line;
              // tmpletterindex--; // since m_currentindex is based on raw char string. we reduce index by one position for '\n'
         }
      
       }
      
    }
    void TextInput::ShowCharLimitWarning(Rectangle rect)
    {
        float fsize = 8 * World::UI_CANVAS.get()->Scale();
        std::string alert = { "Reached chararcter limit.\nPress BACKSPACE to delete chars..." };
        DrawText(alert.c_str(),  rect.x , rect.y + rect.height + 5 , fsize, RED);
       
    }
    void TextInput::ShowRemainingCharCount(Rectangle rect)
    {

        float fsize = 8 * World::UI_CANVAS.get()->Scale();
        int count = m_charLimit - m_letterCount;
        std::string alert = { "Remaining: "+ std::to_string(count)};      
        DrawText(alert.c_str(),  rect.x, rect.y +  rect.height + 5, fsize, BLACK);
    }    
    void TextInput::SetAligned(ALIGN _align)
    {
        Text::SetAligned(_align);
        
        if(m_isReady)m_hasUpated = true;
    }
    void TextInput::SetVAligned(V_ALIGN _valign)
    {
        Text::SetVAligned(_valign);
        if (m_isReady)PrepareInput();
    }
    void TextInput::SetCursorAt(Vector2 pos)
    {
        float scrnScale = World::UI_CANVAS.get()->Scale();
        int tmpLtrCount = 0;
        float linespacing = 0;
        float fsize = std::max(m_minfontsize * scrnScale, m_fontsize * scrnScale);
        for (auto iter = m_strlines.begin(); iter != m_strlines.end(); ++iter)
        {   
            if (pos.y > iter->size.y  && pos.y < iter->size.y + iter->size.height) {
                std::string text = TextSubtext(iter->text.data(), 0, iter->text.size());
                Vector2 fm = MeasureTextEx(*m_sharedfont, text.c_str(), fsize, m_fontspacing);                
                int chr = 0;
                int chrlen = text.size();
                if (pos.x >= iter->size.x + iter->size.width)
                {
                    tmpLtrCount += chrlen;
                }
                else {                    
                    for (size_t i = 0; i < chrlen; i++)
                    {              
                        std::string nr = text.substr(0, i);
                        if (nr.empty()) {
                            //tmpLtrCount++;
                            continue;
                        }
                        Vector2 charsize = MeasureTextEx(*m_sharedfont, nr.c_str(), fsize, m_fontspacing);
                        if (iter->size.x + charsize.x >= pos.x)
                        {     
                            break;
                        }
                        tmpLtrCount++;// increate letter count untile found in the line;                    
                    }
                }                
                                
                break;
            }
            tmpLtrCount += (iter->text.size()) + 1; // Add extra index for new line                        
        }
        m_curletterindex = tmpLtrCount;
    }
#pragma endregion


#pragma region Event Listener
    void TextInput::OnScreenSizeChangeHandler()
    {
        UpdateLayout();
        if (m_isReady)PrepareInput();;
    }
    void TextInput::OnMobileInput(char* input, int isBackspace)
    {

        Logger::Get()->Trace(std::format("TextInput::OnMobileInput() input {} | Backspace {} ", std::string{ input }, isBackspace));
        // Check for Backspace evet
        if (isBackspace == 1 || *input  == 8) {
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
                whilebackspace(m_letterCount, m_chr, m_curletterindex);
             
                m_text = m_chr;
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
                    typeinbetween(m_letterCount, key, m_chr, m_curletterindex);
                }
                else {
                    std::string s(1, static_cast<char>(key));
                    m_chr[m_letterCount] = s[0];
                    m_chr[m_letterCount + 1] = '\0'; // Add null terminator at the end of the string.                 
                    m_curletterindex = m_letterCount + 1;
                }

                m_text = m_chr;
                m_hasUpated = true;

                m_isBackspace = false;
                m_isLeftKey = false;
                m_isRightKey = false;
                m_isDeleteKey = false;
                m_letterCount++;
            }
            else if (key == 13) {// Enter key
                if (m_letterCount < m_charLimit)
                {
                    int currentLen = (int)strlen(m_chr);
                    if (m_curletterindex < m_letterCount) {
                        int moveCount = m_letterCount - m_curletterindex;
                        std::memmove(&m_chr[m_curletterindex + 1], &m_chr[m_curletterindex], moveCount + 1);

                        m_chr[m_curletterindex] = '\n';
                    }
                    else {
                        m_chr[m_curletterindex] = '\n';
                    }
                    m_bnewline = true;
                    m_chr[currentLen + 1] = '\0'; // Add null terminator at the end of the string.                 
                    m_curletterindex++;
                    m_letterCount++;
                    m_hasUpated = true;
                }

            }
        }

    }
#pragma endregion

}