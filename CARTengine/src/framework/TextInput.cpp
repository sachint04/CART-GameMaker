#include "TextInput.h"
#include "AssetManager.h"
namespace cart
{
#pragma region Construction & Initialization


	TextInput::TextInput(World* _owningworld, const std::string& _id, Vector2 _size)
		:Text{m_owningworld, _id, _size},
		m_charLimit{200},
		m_letterCount{0},
        m_mouseOnText{false},
        m_framesCounter{0}
	{
	}
#pragma endregion

#pragma region Clean Up


	TextInput::~TextInput()
	{
	}
    void TextInput::Destroy() {
        Text::Destroy();
    }
#pragma endregion

#pragma region  LOOP

	void TextInput::Update(float _deltaTime)
	{

        if (CheckCollisionPointRec(GetMousePosition(), GetBounds())) m_mouseOnText = true;
        else m_mouseOnText = false;

        if (m_mouseOnText)
        {
            // Set the window's cursor to the I-Beam
            SetMouseCursor(MOUSE_CURSOR_IBEAM);

            // Get char pressed (unicode character) on the queue
            int key = GetCharPressed();

            // Check if more characters have been pressed on the same frame
            while (key > 0)
            {
                // NOTE: Only allow keys in range [32..125]
                if ((key >= 32) && (key <= 125) && (m_letterCount < m_charLimit))
                {
                    name[m_letterCount] = (char)key;
                    name[m_letterCount + 1] = '\0'; // Add null terminator at the end of the string.
                    m_letterCount++;
                }

                key = GetCharPressed();  // Check next character in the queue
            }

            if (IsKeyPressed(KEY_BACKSPACE))
            {
                m_letterCount--;
                if (m_letterCount < 0) m_letterCount = 0;
                name[m_letterCount] = '\0';
            }
        }
        else SetMouseCursor(MOUSE_CURSOR_DEFAULT);

        if (m_mouseOnText) m_framesCounter++;
        else m_framesCounter = 0;
	}
	void TextInput::Draw(float _deltaTime)
	{

        DrawText("PLACE MOUSE OVER INPUT BOX!", 240, 140, 20, GRAY);
        Rectangle textBox = GetBounds();
        DrawRectangleRec(textBox, LIGHTGRAY);
        if (m_mouseOnText) DrawRectangleLines((int)textBox.x, (int)textBox.y, (int)textBox.width, (int)textBox.height, RED);
        else DrawRectangleLines((int)textBox.x, (int)textBox.y, (int)textBox.width, (int)textBox.height, DARKGRAY);
        shared<Font> fnt = AssetManager::Get().LoadFontAsset(m_font, m_fontsize);
        DrawTextEx(*fnt, name, { (float)textBox.x + 5, (float)textBox.y + 8 }, m_fontsize, m_fontspacing, BLACK);

        DrawText(TextFormat("INPUT CHARS: %i/%i", m_letterCount, MAX_INPUT_CHARS), 315, 250, m_fontsize, DARKGRAY);

        if (m_mouseOnText)
        {
            if (m_letterCount < MAX_INPUT_CHARS)
            {
                // Draw blinking underscore char
                Vector2 txtsize = MeasureTextEx(*fnt, name, (float)m_fontsize, m_fontspacing);
                if (((m_framesCounter / 20) % 2) == 0) DrawText("_", (int)textBox.x + txtsize.x, (int)textBox.y + 10, m_fontsize, BLACK);

            }
            else DrawText("Press BACKSPACE to delete chars...", 230, 300, 20, GRAY);
        }
	}
#pragma endregion

}