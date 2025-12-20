#pragma once
#include "Text.h"
#include <string>
#define MAX_INPUT_CHARS     600
namespace cart {

	class TextInput : public Text
	{
	protected:
		void OnScreenSizeChange() override;
	public:
		TextInput(World* _owningworld, const std::string& _id);
		~TextInput();
		void Init()override;
		void Start()override;
		void Update(float _deltaTime)override;
		void Draw(float _deltaTime)override;
		void Destroy()override;
		void SetText(const std::string &txt);
		std::string GetInputText();
		void SetFontName(const std::string& strfnt)override;
		float GetFontSize() { return m_fontsize; };
		void SetFontSize(float size) override;
		void UpdateLayout();
		void PrepareInput();
		void ProcessInput();
		void OnMobileInput(const char* input);
	private:
		int m_key;
		int m_charLimit;
		int m_curletterindex;
		int m_letterCount = 0;
		bool m_mouseOnText;
		int m_framesCounter = 0;
		char name[MAX_INPUT_CHARS + 1] = "\0";
	//	std::vector<std::string> lines;
		//std::vector<std::pair<std::pair<int, int>, Vector2>> m_lrange;
		std::vector<std::pair<std::string, Vector2>> m_lines;
		bool m_isBackspace;
		bool m_isRightKey;
		bool m_isLeftKey;
		bool m_isDeleteKey;
		bool m_touch;
		bool m_hasUpated;

		float m_textmargin;
		float m_keydownWaitTimeMultiplyer;
		float m_keydownMulitiplyerDuration;
		float m_keydownActionDuration;
		float m_tempkeydownActionDuration;
		float m_keydownActionMinDuration;
		double m_keyWaitTimer;
		double m_backspacekeyWaitTimer;

		Vector2 m_touchstartpos;
		Vector2 m_touchendpos;
		Vector2 m_cursorLoc;
	/*	shared<Font> fnt;*/
		shared<Font> infofnt;
		void TextLine(const std::vector<std::pair<std::string, Vector2>>& str);
		//void CharsToVec();
	};
}