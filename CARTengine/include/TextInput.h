#pragma once
#include "Text.h"
#include <string>
#include "Types.h"
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
		void SetFontName(const std::string& strfnt)override;
		float GetFontSize() { return m_fontsize; };
		void SetFontSize(float size) override;
		void SetTextProperties(Text_Properties _props)override;
		void SetFocused(bool _flag)override;
		void SetVisible(bool _flag)override;
		void Destroy()override;
		void SetAligned(ALIGN _align)override;
		void SetVAligned(V_ALIGN _valign)override;


		void SetText(const std::string &txt);
		std::string GetInputText();
		void UpdateLayout();
		void PrepareInput(Rectangle bounds);
		void CalculateCursor(Rectangle bounds);
		void ShowCharLimitWarning(Rectangle bounds);
		void ShowRemainingCharCount(Rectangle bounds);
		void OnMobileInput(char* input, int isBackspace);
	private:
		int m_key;
		int m_charLimit;
		int m_curletterindex;
		int m_letterCount = 0;
		bool m_mouseOnText;
		int m_framesCounter = 0;
		char name[MAX_INPUT_CHARS + 1] = "\0";
		std::string m_mobileinput;
	//	std::vector<std::string> lines;
		//std::vector<std::pair<std::pair<int, int>, Vector2>> m_lrange;
		std::vector<std::string>m_lines;
		std::vector<Vector2>m_pos;
		bool m_isBackspace;
		bool m_isRightKey;
		bool m_isLeftKey;
		bool m_isDeleteKey;
		bool m_touch;
		bool m_hasUpated;
		bool m_bMobileInput;
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
		shared<Font> m_infofnt;
		void TextLine(Rectangle rect);
		//void CharsToVec();
	};
}