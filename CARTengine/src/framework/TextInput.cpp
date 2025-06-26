#include "TextInput.h"
#include <stdexcept>
#include "AssetManager.h"
#include "Clock.h"
#include "MathUtility.h"
#include "Application.h"
#include "World.h"
#include "CARTjson.h"
#include "component/InputController.h"
namespace cart
{
#pragma region Construction & Initialization


    TextInput::TextInput(World* _owningworld, const std::string& _id, Vector2 _size)
        :Text{ _owningworld, _id, _size },
        m_charLimit{ 200 },
        m_letterCount{ 0 },
        m_mouseOnText{ false },
        m_framesCounter{ 0 },
        //    lines{  },
        m_isBackspace{ false },
        m_keyWaitTimer{ 0 },
        m_backspacekeyWaitTimer{ 0 },
        m_textmargin{5},
        m_touch{false},
        m_touchstartpos{},
        m_touchendpos{},
        m_lines{},
        m_curletterindex{0},
        m_lrange{},
        m_isLeftKey{false},
        m_isRightKey{ false },
        m_isDeleteKey{ false }
	{
        Text::m_fontsize = 14.f;
        Text::m_fontspacing = 2.f;
	}

    void TextInput::Init()
    {
        Text::Init();
        m_owningworld->GetInputController()->RegisterUI(GetWeakRef());

        Rectangle textBox = GetBounds();
        m_lines.push_back({ std::string{},  { (float)textBox.x + m_textmargin, (float)textBox.y + 8 } });

        m_lrange.push_back({ {0,0}, { (float)textBox.x + m_textmargin, (float)textBox.y + 8 } });
        json& data = CARTjson::GetAppData();
        std::string strfont = data["cart"]["font"]["verdana"]["path"];
        infofnt = AssetManager::Get().LoadFontAsset(strfont, 14);
        Vector2 fntmeasure = MeasureTextEx(*m_sharedfont, "W", GetFontSize(), 2.f);

        size_t n = (m_text.size() < m_charLimit) ? m_text.size() : m_charLimit;

        for (int i = 0; i < n ; i++)
        {
            name[i] = m_text.at(i);
            m_lines[m_lines.size() - 1].first.append(std::string{ name[i] });
            if (MeasureTextEx(*m_sharedfont, m_lines[m_lines.size() - 1].first.c_str(), GetFontSize(), 2.f).x >= textBox.width - (fntmeasure.x + m_textmargin))
            {
                m_lines.push_back({ std::string{},  { (float)textBox.x + m_textmargin, ((float)textBox.y + 8) + (fntmeasure.y * m_lines.size() - 1) } });            
                
                m_lrange[m_lrange.size() - 1].first.second = i;
                std::pair<int, int> p = { (i + 1),0 };
                std::pair<std::pair<int, int>, Vector2> pp = { p, { (float)textBox.x + m_textmargin, ((float)textBox.y + 8) + (fntmeasure.y * m_lrange.size() - 1) } };
                m_lrange.push_back( pp);
            }
        }
        name[n] = '\0';
        m_lrange[m_lrange.size() - 1].first.second = n;
        m_letterCount = n;
        m_curletterindex = n;
    }
#pragma endregion

#pragma region Clean Up


	TextInput::~TextInput()
	{
        

        infofnt.reset();
	}
    void TextInput::Destroy() {
        Text::Destroy();
    }

    
   
#pragma endregion

#pragma region  LOOP

	void TextInput::Update(float _deltaTime)
	{
        if (!m_visible)return;
        Rectangle textBox = GetBounds();

        auto typeinbetween = [&](int& c, int key, char* s, std::vector<std::pair<std::pair<int, int>, Vector2>>& r, std::vector<std::pair<std::string, Vector2>>& l, int& d)
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
            l.push_back({ std::string{},  { (float)textBox.x + m_textmargin, (float)textBox.y + 8 } });

            r.clear();
            r.push_back({ {0,0}, { (float)textBox.x + m_textmargin, (float)textBox.y + 8 } });

            Vector2 fntmeasure = MeasureTextEx(*m_sharedfont, "W", (float)m_fontsize, m_fontspacing);
            for (int i = 0; i < c; i++)
            {

                l[l.size() - 1].first.append(std::string{ s[i] });
                if (MeasureTextEx(*m_sharedfont, l[l.size() - 1].first.c_str(), (float)m_fontsize, m_fontspacing).x >= textBox.width - (fntmeasure.x + m_textmargin))
                {
                    l.push_back({ std::string{},  { (float)textBox.x + m_textmargin, ((float)textBox.y + 8) + (fntmeasure.y * l.size() - 1) } });
                    r[r.size() - 1].first.second = i;
                    std::pair<int, int> p = { (i + 1),0 };
                    std::pair<std::pair<int, int>, Vector2> pp = { p, { (float)textBox.x + m_textmargin, ((float)textBox.y + 8) + (fntmeasure.y * r.size() - 1) } };
                    r.push_back(pp);
                }
            }
            r[r.size() - 1].first.second = c;
        };

        auto whilebackspace = [&](int &c, char * s, std::vector<std::pair<std::pair<int, int>, Vector2>>& r, std::vector<std::pair<std::string, Vector2>>& l, int &d)
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
            l.push_back({ std::string{},  { (float)textBox.x + m_textmargin, (float)textBox.y + 8 } });
           
            r.clear();
            r.push_back({ {0,0}, { (float)textBox.x + m_textmargin, (float)textBox.y + 8 } });

            Vector2 fntmeasure = MeasureTextEx(*m_sharedfont, "W", (float)m_fontsize, m_fontspacing);
            for (int i = 0; i < c; i++)
            {
 
                l[l.size() - 1].first.append(std::string{ s[i] });
                if (MeasureTextEx(*m_sharedfont, l[l.size() - 1].first.c_str(), (float)m_fontsize, m_fontspacing).x >= textBox.width - (fntmeasure.x + m_textmargin))
                {
                    l.push_back({ std::string{},  { (float)textBox.x + m_textmargin, ((float)textBox.y + 8) + (fntmeasure.y * m_lines.size() - 1) } });
                    r[r.size() - 1].first.second = i;
                    std::pair<int, int> p = { (i + 1),0 };
                    std::pair<std::pair<int, int>, Vector2> pp = { p, { (float)textBox.x + m_textmargin, ((float)textBox.y + 8) + (fntmeasure.y * m_lrange.size() - 1) } };
                    r.push_back(pp);
                }
            }
            r[r.size() - 1].first.second = c;
        };

        auto whileleftkey = [&](int& d) {
            d--;
            if (d < 0)d = 0;
        };
        
        auto whilerightkey = [&](int& d, const int & c) {
            d++;
            if (d > c)d = c;
        };

        auto whiledeletekey = [&](int& c, char* s, std::vector<std::pair<std::pair<int, int>, Vector2>>& r, std::vector<std::pair<std::string, Vector2>>& l, int& d)
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
            l.push_back({ std::string{},  { (float)textBox.x + m_textmargin, (float)textBox.y + 8 } });

            r.clear();
            r.push_back({ {0,0}, { (float)textBox.x + m_textmargin, (float)textBox.y + 8 } });

            Vector2 fntmeasure = MeasureTextEx(*m_sharedfont, "W", (float)m_fontsize, m_fontspacing);
            for (int i = 0; i < c; i++)
            {

                l[l.size() - 1].first.append(std::string{ s[i] });
                if (MeasureTextEx(*m_sharedfont, l[l.size() - 1].first.c_str(), (float)m_fontsize, m_fontspacing).x >= textBox.width - (fntmeasure.x + m_textmargin))
                {
                    l.push_back({ std::string{},  { (float)textBox.x + m_textmargin, ((float)textBox.y + 8) + (fntmeasure.y * l.size() - 1) } });
                    r[r.size() - 1].first.second = i;
                    std::pair<int, int> p = { (i + 1),0 };
                    std::pair<std::pair<int, int>, Vector2> pp = { p, { (float)textBox.x + m_textmargin, ((float)textBox.y + 8) + (fntmeasure.y * r.size() - 1) } };
                    r.push_back(pp);
                }
            }
            r[r.size() - 1].first.second = c;
        };

      /*  if (CheckCollisionPointRec(GetMousePosition(), GetBounds())) m_mouseOnText = true;
        else m_mouseOnText = false;*/

       
        m_mouseOnText = m_owningworld->GetInputController()->IsMouseOver(GetWeakRef());
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
                    if (TestMouseOver(m_locmouse) == true) {
                        ButtonUp();
                    }
                }
                m_touch = false;
            }

#else          
            Vector2 tPos = { (float)GetMouseX(), (float)GetMouseY() };
            if (IsMouseButtonPressed(0)) {
                if (m_touch == true)return;
                m_touchstartpos = tPos;
                m_touch = true;
            }
            if (IsMouseButtonReleased(0)) {
                if (m_touch == false)return;
                if(m_lines[0].first.length() == 0) return;

                m_touchendpos = tPos;
                if (GetVectorLength(Direction(m_touchendpos, m_touchstartpos)) < 5) {
                    m_curletterindex = m_letterCount;
                    Vector2 fm = MeasureTextEx(*m_sharedfont, "W", (float)m_fontsize, m_fontspacing);
                    int tmpLtrCount = 0;
                    for (auto iter = m_lines.begin(); iter != m_lines.end(); ++iter) {
                            
                       if (m_touchendpos.y > iter->second.y && m_touchendpos.y <= iter->second.y + fm.y) 
                        {
                            int chr = 0;
                            while (chr < iter->first.length() - 1)
                            {                           
                                std::string nr = iter->first.substr(0, chr);                             
                                if (iter->second.x + MeasureTextEx(*m_sharedfont, nr.c_str(), (float)m_fontsize, m_fontspacing).x >= m_touchendpos.x)
                                {
                                    m_curletterindex = tmpLtrCount + nr.size();
                                   //LOG("Cursor at letter count %d\n  word %s\n", m_letterCount, nr.c_str());
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
#endif

            if (m_isBackspace) {
                double t = Clock::Get().ElapsedTime();
                if (t - m_backspacekeyWaitTimer >= 0.1f)
                {
                    if (m_letterCount >= 0) {
                        whilebackspace(m_letterCount, name, m_lrange, m_lines, m_curletterindex);
                    }
                    m_backspacekeyWaitTimer = t;
                }

              if (IsKeyUp(KEY_BACKSPACE))                    
                m_isBackspace = false;
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
                    whiledeletekey(m_letterCount, name, m_lrange, m_lines, m_curletterindex);
                    m_keyWaitTimer = t;
                }
                if (IsKeyUp(KEY_DELETE))
                    m_isDeleteKey = false;
            }

            // Set the window's cursor to the I-Beam
            SetMouseCursor(MOUSE_CURSOR_IBEAM);

            // Get char pressed (unicode character) on the queue
            int key = GetCharPressed();

            // Check if more characters have been pressed on the same frame
            while (key > 0 )
            {                          
                // NOTE: Only allow keys in range [32..125]
               if ((key >= 32) && (key <= 125) && (m_letterCount < m_charLimit))
                {
                   Vector2 fntmeasure = MeasureTextEx(*m_sharedfont, "W", (float)m_fontsize, m_fontspacing);
                    Rectangle textBox = GetBounds();
                    if (m_curletterindex < m_letterCount) 
                    {
                        typeinbetween(m_letterCount, key, name, m_lrange, m_lines, m_curletterindex);
                    }
                    else {

                        name[m_letterCount] = (char)key;
                        name[m_letterCount + 1] = '\0'; // Add null terminator at the end of the string.                 
                        m_curletterindex = m_letterCount + 1;
                        m_lines[m_lines.size() - 1].first.append(std::string{ name[m_letterCount] });
            
                        if (MeasureTextEx(*m_sharedfont, m_lines[m_lines.size() - 1].first.c_str(), (float)m_fontsize, m_fontspacing).x >= textBox.width - (fntmeasure.x +  m_textmargin))
                        {
                            m_lines.push_back({ std::string{},  { (float)textBox.x + m_textmargin, ((float)textBox.y + 8) + (fntmeasure.y * m_lines.size() - 1) } });
                 
                        }
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
                    whilebackspace(m_letterCount, name, m_lrange, m_lines, m_curletterindex);
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
                whiledeletekey(m_letterCount, name, m_lrange, m_lines, m_curletterindex);
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
	void TextInput::Draw(float _deltaTime)
	{
        if (!m_visible)return;
        Rectangle textBox = GetBounds();
       // DrawRectangleRec(textBox, LIGHTGRAY);
        if (m_mouseOnText) DrawRectangleLines((int)textBox.x, (int)textBox.y, (int)textBox.width, (int)textBox.height, RED);
        else DrawRectangleLines((int)textBox.x, (int)textBox.y, (int)textBox.width, (int)textBox.height, DARKGRAY);
        TextLine(m_lines);             

        DrawTextEx(*infofnt, TextFormat("INPUT CHARS: %i/%i", m_letterCount, MAX_INPUT_CHARS), { (float)textBox.x , (float)textBox.y + textBox.height }, 14, 1.f, DARKGRAY);


        if (m_mouseOnText)
        {
            if (m_letterCount < MAX_INPUT_CHARS)
            {
                // Draw blinking underscore char
                Vector2 fntsize = MeasureTextEx(*m_sharedfont, "W", (float)m_fontsize, m_fontspacing);
                Vector2 fntmeasure = {0,0};
                int chrcount = 0, lc = -1, chrs = 0, s  = 0;
                if (m_curletterindex == std::string{ name }.size()) {
                    fntmeasure = MeasureTextEx(*m_sharedfont, m_lines[m_lines.size() - 1].first.c_str(), (float)m_fontsize, m_fontspacing);
                    if (((m_framesCounter / 20) % 2) == 0) DrawText("|",  (float)textBox.x + m_textmargin + fntmeasure.x , m_lines[m_lines.size() - 1].second.y, m_fontsize +2, GRAY);
                }
                else {

                    for (auto iter = m_lines.begin(); iter != m_lines.end(); ++iter) {
                        ++lc;
                        if (m_curletterindex == 0)
                        {
                            break;
                        }
                        chrcount = iter->first.size();   
                        int rem = m_curletterindex - s;
                        if (chrcount > rem) {
                            chrs =  rem;
                            std::string st = iter->first.substr(0, chrs);
                         //   LOG("chars in line %d", chrcount);
                       //     LOG("Cursor moved to row %d col %d char %s\n", lc, chrs, st.c_str());
                            fntmeasure = MeasureTextEx(*m_sharedfont, st.c_str(), (float)m_fontsize, m_fontspacing);
                            break;
                        } 
                        s += iter->first.size();
                    }
                    
                    if (((m_framesCounter / 10) % 2) == 0) DrawText("|", (float)textBox.x + m_textmargin + fntmeasure.x, m_lines[lc].second.y, m_fontsize +2 , GRAY);
                }

                
            }

            else {
              
              Vector2 fntmeasure = MeasureTextEx(*m_sharedfont, m_lines[m_lines.size() - 1].first.c_str(), (float)m_fontsize, m_fontspacing);
                DrawTextEx(*infofnt, "Reached chararcter limit. Press BACKSPACE to delete chars...", { (float)textBox.x , (float)textBox.y + textBox.height - fntmeasure.y}, 14, 1.f, DARKGRAY);
                //DrawText("|", (float)textBox.x + m_textmargin + fntmeasure.x, m_lines[m_lines.size() - 1].second.y, m_fontsize + 2, RED);
            };
        }
	}
#pragma endregion


#pragma region  Helper
    void TextInput::TextLine(const std::vector<std::pair<std::string, Vector2>>& str)
    {
       
       auto iter = str.begin();
        Rectangle textBox = GetBounds();
        while (iter != str.end())
        {
            if (iter->first.size() > 0)
            {
                m_sharedfont = AssetManager::Get().LoadFontAsset(m_font, m_fontsize);
                DrawTextEx(*m_sharedfont, iter->first.c_str(),iter->second, m_fontsize, m_fontspacing, m_textColor);
              
            }
            ++iter;
        }

    }

    void TextInput::SetText(const std::string& txt)
    {
        Rectangle textBox = GetBounds();
        m_lines.clear();
        m_lrange.clear();


        m_lines.push_back({ std::string{},  { (float)textBox.x + m_textmargin, (float)textBox.y + 8 } });
        m_lrange.push_back({ {0,0}, { (float)textBox.x + m_textmargin, (float)textBox.y + 8 } });

        if(!m_sharedfont)
            m_sharedfont = AssetManager::Get().LoadFontAsset(m_font, m_fontsize);

        if(!infofnt)
        infofnt = AssetManager::Get().LoadFontAsset(CARTjson::GetAppData()["cart"]["font"]["verdana"]["path"], 14);
        
        Vector2 fntmeasure = MeasureTextEx(*m_sharedfont, "W", m_fontsize, m_fontspacing);


        m_text = txt;
        size_t n = (m_text.size() < m_charLimit) ? m_text.size() : m_charLimit;

        for (int i = 0; i < n; i++)
        {
            name[i] = m_text.at(i);
            m_lines[m_lines.size() - 1].first.append(std::string{ name[i] });
            if (MeasureTextEx(*m_sharedfont, m_lines[m_lines.size() - 1].first.c_str(), (float)m_fontsize, m_fontspacing).x >= textBox.width - (fntmeasure.x + m_textmargin))
            {
                m_lines.push_back({ std::string{},  { (float)textBox.x + m_textmargin, ((float)textBox.y + 8) + (fntmeasure.y * m_lines.size() - 1) } });

                m_lrange[m_lrange.size() - 1].first.second = i;
                std::pair<int, int> p = { (i + 1),0 };
                std::pair<std::pair<int, int>, Vector2> pp = { p, { (float)textBox.x + m_textmargin, ((float)textBox.y + 8) + (fntmeasure.y * m_lrange.size() - 1) } };
                m_lrange.push_back(pp);
            }
        }
        name[n] = '\0';
        m_lrange[m_lrange.size() - 1].first.second = n;
        m_letterCount = n;
        m_curletterindex = n;
    }
    void TextInput::SetFontName(const std::string& strfnt)
    {
        Text::SetFontName(strfnt);
        
        //  int &c, char * s, std::vector<std::pair<std::pair<int, int>, Vector2>>& r, std::vector<std::pair<std::string, Vector2>>& l, int &d
  //    m_letterCount, name, m_lrange, m_lines, m_curletterindex
        Rectangle textBox = GetBounds();
        m_lines.clear();
        m_lines.push_back({ std::string{},  { (float)textBox.x + m_textmargin, (float)textBox.y + 8 } });

        m_lrange.clear();
        m_lrange.push_back({ {0,0}, { (float)textBox.x + m_textmargin, (float)textBox.y + 8 } });

        Vector2 fntmeasure = MeasureTextEx(*m_sharedfont, "W", (float)m_fontsize, m_fontspacing);
        for (int i = 0; i < m_letterCount; i++)
        {

            m_lines[m_lines.size() - 1].first.append(std::string{ name[i] });
            if (MeasureTextEx(*m_sharedfont, m_lines[m_lines.size() - 1].first.c_str(), (float)m_fontsize, m_fontspacing).x >= textBox.width - (fntmeasure.x + m_textmargin))
            {
                m_lines.push_back({ std::string{},  { (float)textBox.x + m_textmargin, ((float)textBox.y + 8) + (fntmeasure.y * m_lines.size() - 1) } });
                m_lrange[m_lrange.size() - 1].first.second = i;
                std::pair<int, int> p = { (i + 1),0 };
                std::pair<std::pair<int, int>, Vector2> pp = { p, { (float)textBox.x + m_textmargin, ((float)textBox.y + 8) + (fntmeasure.y * m_lrange.size() - 1) } };
                m_lrange.push_back(pp);
            }
        }
        m_lrange[m_lrange.size() - 1].first.second = m_letterCount;
    }
    void TextInput::SetFontSize(float size)
    {
        Text::SetFontSize(size);
     
        //  int &c, char * s, std::vector<std::pair<std::pair<int, int>, Vector2>>& r, std::vector<std::pair<std::string, Vector2>>& l, int &d
    //    m_letterCount, name, m_lrange, m_lines, m_curletterindex
        Rectangle textBox = GetBounds();
        m_lines.clear();
        m_lines.push_back({ std::string{},  { (float)textBox.x + m_textmargin, (float)textBox.y + 8 } });

        m_lrange.clear();
        m_lrange.push_back({ {0,0}, { (float)textBox.x + m_textmargin, (float)textBox.y + 8 } });

        Vector2 fntmeasure = MeasureTextEx(*m_sharedfont, "W", (float)m_fontsize, m_fontspacing);
        for (int i = 0; i < m_letterCount; i++)
        {

            m_lines[m_lines.size() - 1].first.append(std::string{ name[i] });
            if (MeasureTextEx(*m_sharedfont, m_lines[m_lines.size() - 1].first.c_str(), (float)m_fontsize, m_fontspacing).x >= textBox.width - (fntmeasure.x + m_textmargin))
            {
                m_lines.push_back({ std::string{},  { (float)textBox.x + m_textmargin, ((float)textBox.y + 8) + (fntmeasure.y * m_lines.size() - 1) } });
                m_lrange[m_lrange.size() - 1].first.second = i;
                std::pair<int, int> p = { (i + 1),0 };
                std::pair<std::pair<int, int>, Vector2> pp = { p, { (float)textBox.x + m_textmargin, ((float)textBox.y + 8) + (fntmeasure.y * m_lrange.size() - 1) } };
                m_lrange.push_back(pp);
            }
        }
        m_lrange[m_lrange.size() - 1].first.second = m_letterCount;
    }
#pragma endregion

}