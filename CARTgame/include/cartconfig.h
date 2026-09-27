#pragma once
#include <string>
std::string GetResourceDir()
{
#ifdef PLATFORM_WEB
	return "assets";
#endif

#ifdef NDEBUG // release build
	return "assets/";
#else // debug build
	return "D:/c++/CART/CARTgame/assets/";
#endif // 

}

