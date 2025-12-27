#pragma once
#include <string>
#include "Types.h"
using namespace cart;

namespace MobileKeyboard
{
#ifdef __EMSCRIPTEN__

    KeyboardStatus GetStatus();
    bool GetIgnoreBlurEvent();

    std::string GetText();
    void GetSelection(int& start, int& length);

    void TouchStart(std::string& text,
              KeyboardType type,
              bool autocorrect,
              bool multiline,
              bool secure,
              bool alert,
              const std::string& placeholder,
              int characterLimit);
    
    void InteruptTouch();
    void Hide(bool delay);

    void SetText(const std::string& text);
    void SetSelection(int start, int length);
    void SetCharacterLimit(int limit);
#endif // __EMSCRIPTEN__
}
