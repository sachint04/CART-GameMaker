#pragma once
#include "Text.h"

#define MAX_INPUT_CHARS     200
namespace cart {

	class TextInput : public Text
	{
		
	public:
		TextInput(World* _owningworld, const std::string& _id, Vector2 _size);
		~TextInput();

		void Update(float _deltaTime)override;
		void Draw(float _deltaTime)override;
		void Destroy()override;

	private:
		int m_charLimit;
		int m_letterCount = 0;
		bool m_mouseOnText;
		int m_framesCounter = 0;
		char name[MAX_INPUT_CHARS + 1] = "\0";
	};
}