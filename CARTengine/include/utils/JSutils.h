#pragma once

#ifdef __cplusplus
extern "C" {
#endif
#ifdef __EMSCRIPTEN__
    extern void GetMobileChar();
    extern void ToggleMobileKeyboard(bool show);
#endif
#ifdef __cplusplus
}
#endif