#pragma once
#include "Text.h"
#define MAX_INPUT_CHARS     200
namespace cart {

	class TextInput : public Text
	{
		
	public:
		TextInput(World* _owningworld, const std::string& _id);
		~TextInput();
		void Init()override;
		void Update(float _deltaTime)override;
		void Draw(float _deltaTime)override;
		void Destroy()override;
		void SetText(const std::string &txt);
		void SetFontName(const std::string& strfnt)override;
		float GetFontSize() { return m_fontsize; };
		void SetFontSize(float size) override;
	private:
		int m_charLimit;
		int m_curletterindex;
		int m_letterCount = 0;
		bool m_mouseOnText;
		int m_framesCounter = 0;
		char name[MAX_INPUT_CHARS + 1] = "\0";
	//	std::vector<std::string> lines;
		std::vector<std::pair<std::pair<int, int>, Vector2>> m_lrange;
		std::vector<std::pair<std::string, Vector2>> m_lines;
		bool m_isBackspace;
		bool m_isRightKey;
		bool m_isLeftKey;
		bool m_isDeleteKey;
		double m_keyWaitTimer;
		double m_backspacekeyWaitTimer;
		int m_textmargin;
		bool m_touch;
		Vector2 m_touchstartpos;
		Vector2 m_touchendpos;
	/*	shared<Font> fnt;*/
		shared<Font> infofnt;
		void TextLine(const std::vector<std::pair<std::string, Vector2>>& str);
		//void CharsToVec();
	};
}