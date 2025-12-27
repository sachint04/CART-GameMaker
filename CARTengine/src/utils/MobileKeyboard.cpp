#include "utils/MobileKeyboard.h"
#ifdef  __EMSCRIPTEN__
    #include <emscripten.h>
#endif //  __EMSCRIPTEN__

#include <vector>

extern "C"
{
#ifdef  __EMSCRIPTEN__


    EM_JS(int, JS_GetKeyboardStatus, (), {
        return _JS_MobileKeyboard_GetKeyboardStatus();
    });

    EM_JS(int, JS_GetIgnoreBlurEvent, (), {
        return _JS_MobileKeybard_GetIgnoreBlurEvent();
    });

    EM_JS(int, JS_GetTextLength, (), {
        return _JS_MobileKeyboard_GetText(0, 0);
    });

    EM_JS(void, JS_GetText, (char* buffer, int bufferSize), {
        _JS_MobileKeyboard_GetText(buffer, bufferSize);
    });

    EM_JS(void, JS_GetSelection, (int* start, int* length), {
        _JS_MobileKeyboard_GetTextSelection(start, length);
    });

    EM_JS(void, JS_Touch_Start,
        (const char* text,
         int keyboardType,
         int autocorrect,
         int multiline,
         int secure,
         int alert,
         const char* placeholder,
         int charLimit), {

        _JS_MobileKeyboard_Start(
            text,
            keyboardType,
            autocorrect,
            multiline,
            secure,
            alert,
            placeholder,
            charLimit
        );
    });

    EM_JS(void, JS_InteruptTouch, (), {
        _JS_MobileKeyboard_InteruptTouch();
    });

    EM_JS(void, JS_HideKeyboard, (int delay), {
        _JS_MobileKeyboard_Hide(delay);
    });

    EM_JS(void, JS_SetText, (const char* text), {
        _JS_MobileKeyboard_SetText(text);
    });

    EM_JS(void, JS_SetSelection, (int start, int length), {
        _JS_MobileKeyboard_SetTextSelection(start, length);
    });

    EM_JS(void, JS_SetCharLimit, (int limit), {
        _JS_MobileKeyboard_SetCharacterLimit(limit);
    });

#endif //  __EMSCRIPTEN__
}

/* ==============================
   C++ wrapper implementation
   ============================== */

namespace MobileKeyboard
{
#ifdef  __EMSCRIPTEN__
    KeyboardStatus GetStatus()
    {
        return static_cast<KeyboardStatus>(JS_GetKeyboardStatus());
    }

    bool GetIgnoreBlurEvent()
    {
        return JS_GetIgnoreBlurEvent() != 0;
    }

    std::string GetText()
    {
        int length = JS_GetTextLength();
        if (length <= 0) return {};

        std::vector<char> buffer(length + 1);
        JS_GetText(buffer.data(), buffer.size());

        return std::string(buffer.data());
    }

    void GetSelection(int& start, int& length)
    {
        JS_GetSelection(&start, &length);
    }


    void TouchStart(std::string& text,
              KeyboardType type,
              bool autocorrect,
              bool multiline,
              bool secure,
              bool alert,
              const std::string& placeholder,
              int characterLimit)
    {
        JS_Touch_Start(
            text.c_str(),
            static_cast<int>(type),
            autocorrect,
            multiline,
            secure,
            alert,
            placeholder.c_str(),
            characterLimit
        );
    }

    void InteruptTouch()
    {
        JS_InteruptTouch();
    }
    void Hide(bool delay)
    {
        JS_HideKeyboard(delay ? 1 : 0);
    }

    void SetText(const std::string& text)
    {
        JS_SetText(text.c_str());
    }

    void SetSelection(int start, int length)
    {
        JS_SetSelection(start, length);
    }

    void SetCharacterLimit(int limit)
    {
        JS_SetCharLimit(limit);
    }
#endif
}
