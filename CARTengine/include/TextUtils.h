#pragma once
#include <raylib.h>
#include <iostream>
namespace cart
{
	void DrawTextBoxed(Font font,
		const std::string&  text,
		Rectangle rec,
		float fontSize,
		float spacing,
		bool wordWrap,
		Color tint,
		int selectStart,
		int selectLength,
		Color selectTint,
		Color selectBackTint);
}