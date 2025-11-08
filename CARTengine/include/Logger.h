#pragma once
#include <string>
#include <map>
#include <raylib.h>
#include <format>
#include "Types.h"

namespace cart
{

	class Logger
	{
	public:
		Logger(Logger const&) = delete;
		void operator=(const Logger&) = delete;
		~Logger();
		static Logger* Get();
		void Push(std::string _t, LOG_TYPE _type = LOG_TYPE::LOG_INFO);
		void Show();
		void Hide();
		void Clear();
		void Update(float _deltaTime);
		void Draw(float _deltaTime);
		bool IsMouseOverUI() { return m_ismouseOverUI; }
		void SetRect(Rectangle _rect);
		void SetMaxLogCount(int _lines);
		void Drag();
	private:

		Logger();

		static Logger* instance;
		std::vector<Log_Data> logdb;
//		std::map<int, std::string > logdb = {};

		bool resizing = false;
		bool wordWrap = true;
		bool isVisible = false;

		Vector2 lastMouse = { 0.0f, 0.0f }; // Stores last mouse coordinates
		Color borderColor = BLUE;         // Container border color
		Font font;       // Get default system font

		void DrawTextBoxed(Font font, const char* text, Rectangle rec, float fontSize, float spacing, bool wordWrap, Color tint);
		void DrawTextBoxedSelectable(Font font, const char* text, Rectangle rec, float fontSize, float spacing, bool wordWrap, Color tint, int selectStart, int selectLength, Color selectTint, Color selectBackTint);

		std::string strinfo = "Info ->";
		std::string stralert = "ALERT! ->";
		std::string strerror = "ERROR!!! ->";
		int m_max_log_count =10;
		int m_log_count = 0;
		int screenWidth = 300;
		int screenHeight = 600;
		Rectangle container = { 25.0f, 25.0f, 250.f, 600.f };
		Rectangle resizer = { 283.f, 408 , 14, 14 };
		Rectangle clearbtn = { 283.f, 28.0f , 14, 14 };
		float minWidth = 60;
		float minHeight = 60;
		float maxWidth = 800.0f;
		float maxHeight = 600.0f;
		bool m_ismouseOverUI = false;
		float fontsize = 10.f;
		bool dragging = false;

	};



}