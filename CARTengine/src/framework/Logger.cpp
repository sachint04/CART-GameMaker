#include "Logger.h"
#include "Core.h"
#include <raylib.h>
namespace cart
{
    Logger* Logger::instance = nullptr;

#pragma region INIT

    Logger::Logger()
        :isVisible{ true }, logdb{}
    {
        font = GetFontDefault();
    }
#pragma endregion

#pragma region Helpers


    void Logger::Show()
	{
        isVisible = true;
	}

	void Logger::Hide()
	{
        isVisible = false;
	}

    void Logger::Clear()
    {
        logdb.clear();
    }

    void Logger::Push(std::string _t, LOG_TYPE _type)
    {
        if (isVisible) 
        {
            logdb.push_back({ _type, _t });
        }
       
        printf("%s", _t.c_str());
        
   }

    void Logger::SetRect(Rectangle _rect)
    {
        container = _rect;
        resizer.x = container.x + container.width - 17;
        resizer.y = container.y + container.height - 17;

        clearbtn.x = container.x + container.width - 17;
        clearbtn.y = container.y + 2;
    }

    void Logger::SetMaxLogCount(int _lines)
    {
        m_max_log_count = _lines;
    }

    void Logger::Drag()
    {
    }
#pragma endregion

#pragma region CleanUP


    Logger::~Logger()
    {
        std::cout << "Logger Destroyed" << std::endl;
    }

    Logger* Logger::Get() {
        if (instance == nullptr) {
            
            instance = new Logger{};
        }
        return instance;
    }
#pragma endregion

#pragma region LOOP
	void Logger::Update(float _deltaTime)
	{


        if (!isVisible)return;
       
        if (IsKeyPressed(KEY_SPACE)) wordWrap = !wordWrap;
        Vector2 mouse = GetMousePosition();
      
        // Check if the mouse is inside the container and toggle border color
        if (CheckCollisionPointRec(mouse, container)) borderColor = Fade(MAROON, 0.4f);
        else if (!resizing) borderColor = MAROON;
        
        Rectangle titlebarRect = { container.x, container.y - 20, container.width, 20.f };
        Vector2 draggingoffset = { 0 };
        // Container resizing logic
        if (resizing)
        {
            if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
                resizing = false; m_ismouseOverUI = false;
            }

            float width = container.width + (mouse.x - lastMouse.x);
            container.width = (width > minWidth) ? ((width < maxWidth) ? width : maxWidth) : minWidth;

            float height = container.height + (mouse.y - lastMouse.y);
            container.height = (height > minHeight) ? ((height < maxHeight) ? height : maxHeight) : minHeight;
        }
        else if (dragging) 
        {
            if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
                dragging = false; m_ismouseOverUI = false;
            }
            draggingoffset = { mouse.x - lastMouse.x, mouse.y - lastMouse.y };
            container.x = container.x + draggingoffset.x;
            container.y = container.y + draggingoffset.y;
        }
        else
        {
            // Check if we're resizing

            if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) { // Mouse Release
                if (CheckCollisionPointRec(mouse, clearbtn))
                {
                    Clear();
                    m_ismouseOverUI = true;
                }
            }

            if (IsMouseButtonDown(MOUSE_BUTTON_LEFT))// Mouse down
            {
                if (CheckCollisionPointRec(mouse, resizer)) {
                    {
                        resizing = true;
                    }
                    m_ismouseOverUI = true;
                }
                else if (CheckCollisionPointRec(mouse, titlebarRect))
                {
                    dragging = true;
                    m_ismouseOverUI = true;
                }
                else {
                    m_ismouseOverUI = false;
                }
            }

            if (CheckCollisionPointRec(mouse, container)) {

                m_ismouseOverUI = true;
            }
        }
        clearbtn.x = container.x + container.width - 17;
        clearbtn.y = container.y + 2;

        // Move resizer rectangle properly
        resizer.x = container.x + container.width - 17;
        resizer.y = container.y + container.height - 17;

        lastMouse = mouse; // Update mouse
	}

    void Logger::Draw(float _deltaTime) {

        if (!isVisible)return;

        auto checkrect = [&](Vector2& rect, const std::string& t, float& fontsize, float spacing )
        {            
            rect = MeasureTextEx(GetFontDefault(), t.c_str(), fontsize, spacing);
        };
        DrawRectangleLinesEx(container, 3, borderColor);    // Draw container border
        


        DrawRectangle(container.x, container.y , container.width, container.height, RAYWHITE);
        std::string  info_str = { " SHOWING LAST " + std::to_string(m_max_log_count) + " LOGS." };
        DrawText(info_str.c_str(), container.x, container.y - 15, 10, WHITE);
        std::string t = "";

        int counter = 0;
        int pos = container.y;
        // Draw text in container (add some padding)
        int size = logdb.size();
        int offset = std::max(size, size - m_max_log_count);
        if (offset > 0) {

            for (auto iter = logdb.end() - offset;  iter != logdb.end(); ++iter)
            {        
                t +=  iter->log + "\n"  ;  
                Vector2 rect = {};
                checkrect(rect, iter->log.c_str(), fontsize, 2.f);
                if (pos > container.y + container.height - 50)
                {
                    break;
                }
                DrawTextBoxed(font, iter->log.c_str(), { container.x + 4, (float)pos + 4, container.width, rect.y + 4 }, fontsize, 2.0f, wordWrap, BLACK);
                pos += rect.y; 
            }
        }
       
        DrawRectangleRec(resizer, borderColor);             // Draw the resize box
        DrawRectangleRec(clearbtn, borderColor);// clear Text box
        DrawRectangle(container.x, container.y - 20, container.width, 20, borderColor);// draw Title bar
        
    }
#pragma endregion

#pragma region DrawText
    // Draw text using font inside rectangle limits
    void Logger::DrawTextBoxed(Font font, const char* text, Rectangle rec, float fontSize, float spacing, bool wordWrap, Color tint)
    {
        DrawTextBoxedSelectable(font, text, rec, fontSize, spacing, wordWrap, tint, 0, 0, WHITE, WHITE);
    }


    void Logger::DrawTextBoxedSelectable(Font font, const char* text, Rectangle rec, float fontSize, float spacing, bool wordWrap, Color tint, int selectStart, int selectLength, Color selectTint, Color selectBackTint)
    {
        int length = TextLength(text);  // Total length in bytes of the text, scanned by codepoints in loop

        float textOffsetY = 0;          // Offset between lines (on line break '\n')
        float textOffsetX = 0.0f;       // Offset X to next character to draw

        float scaleFactor = fontSize / (float)font.baseSize;     // Character rectangle scaling factor

        // Word/character wrapping mechanism variables
        enum { MEASURE_STATE = 0, DRAW_STATE = 1 };
        int state = wordWrap ? MEASURE_STATE : DRAW_STATE;

        int startLine = -1;         // Index where to begin drawing (where a line begins)
        int endLine = -1;           // Index where to stop drawing (where a line ends)
        int lastk = -1;             // Holds last value of the character position

        for (int i = 0, k = 0; i < length; i++, k++)
        {
            // Get next codepoint from byte string and glyph index in font
            int codepointByteCount = 0;
            int codepoint = GetCodepoint(&text[i], &codepointByteCount);
            int index = GetGlyphIndex(font, codepoint);

            // NOTE: Normally we exit the decoding sequence as soon as a bad byte is found (and return 0x3f)
            // but we need to draw all of the bad bytes using the '?' symbol moving one byte
            if (codepoint == 0x3f) codepointByteCount = 1;
            i += (codepointByteCount - 1);

            float glyphWidth = 0;
            if (codepoint != '\n')
            {
                glyphWidth = (font.glyphs[index].advanceX == 0) ? font.recs[index].width * scaleFactor : font.glyphs[index].advanceX * scaleFactor;

                if (i + 1 < length) glyphWidth = glyphWidth + spacing;
            }

            // NOTE: When wordWrap is ON we first measure how much of the text we can draw before going outside of the rec container
            // We store this info in startLine and endLine, then we change states, draw the text between those two variables
            // and change states again and again recursively until the end of the text (or until we get outside of the container)
            // When wordWrap is OFF we don't need the measure state so we go to the drawing state immediately
            // and begin drawing on the next line before we can get outside the container
            if (state == MEASURE_STATE)
            {
                // TODO: There are multiple types of spaces in UNICODE, maybe it's a good idea to add support for more
                // Ref: http://jkorpela.fi/chars/spaces.html
                if ((codepoint == ' ') || (codepoint == '\t') || (codepoint == '\n')) endLine = i;

                if ((textOffsetX + glyphWidth) > rec.width)
                {
                    endLine = (endLine < 1) ? i : endLine;
                    if (i == endLine) endLine -= codepointByteCount;
                    if ((startLine + codepointByteCount) == endLine) endLine = (i - codepointByteCount);

                    state = !state;
                }
                else if ((i + 1) == length)
                {
                    endLine = i;
                    state = !state;
                }
                else if (codepoint == '\n') state = !state;

                if (state == DRAW_STATE)
                {
                    textOffsetX = 0;
                    i = startLine;
                    glyphWidth = 0;

                    // Save character position when we switch states
                    int tmp = lastk;
                    lastk = k - 1;
                    k = tmp;
                }
            }
            else
            {
                if (codepoint == '\n')
                {
                    if (!wordWrap)
                    {
                        textOffsetY += (font.baseSize + font.baseSize / 2) * scaleFactor;
                        textOffsetX = 0;
                    }
                }
                else
                {
                    if (!wordWrap && ((textOffsetX + glyphWidth) > rec.width))
                    {
                        textOffsetY += (font.baseSize + font.baseSize / 2) * scaleFactor;
                        textOffsetX = 0;
                    }

                    // When text overflows rectangle height limit, just stop drawing
                    if ((textOffsetY + font.baseSize * scaleFactor) > rec.height) break;

                    // Draw selection background
                    bool isGlyphSelected = false;
                    if ((selectStart >= 0) && (k >= selectStart) && (k < (selectStart + selectLength)))
                    {
                        DrawRectangleRec( { rec.x + textOffsetX - 1, rec.y + textOffsetY, glyphWidth, (float)font.baseSize* scaleFactor }, selectBackTint);
                        isGlyphSelected = true;
                    }

                    // Draw current character glyph
                    if ((codepoint != ' ') && (codepoint != '\t'))
                    {
                        DrawTextCodepoint(font, codepoint,  { rec.x + textOffsetX, rec.y + textOffsetY }, fontSize, isGlyphSelected ? selectTint : tint);
                    }
                }

                if (wordWrap && (i == endLine))
                {
                    textOffsetY += (font.baseSize + font.baseSize / 2) * scaleFactor;
                    textOffsetX = 0;
                    startLine = endLine;
                    endLine = -1;
                    glyphWidth = 0;
                    selectStart += lastk - k;
                    k = lastk;

                    state = !state;
                }
            }

            if ((textOffsetX != 0) || (codepoint != ' ')) textOffsetX += glyphWidth;  // avoid leading spaces
        }
    }
#pragma endregion
}