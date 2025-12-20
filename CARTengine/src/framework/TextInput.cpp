#include "TextInput.h"
#include <stdexcept>
#include "utils/JSutils.h"
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
        m_keydownActionDuration{1.f},
        m_tempkeydownActionDuration{1.f},
        m_keydownActionMinDuration{0.015f}
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
        infofnt.reset();
	}
    void TextInput::Destroy() {
        if (m_isPendingDestroy)return;
        m_owningworld->GetInputController()->RemoveUI(GetId());
        Text::Destroy();
    }

    
   
#pragma endregion

#pragma region  LOOP

	void TextInput::Update(float _deltaTime)
	{
        if (!m_visible)return;
        Text::Update(_deltaTime);
        Rectangle textBox = GetBounds();
        float scrnScale = UICanvas::Get().lock()->Scale();
        
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
            Vector2 fntmeasure = MeasureTextEx(*m_sharedfont, "W", (float)m_fontsize * scrnScale, m_fontspacing * scrnScale);
            for (int i = 0; i < c; i++)
            {

                l[l.size() - 1].first.append(std::string{ s[i] });
                if (MeasureTextEx(*m_sharedfont, l[l.size() - 1].first.c_str(), (float)m_fontsize * scrnScale, m_fontspacing * scrnScale).x >= textBox.width - (fntmeasure.x + m_textmargin * scrnScale))
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

            Vector2 fntmeasure = MeasureTextEx(*m_sharedfont, "W", (float)m_fontsize * scrnScale, m_fontspacing * scrnScale);
            for (int i = 0; i < c; i++)
            {
 
                l[l.size() - 1].first.append(std::string{ s[i] });
                if (MeasureTextEx(*m_sharedfont, l[l.size() - 1].first.c_str(), (float)m_fontsize * scrnScale, m_fontspacing * scrnScale).x >= textBox.width - (fntmeasure.x + m_textmargin * scrnScale))
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

            Vector2 fntmeasure = MeasureTextEx(*m_sharedfont, "W", (float)m_fontsize * scrnScale, m_fontspacing * scrnScale);
            for (int i = 0; i < c; i++)
            {

                l[l.size() - 1].first.append(std::string{ s[i] });
                if (MeasureTextEx(*m_sharedfont, l[l.size() - 1].first.c_str(), (float)m_fontsize * scrnScale, m_fontspacing * scrnScale).x >= textBox.width - (fntmeasure.x + m_textmargin * scrnScale))
                {
                    l.push_back({ std::string{},  { (float)textBox.x + m_textmargin * scrnScale, ((float)textBox.y + 8) + (fntmeasure.y * l.size() - 1) } });
                
                }
            }
        };
        
        
      /*  if (CheckCollisionPointRec(GetMousePosition(), GetBounds())) m_mouseOnText = true;
        else m_mouseOnText = false;*/

       
        m_mouseOnText = m_owningworld->GetInputController()->IsMouseOver(GetWeakRef());
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
                    if (TestMouseOver(m_locmouse) == true) {
                        ButtonUp();
                    }
                }
                m_touch = false;
            }

#endif        
            Vector2 tPos = { (float)GetMouseX(), (float)GetMouseY() };
            if (IsMouseButtonPressed(0)) 
            {
                if (m_touch == true)return;
                m_touchstartpos = tPos;
                m_touch = true;
            }
            if (IsMouseButtonReleased(0)) 
            {
                if (m_touch == false)return;
                if(m_lines.size() == 0 || m_lines[0].first.length() == 0) return;

                m_touchendpos = tPos;
                if (GetVectorLength(Direction(m_touchendpos, m_touchstartpos)) < 5) {
                    m_curletterindex = m_letterCount;
                    Vector2 fm = MeasureTextEx(*m_sharedfont, "W", (float)m_fontsize * scrnScale, m_fontspacing * scrnScale);
                    int tmpLtrCount = 0;
                    for (auto iter = m_lines.begin(); iter != m_lines.end(); ++iter) {
                            
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
           
            // Set the window's cursor to the I-Beam
            SetMouseCursor(MOUSE_CURSOR_IBEAM);
            if (m_isBackspace) {
                double t = Clock::Get().ElapsedTime();
                if (t - m_keyWaitTimer > m_keydownMulitiplyerDuration) {
                    m_tempkeydownActionDuration = std::max(m_keydownActionMinDuration, m_tempkeydownActionDuration  *m_keydownWaitTimeMultiplyer);
                    Logger::Get()->Trace(std::format("backspace time {} | {}", m_tempkeydownActionDuration, m_keydownMulitiplyerDuration));
                    m_keyWaitTimer = t;
                }
                //float keydownActionMulitiplyer = m_keydownMulitiplyerDuration;
                if (t - m_backspacekeyWaitTimer >= m_tempkeydownActionDuration)
                {
                    if (m_letterCount >= 0) {
                        whilebackspace(m_letterCount, name, m_lines, m_curletterindex);
                        m_text = { name };
                        PrepareInput();
                    }
                    m_backspacekeyWaitTimer = t;
                }

                if (IsKeyUp(KEY_BACKSPACE)) {
                    m_tempkeydownActionDuration = m_keydownMulitiplyerDuration;
                    m_isBackspace = false;
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
                if (IsKeyUp(KEY_LEFT))
                    m_isLeftKey = false;
               
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
                    m_keyWaitTimer = t;
                }
                if (IsKeyUp(KEY_DELETE))
                    m_isDeleteKey = false;
            }
            // Get char pressed (unicode character) on the queue
            int key = GetCharPressed();

            // Check if more characters have been pressed on the same frame
            while (key > 0)
            {           
                int  txtmargin = m_textmargin * scrnScale;
                int tx = textBox.x + 8;
                int ty = textBox.y + 8;
                // NOTE: Only allow keys in range [32..125]
               if ((key >= 32) && (key <= 125) && (m_letterCount < m_charLimit))
                {
                   Vector2 fntmeasure = MeasureTextEx(*m_sharedfont, "W", (float)m_fontsize * scrnScale, m_fontspacing * scrnScale);
                    Rectangle textBox = GetBounds();
                    
                    if (m_curletterindex < m_letterCount) 
                    {
                        typeinbetween(m_letterCount, key, name,  m_lines, m_curletterindex);
                    }
                    else {
                        
                        name[m_letterCount] = (char)key;
                        name[m_letterCount + 1] = '\0'; // Add null terminator at the end of the string.                 
                        m_curletterindex = m_letterCount + 1;
                        if (m_lines.size() == 0) {
                            m_lines.push_back({ std::string{},  { (float)tx, (float)ty } });
                        }
                       
                        m_lines[m_lines.size() - 1].first.append(std::string{ name[m_letterCount] });
            
                        if (MeasureTextEx(*m_sharedfont, m_lines[m_lines.size() - 1].first.c_str(), m_fontsize * scrnScale, m_fontspacing * scrnScale).x >= textBox.width - (fntmeasure.x +  m_textmargin * scrnScale))
                        {
                            m_lines.push_back({ std::string{},  { (float)tx, (float)ty + (fntmeasure.y * m_lines.size()) } });
                 
                        }
                        m_text = { name };
                    }

                    m_isBackspace = false;
                    m_isLeftKey = false;
                    m_isRightKey = false;
                    m_isDeleteKey = false;
                    m_letterCount++;
                }
                key = GetCharPressed();  // Check next character in the queue
               
            }
           
            if (IsKeyPressed(KEY_BACKSPACE))
            {
                if (!m_isBackspace) {
                    m_backspacekeyWaitTimer = Clock::Get().ElapsedTime();
                    m_keyWaitTimer = Clock::Get().ElapsedTime();
                    whilebackspace(m_letterCount, name, m_lines, m_curletterindex);
                    m_text = { name };
                    PrepareInput();
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
        }
        else {
            m_isBackspace = false;
            SetMouseCursor(MOUSE_CURSOR_DEFAULT);
        }

    

        if (m_mouseOnText) m_framesCounter++;
        else m_framesCounter = 0;
	}
#pragma region Keyboard related helper functions

   
#pragma endregion
    void TextInput::Draw(float _deltaTime)
    {
        if (!m_visible)return;
        UIElement::Draw(_deltaTime);
        Rectangle textBox = GetBounds();
        float scrnScale = UICanvas::Get().lock()->Scale();

        // DrawRectangleRec(textBox, LIGHTGRAY);
        if (m_mouseOnText) DrawRectangleLines((int)textBox.x, (int)textBox.y, (int)textBox.width, (int)textBox.height, RED);
        else DrawRectangleLines((int)textBox.x, (int)textBox.y, (int)textBox.width, (int)textBox.height, DARKGRAY);
             
        TextLine(m_lines);             

       // DrawTextEx(*infofnt, TextFormat("INPUT CHARS: %i/%i", m_letterCount, MAX_INPUT_CHARS), { (float)textBox.x , (float)textBox.y + textBox.height }, 14, 1.f, DARKGRAY);


        if (m_mouseOnText || m_hasUpated)
        {
            ProcessInput();

            m_hasUpated = false;
            //if (m_letterCount < m_charLimit)
            //{
            //    // Draw blinking underscore char
            //    Vector2 fntsize = MeasureTextEx(*m_sharedfont, "W", (float)m_fontsize * scrnScale, m_fontspacing * scrnScale);
            //    Vector2 fntmeasure = {0,0};
            //    int chrcount = 0, lc = -1, chrs = 0, s  = 0;
            //    std::string txt = { name };
            //    int txtmargin = m_textmargin * scrnScale;
            //    int fntspace = m_fontspacing * scrnScale;
            //    if (m_curletterindex == txt.size()) {
            //        if (txt.size() > 0) {

            //        fntmeasure = MeasureTextEx(*m_sharedfont, m_lines[m_lines.size() - 1].first.c_str(), (float)m_fontsize * scrnScale, m_fontspacing * scrnScale);
            //            if (((m_framesCounter / 20) % 2) == 0) DrawText("|",  textBox.x + txtmargin + fntmeasure.x + fntspace, m_lines[m_lines.size() - 1].second.y, m_fontsize * scrnScale +2, GRAY);
            //        }
            //        else {
            //            if (((m_framesCounter / 20) % 2) == 0) DrawText("|", 
            //              textBox.x + txtmargin, textBox.y + txtmargin,
            //              m_fontsize * scrnScale + 2, GRAY);
            //        }
            //    }
            //    else {

            //        for (auto iter = m_lines.begin(); iter != m_lines.end(); ++iter) {
            //            ++lc;
            //            if (m_curletterindex == 0)
            //            {
            //                break;
            //            }
            //            chrcount = iter->first.size();   
            //            int rem = m_curletterindex - s;
            //            if (chrcount > rem) {
            //                chrs =  rem;
            //                std::string st = iter->first.substr(0, chrs);
            //         
            //                fntmeasure = MeasureTextEx(*m_sharedfont, st.c_str(), (float)m_fontsize * scrnScale, m_fontspacing * scrnScale);
            //                break;
            //            } 
            //            s += iter->first.size();
            //        }
            //        
            //        if (((m_framesCounter / 10) % 2) == 0) DrawText("|", (float)textBox.x + m_textmargin * scrnScale + fntmeasure.x, m_lines[lc].second.y, m_fontsize * scrnScale +2 , GRAY);
            //    }

            //    
            //}

            //else {
            //  
            //  Vector2 fntmeasure = MeasureTextEx(*m_sharedfont, m_lines[m_lines.size() - 1].first.c_str(), (float)m_fontsize * scrnScale, m_fontspacing * scrnScale);
            //  if (!infofnt)
            //  {
            //      std::string staticassetpath = m_owningworld->GetApplication()->GetStaticAssetsPath();
            //      json& configdata = CARTjson::GetAppData();
            //      std::string fntpath = configdata["cart"]["font"]["verdana"]["path"];
            //      infofnt = AssetManager::Get().LoadFontAsset(std::string{ staticassetpath + fntpath }, 14);
            //  }
            //  DrawTextEx(*infofnt, "Reached chararcter limit. Press BACKSPACE to delete chars...", { (float)textBox.x , (float)textBox.y + textBox.height - fntmeasure.y}, 14, 1.f, DARKGRAY);
            //    //DrawText("|", (float)textBox.x + m_textmargin + fntmeasure.x, m_lines[m_lines.size() - 1].second.y, m_fontsize + 2, RED);
            //};
        }
	}
#pragma endregion

#pragma region  Helper
    void TextInput::TextLine(const std::vector<std::pair<std::string, Vector2>>& str)
    {  
       auto iter = str.begin();
        Rectangle textBox = GetBounds();
        float scrnScale = UICanvas::Get().lock()->Scale();
        int count = 0, l = 0;
        int txtmargin = m_textmargin * scrnScale;
     
        while (iter != str.end())
        {
            if (iter->first.size() > 0)
            {
                m_sharedfont = AssetManager::Get().LoadFontAsset(m_font, m_fontsize * scrnScale);
                DrawTextEx(*m_sharedfont, iter->first.c_str(),iter->second, m_fontsize * scrnScale, m_fontspacing * scrnScale, m_textColor);
              
            }
            count += iter->first.size();
            if (m_curletterindex <= count && m_curletterindex > count - iter->first.size())// currently cursor is somewhere on this line
            {
                //get Single char size
               Vector2 fntsize = MeasureTextEx(*m_sharedfont, "W", (float)m_fontsize * scrnScale, m_fontspacing * scrnScale);
               int sizechr = iter->first.size();
               int stripcharnum = iter->first.size() - (count - m_curletterindex);
                std::string copy = iter->first.substr(0, iter->first.size() - (count - m_curletterindex) + 1) ;// get exact chars to cursor from string
                
                m_cursorLoc = MeasureTextEx(*m_sharedfont, copy.c_str(), (float)m_fontsize * scrnScale, m_fontspacing * scrnScale);// measure width of part of text
                m_cursorLoc.x = textBox.x + m_cursorLoc.x + txtmargin;
                m_cursorLoc.y = textBox.y + l * fntsize.y + m_cursorLoc.y * 0.5f;
            }
            l++;
            ++iter;
        }
       
    }
    void TextInput::SetText(const std::string& txt)
    {
        m_text = txt;
        PrepareInput();
    }
    void TextInput::SetFontName(const std::string& strfnt)
    {
        Text::SetFontName(strfnt);
        PrepareInput();
        m_hasUpated = true;
    }
    void TextInput::SetFontSize(float size)
    {
        Text::SetFontSize(size);
        PrepareInput();
        m_hasUpated = true;
     
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

    void TextInput::PrepareInput()
    {
        float scrnScale = UICanvas::Get().lock()->Scale();
        Rectangle textBox = GetBounds();

        int tx = textBox.x + (m_textmargin * scrnScale);
        int ty = textBox.y + (m_textmargin * scrnScale);

        m_lines.clear();
        m_lines.push_back({ std::string{},  { (float)tx, (float)ty } });

        if (!m_sharedfont)
            m_sharedfont = AssetManager::Get().LoadFontAsset(m_font, m_fontsize * scrnScale);

        if (!infofnt)
        {
            std::string staticassetpath = m_owningworld->GetApplication()->GetStaticAssetsPath();
            json& configdata = CARTjson::GetAppData();
            std::string fntpath = configdata["cart"]["font"]["verdana"]["path"];
            infofnt = AssetManager::Get().LoadFontAsset(std::string{ staticassetpath + fntpath }, 14 * scrnScale);
        }

        Vector2 fntmeasure = MeasureTextEx(*m_sharedfont, "W", (float)m_fontsize * scrnScale, m_fontspacing * scrnScale);

        for (size_t i = 0; i < MAX_INPUT_CHARS; i++)
        {
            name[i] = i < std::min((size_t)m_text.size(), (size_t)m_charLimit) ? (char)m_text.at(i) : '\0';
        }

        m_letterCount = std::min((size_t)m_text.size(), (size_t)m_charLimit);
       // size_t n = (m_text.size() < m_charLimit) ? m_text.size() : m_charLimit;

        for (int i = 0; i < m_letterCount; i++)
        {
            name[i] = m_text.at(i);
            m_lines[m_lines.size() - 1].first.append(std::string{ name[i] });
            if (MeasureTextEx(*m_sharedfont, m_lines[m_lines.size() - 1].first.c_str(), m_fontsize * scrnScale, m_fontspacing * scrnScale).x >= textBox.width - (fntmeasure.x + m_textmargin * scrnScale))
            {
                m_lines.push_back({ std::string{},  {(float)tx , (float)ty + (fntmeasure.y * m_lines.size()) } });

            }
        }

        m_curletterindex = m_letterCount;
       


    }

    void TextInput::ProcessInput()
    {
        int chrcount = 0, lc = -1, chrs = 0, s = 0, txtmargin, fntspace;
        Vector2 fntsize, fntmeasure, cursorloc;
        Rectangle textBox = GetBounds();
        float scrnScale = UICanvas::Get().lock()->Scale();
        fntsize = MeasureTextEx(*m_sharedfont, "W", (float)m_fontsize * scrnScale, m_fontspacing * scrnScale);
        fntmeasure = { 0,0 };
        txtmargin = m_textmargin * scrnScale;
        fntspace = m_fontspacing * scrnScale;
        cursorloc = { textBox.x + txtmargin, textBox.y + txtmargin };
        
        if (!infofnt)
        {
            std::string staticassetpath = m_owningworld->GetApplication()->GetStaticAssetsPath();
            json& configdata = CARTjson::GetAppData();
            std::string fntpath = configdata["cart"]["font"]["verdana"]["path"];
            infofnt = AssetManager::Get().LoadFontAsset(std::string{ staticassetpath + fntpath }, 14);
        }
        if (m_letterCount < m_charLimit)
        {
            // Draw blinking underscore char
            std::string txt = { name };
            if (m_curletterindex < txt.size()) {               
                for (auto iter = m_lines.begin(); iter != m_lines.end(); ++iter) {
                    ++lc;
                    if (m_curletterindex == 0)
                    {
                        break;
                    }
                    chrcount = iter->first.size();
                    int rem = m_curletterindex - s;
                    if (chrcount > rem) {
                        chrs = rem;
                        std::string st = iter->first.substr(0, chrs);

                        fntmeasure = MeasureTextEx(*m_sharedfont, st.c_str(), (float)m_fontsize * scrnScale, m_fontspacing * scrnScale);
                        break;
                    }
                    s += iter->first.size();
                }

            }
        }
        else {
            fntmeasure = MeasureTextEx(*m_sharedfont, m_lines[m_lines.size() - 1].first.c_str(), (float)m_fontsize * scrnScale, m_fontspacing * scrnScale);
           
            DrawTextEx(*infofnt, "Reached chararcter limit. Press BACKSPACE to delete chars...", { (float)textBox.x , (float)textBox.y + textBox.height - fntmeasure.y }, 14, 1.f, DARKGRAY);
        };
        
        fntmeasure = MeasureTextEx(*m_sharedfont, m_lines[m_lines.size() - 1].first.c_str(), (float)m_fontsize * scrnScale, m_fontspacing * scrnScale);
        if (((m_framesCounter / 10) % 2) == 0) DrawText("|", m_cursorLoc.x, m_cursorLoc.y, m_fontsize * scrnScale + 2, GRAY);
    }
    
#pragma endregion


#pragma region Event Listener

    void TextInput::OnScreenSizeChange()
    {
        UpdateLayout();
    }
    void TextInput::OnMobileInput(const char* input)
    {
        int key = GetCharPressed();
        if (IsWindowResized() || key == 0) // Check JS input only if raylib input is empty/generic (code 229)
        {
            
        }
        if ((key >= 32) && (key <= 125) && (m_letterCount < m_charLimit)) {
            m_letterCount = 0;
            std::string mutable_str{ input };
            for (size_t i = 0; i < MAX_INPUT_CHARS; i++)
            {
                name[i] = '\0';
            }
            while (mutable_str.size() > 0) {// transfre all chars to local char array
                std::string achar = mutable_str.substr(0, 1);
                name[m_letterCount] = (char)achar.c_str();
                mutable_str = mutable_str.substr(1);
                m_letterCount++;
            }
        }
       

        if (IsKeyPressed(KEY_BACKSPACE)) {
            if (m_letterCount > 0) {
                m_letterCount--;
                name[m_letterCount] = '\0';
            }
        }
    }
#pragma endregion

}