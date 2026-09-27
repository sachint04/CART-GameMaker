#include "Text.h"
#include "AssetManager.h"
#include "World.h"
#include "UICanvas.h"
namespace cart {

#pragma region  Constructor & Initialization
	Text::Text(World* _owningworld, const std::string& _id, bool isExcludedFromParentAutoControl)
		:UIElement{ _owningworld,_id, isExcludedFromParentAutoControl },
		m_text{ },
		m_font{ },
		m_fontsize{ },
		m_margin(5),
		m_align{ LEFT },
		m_background{ 0,0,0,0 },
		m_textColor{ BLACK },
		m_fontspacing{ 1.f },
		m_textLocation{},
		m_minfontsize{},
		m_maxfontsize{},
		m_minfontspacing{},
		m_valign{MIDDLE},
		m_multiline{false},
		m_strlines{},
		m_overflow{false}, 
		m_linespace{1.f}
	{
	}

	void Text::Init()
	{
		UIElement::Init();
		
	}

	void Text::Start()
	{
		UIElement::Start();

	}

#pragma endregion

#pragma region  LOOP

	void Text::Update(float _deltaTime)
	{
		if (!m_visible)return;
		UIElement::Update(_deltaTime);
		m_sharedfont = AssetManager::Get().LoadFontAsset(m_font, std::ceil(m_fontsize *  World::UI_CANVAS.get()->Scale()));

	}
	void Text::Draw(float _deltaTime)
	{
		if (m_visible == false)return;
		UIElement::Draw(_deltaTime);		
		float fsize = std::max(m_minfontsize, std::ceil(m_fontsize * World::UI_CANVAS.get()->Scale()));
		float fspace = std::max(m_minfontspacing, m_fontspacing );
		//Rectangle rect = GetBounds();
		//if(!m_sharedfont)
		m_sharedfont = AssetManager::Get().LoadFontAsset(m_font, fsize);

		//m_textsize = MeasureTextEx(*m_sharedfont, m_text.c_str(), fsize, fspace);
//		DrawRectangle(m_location.x, m_location.y, m_width, m_height, m_background);	

		if (!m_strlines.empty()) {
			if (m_multiline) {
				for (auto iter = m_strlines.begin(); iter != m_strlines.end(); ++iter)
				{					
					DrawTextEx(*m_sharedfont, TextSubtext(iter->text.data(), 0, iter->text.size()), { iter->size.x, iter->size.y }, fsize, fspace, m_textColor);
				}
			}
			else {
				auto begin = m_strlines.begin();
				if (begin != m_strlines.end())
				{
					if (!begin->text.empty())
					{
						DrawTextEx(*m_sharedfont, TextSubtext(begin->text.data(), 0, begin->text.size()), {begin->size.x, begin->size.y}, fsize, fspace, m_textColor);
					}
				}
			}
		}
	}
#pragma endregion

#pragma region  Helpers
	void Text::SetTextProperties(Text_Properties _prop)
	{	
		SetUIProperties((UI_Properties)_prop);
		m_font = _prop.font;
		m_text = _prop.text;
		m_fontsize = std::max(_prop.fontsize, _prop.minfontsize);
		m_minfontsize = _prop.minfontsize;
		m_maxfontsize = _prop.maxfontsize;
		m_align = _prop.align;	
		m_valign = _prop.valign;	
		m_background = _prop.textbackground;
		m_fontspacing = _prop.fontspacing;
		m_minfontspacing = _prop.minfontspacing;
		m_textColor = _prop.textcolor;	
		m_multiline = _prop.multiline;
		m_linespace = _prop.linespace;
		if(m_isReady)UpdateTextLocation();
	}
	void Text::SetFontName(const std::string& strfnt)
	{
		m_sharedfont = AssetManager::Get().LoadFontAsset(strfnt, m_fontsize);
		AssetManager::Get().UnloadFontAsset(m_font, m_fontsize);
		m_font = strfnt;
	}
	std::string Text::GetFontName() {
		return m_font;
	}
	void Text::SetFontSize(float size)
	{
		//if (m_fontsize == size)return;
		m_sharedfont.reset();
		float tmpsize = std::max(size, m_minfontsize);
		
		if (m_maxfontsize > 0) tmpsize = std::min(m_maxfontsize, tmpsize);

		m_sharedfont = AssetManager::Get().LoadFontAsset(m_font, tmpsize);
		AssetManager::Get().UnloadFontAsset(m_font, m_fontsize);
		m_fontsize = tmpsize;// std::max(size, m_minfontsize);
	}
	void Text::SetMinFontSize(float size)
	{
		m_minfontsize = size;
	}
	void Text::SetMaxFontSize(float size)
	{
		m_maxfontsize = size;
	}
	float Text::GetMinFontSize()
	{
		return m_minfontsize;
	}
	float Text::GetMaxFontSize()
	{
		return m_maxfontsize;
	}
	void Text::SetTextColor(Color col)
	{
		m_textColor = col;
	}
	void Text::SetText(const std::string& str) {
		m_text = str;
		UpdateTextLocation();
	}
	void Text::SetAligned(ALIGN _align)
	{
		m_align = _align;
	}
	void Text::SetVAligned(V_ALIGN _valign)
	{
		m_valign = _valign;
	}
	bool Text::ValidateTextProperties(Text_Properties _prop)
	{
		return _prop.fontsize && _prop.minfontsize && _prop.fontspacing && _prop.minfontspacing;
	}
	void Text::UpdateTextLocation()
	{
		float uiScale = World::UI_CANVAS.get()->Scale();
		float fsize = std::max(m_minfontsize, std::ceil(m_fontsize * uiScale));
		float fspace = std::max(m_minfontspacing, m_fontspacing);
		float linespace = std::max(0.8f, m_linespace * uiScale);
		Rectangle rect = GetBounds();
		weak<UIElement> parent = GetUIParent();
		if (auto lock = parent.lock())
		{
			Vector2 p = lock->GetPadding();
			rect.x += p.x;
			rect.y += p.y;
			rect.width -= p.x * 2.f;
			rect.height -= p.y * 2.f;
		}
		m_sharedfont = AssetManager::Get().LoadFontAsset(m_font, fsize);

#pragma region Wrap Text new logic
		Vector2 msize = { 0,0 };
		float linespacing = 1.f;
		float maxwidth = rect.width;	
		if (m_multiline) {
			Text::WrapText(m_text, maxwidth, linespacing, m_strlines,  m_sharedfont, fsize, fspace, linespace);
		}
		else {
			m_strlines.clear();
			Vector2 tsize = MeasureTextEx(*m_sharedfont, m_text.c_str(), fsize, fspace);			
			std::string_view lineView = m_text;
			m_strlines.push_back({ lineView, {0,0, tsize.x, tsize.y} });
		}
#pragma endregion
//#pragma region  Wrap text
//		m_strlines = {};
//		std::string strcopy = m_text;
//		std::string spacedelimiter = " ";
//		int theight = 0;
//		std::string line = "";
//		float linespacing = 0;
//		Vector2 msize = { 0,0 };
//		float msgheight = 0;		
//		float maxwidth = rect.width - (m_margin * 2);		
//		
//		while (strcopy.find(spacedelimiter) != std::string::npos) {// create stings of line in
//			auto find = strcopy.find(spacedelimiter);			
//			line = line + strcopy.substr(0, find + 1);
//			msize = MeasureTextEx(*m_sharedfont, line.c_str(), (float)fsize, fspace);
//			if (msize.x >= maxwidth) {
//				line.pop_back();
//				line = line.substr(0, line.find_last_of(spacedelimiter));
//				msize = MeasureTextEx(*m_sharedfont, line.c_str(), (float)fsize, fspace);
//				m_strlines.push_back({ line, msize });// add line to list
//				line.clear();// clear line    
//				theight += msize.y + linespacing;
//			}
//			else {				
//				strcopy = strcopy.substr(find + 1);// remove last word from the string
//			};
//		}
//		if (!strcopy.empty())
//		{
//			line = line + strcopy;
//			msize = MeasureTextEx(*m_sharedfont, line.c_str(), (float)fsize, fspace);
//			m_strlines.push_back({ line , msize });
//			theight += msize.y + linespacing;
//		}
//		
//#pragma endregion
#pragma region Calculate Start position based on Alignment
		float theight = 0;
		for (auto& line : m_strlines)
		{
			theight += line.size.height;
		}
		auto find = m_strlines.begin();

		if (find == m_strlines.end())
			m_strlines.push_back({ "" , {0,0} });

		int al = m_align, va = m_valign;
		
		float sy = rect.y;
		if (va == 1) {
			sy = rect.y + ((rect.height - theight) * 0.5f);
		}
		else if (va == 2) {
			sy = rect.y + (rect.height - theight );
		}

		for (auto iter = m_strlines.begin(); iter != m_strlines.end(); ++iter)
		{
			float sx = rect.x;
			
			
			if (al == 1) {
				sx +=std::floor( (rect.width - (iter->size.width)) * 0.5f);
			}
			else if (al == 2) {
				sx += (rect.width - (iter->size.width ));
			}
			iter->size.x =  sx;
			iter->size.y =  sy;
			sy += iter->size.height + linespacing;
		}
#pragma endregion
		


	}
#pragma endregion

#pragma region Static Helper
/*
	void Text::WrapText(std::string_view text, float maxwidth, float linespacing, std::vector<WrappedLine>& output, shared<Font> & font, float fsize, float fspace) {
		output.clear();
		float theight = 0;
		std::string currentLine = "";
		std::string_view remaining = text;
		const int BINARY_SEARCH_THRESHOLD = 32;

		auto commit = [&](const std::string& l) {
			if (l.empty()) return;
			Vector2 size = MeasureTextEx(*font, l.c_str(), fsize, fspace);
			output.push_back({ l, size }); // vector preserves order and duplicates
			theight += size.y + linespacing;
		};

		while (!remaining.empty()) {
			size_t pos = remaining.find_first_of(" \t\n");
			bool isNL = (pos != std::string_view::npos && remaining[pos] == '\n');

			std::string_view word_v = (pos == std::string_view::npos) ? remaining : remaining.substr(0, pos + 1);
			std::string word(word_v);

			Vector2 totalSize = MeasureTextEx(*font, (currentLine + word).c_str(), fsize, fspace);

			if (totalSize.x > maxwidth && !isNL) {
				// WORD IS TOO LONG - Apply Binary Search
				if (word.length() >= BINARY_SEARCH_THRESHOLD) {
					std::string_view longWord = word;
					while (!longWord.empty()) {
						int low = 1, high = (int)longWord.length(), best = 0;
						while (low <= high) {
							int mid = low + (high - low) / 2;
							std::string test = currentLine + std::string(longWord.substr(0, mid));
							if (MeasureTextEx(*font, test.c_str(), fsize, fspace).x <= maxwidth) {
								best = mid; low = mid + 1;
							}
							else high = mid - 1;
						}

						if (best == 0 && !currentLine.empty()) {
							commit(currentLine); currentLine = "";
						}
						else {
							int take = (best == 0) ? 1 : best;
							currentLine += std::string(longWord.substr(0, take));
							if (take < (int)longWord.length()) { commit(currentLine); currentLine = ""; }
							longWord.remove_prefix(take);
						}
					}
				}
				else {
					// Short word linear wrap
					commit(currentLine);
					currentLine = word;
				}
			}
			else {
				currentLine += word;
			}

			if (isNL) { commit(currentLine); currentLine = ""; }
			remaining.remove_prefix(word_v.length());
		}
		commit(currentLine);
	}
	*/
	
//	void Text::WrapText(std::string_view text, float maxwidth, float linespacing, std::vector<WrappedLine>& output, shared<Font>& font, float fsize, float fspace) {
//		output.clear();
//		std::string currentLine = "";
//		std::string_view remaining = text;
//		const int BINARY_SEARCH_THRESHOLD = 32;
//
//		auto commit = [&](const std::string& l) {
//			if (l.empty() && output.empty() && remaining.empty()) return; // Don't commit nothing
//			std::string measureTarget = l.empty() ? " " : l;
//			Vector2 size = MeasureTextEx(*font, measureTarget.c_str(), fsize, fspace);
//			if (l.empty()) size.x = 0;
//			output.push_back(WrappedLine(l, size));
//		};
//
//		while (!remaining.empty()) {
//			size_t pos = remaining.find_first_of(" \t\n");
//			bool isNL = (pos != std::string_view::npos && remaining[pos] == '\n');
//
//			std::string_view word_v = (pos == std::string_view::npos) ? remaining : remaining.substr(0, pos);
//			std::string word(word_v);
//			std::string_view delim = (pos != std::string_view::npos) ? remaining.substr(pos, 1) : "";
//
//			// Check if adding this word (and space) exceeds width
//			std::string testLine = currentLine + word + (isNL ? "" : std::string(delim));
//			if (MeasureTextEx(*font, testLine.c_str(), fsize, fspace).x > maxwidth) {
//
//				if (word.length() >= BINARY_SEARCH_THRESHOLD) {
//					// --- BINARY SEARCH SPLIT ---
//					std::string_view longWord = word;
//					while (!longWord.empty()) {
//						int low = 1, high = (int)longWord.length(), best = 0;
//						while (low <= high) {
//							int mid = low + (high - low) / 2;
//							std::string test = currentLine + std::string(longWord.substr(0, mid));
//							if (MeasureTextEx(*font, test.c_str(), fsize, fspace).x <= maxwidth) {
//								best = mid; low = mid + 1;
//							}
//							else high = mid - 1;
//						}
//
//						if (best == 0 && !currentLine.empty()) {
//							commit(currentLine); currentLine = "";
//						}
//						else {
//							int take = (best == 0) ? 1 : best;
//							currentLine += std::string(longWord.substr(0, take));
//							if (take < (int)longWord.length() || MeasureTextEx(*font, (currentLine + std::string(delim)).c_str(), fsize, fspace).x > maxwidth) {
//								commit(currentLine); currentLine = "";
//							}
//							longWord.remove_prefix(take);
//						}
//					}
//				}
//				else {
//					// Standard wrap: commit existing and start new line with this word
//					commit(currentLine);
//					currentLine = word;
//				}
//			}
//			else {
//				currentLine += word;
//			}
//
//			// Add delimiter if not a newline
//			if (isNL) {
//				commit(currentLine);
//				currentLine = "";
//			}
//			else if (!delim.empty() && !currentLine.empty() && currentLine.back() != ' ' && currentLine.back() != '\t') {
//				// Only add delim if the binary search didn't just clear the line
//				currentLine += delim;
//			}
//			if (pos == std::string_view::npos) {
//				remaining.remove_prefix(remaining.length());
//			}
//			else {
//				remaining.remove_prefix(pos + 1);
//			}
////			remaining.remove_prefix(pos == std::string_view::npos ? remaining.length() : pos + 1);
//		}
//		//if (!currentLine.empty()) commit(currentLine);
//		commit(currentLine);
//	}

//void Text::WrapText(std::string_view text, float maxwidth, float linespacing, std::vector<WrappedLine>& output, shared<Font>& font, float fsize, float fspace) {
//	output.clear();
//	std::string currentLine = "";
//	std::string_view remaining = text;
//	const int BINARY_SEARCH_THRESHOLD = 32;
//
//	auto commit = [&](const std::string& l) {
//		std::string measureTarget = l.empty() ? " " : l;
//		Vector2 size = MeasureTextEx(*font, measureTarget.c_str(), fsize, fspace);
//		if (l.empty()) size.x = 0;
//		output.push_back(WrappedLine(l, { 0, 0, size.x, size.y }));
//	};
//
//	if (text.empty()) {
//		commit("");
//		return;
//	}
//
//	while (!remaining.empty()) {
//		size_t pos = remaining.find_first_of(" \t\n");
//		bool isNL = (pos != std::string_view::npos && remaining[pos] == '\n');
//
//		std::string_view word_v = (pos == std::string_view::npos) ? remaining : remaining.substr(0, pos);
//		std::string word(word_v);
//		std::string_view delim = (pos != std::string_view::npos) ? remaining.substr(pos, 1) : "";
//
//		// Only process word wrapping if the word itself has characters
//		if (!word.empty()) {
//			if (MeasureTextEx(*font, (currentLine + word).c_str(), fsize, fspace).x > maxwidth) {
//				if (word.length() >= BINARY_SEARCH_THRESHOLD) {
//					// --- BINARY SEARCH LOGIC ---
//					std::string_view longWord = word;
//					while (!longWord.empty()) {
//						int low = 1, high = (int)longWord.length(), best = 0;
//						while (low <= high) {
//							int mid = low + (high - low) / 2;
//							if (MeasureTextEx(*font, (currentLine + std::string(longWord.substr(0, mid))).c_str(), fsize, fspace).x <= maxwidth) {
//								best = mid; low = mid + 1;
//							}
//							else high = mid - 1;
//						}
//						if (best == 0 && !currentLine.empty()) {
//							commit(currentLine); currentLine = "";
//						}
//						else {
//							int take = (best == 0) ? 1 : best;
//							currentLine += std::string(longWord.substr(0, take));
//							if (take < (int)longWord.length()) { commit(currentLine); currentLine = ""; }
//							longWord.remove_prefix(take);
//						}
//					}
//				}
//				else {
//					commit(currentLine);
//					currentLine = word;
//				}
//			}
//			else {
//				currentLine += word;
//			}
//		}
//
//		// --- FIXED DELIMITER HANDLING FOR CONSECUTIVE SPACES ---
//		if (isNL) {
//			commit(currentLine);
//			currentLine = "";
//			if (remaining.length() == 1) commit("");
//		}
//		else if (!delim.empty()) {
//			// Check if adding this specific space/tab pushes the line over the edge
//			if (MeasureTextEx(*font, (currentLine + std::string(delim)).c_str(), fsize, fspace).x <= maxwidth) {
//				currentLine += delim;
//			}
//			else {
//				// If it's a sequence of spaces breaking the boundary, commit the line
//				commit(currentLine);
//				currentLine = "";
//
//				// If it's a trailing space running onto a new line, we keep it as the start of the next line
//				currentLine += delim;
//			}
//		}
//
//		remaining.remove_prefix(pos == std::string_view::npos ? remaining.length() : pos + 1);
//	}
//
//	if (!currentLine.empty()) commit(currentLine);
//}
#include "raylib.h"
#include <vector>
#include <string>

void Text::WrapText(std::string & originalText, float maxWidth, float linespacing , std::vector<WrappedLine>& outWrappedLines, shared<Font> & fontPtr, float fontSize, float spacing, float lineSpaceMultiplier) {
	outWrappedLines.clear();
	if (originalText.empty() || !fontPtr) return; // Guard against null shared pointers

	// Cache the internal Font reference locally to prevent pointer-chasing inside the tight loop
	const Font& font = *fontPtr;

	float scaleFactor = fontSize / (float)font.baseSize;
	float lineHeight = ((font.baseSize + (float)font.baseSize / 2.0f) * scaleFactor)* lineSpaceMultiplier;

	const char* textStartPtr = originalText.c_str();
	int textLength = (int)originalText.length();

	int currentLineStart = 0;
	int lastSpaceByteIndex = -1;

	float currentLineWidth = 0.0f;
	float wordWidthAccumulator = 0.0f;
	float currentOffsetY = 0.0f;

	int i = 0;
	while (i < textLength) {
		int codepointByteSize = 0;
		int codepoint = GetCodepointNext(&textStartPtr[i], &codepointByteSize);
		int glyphIndex = GetGlyphIndex(font, codepoint);

		float charWidth = (font.glyphs[glyphIndex].advanceX == 0)
			? font.recs[glyphIndex].width * scaleFactor
			: font.glyphs[glyphIndex].advanceX * scaleFactor;

		if (codepoint == '\n') {
			std::string_view lineView(&textStartPtr[currentLineStart], i - currentLineStart);
			outWrappedLines.emplace_back(lineView, Rectangle{ 0, currentOffsetY, currentLineWidth, lineHeight });

			currentOffsetY += lineHeight;
			currentLineStart = i + codepointByteSize;
			currentLineWidth = 0.0f;
			wordWidthAccumulator = 0.0f;
			lastSpaceByteIndex = -1;
			i += codepointByteSize;
			continue;
		}

		if (codepoint == ' ' || codepoint == '\t') {
			lastSpaceByteIndex = i;
			wordWidthAccumulator = 0.0f;
		}
		else {
			wordWidthAccumulator += (charWidth + spacing);
		}

		if (currentLineWidth + charWidth > maxWidth) {
			if (lastSpaceByteIndex != -1 && lastSpaceByteIndex > currentLineStart) {
				std::string_view lineView(&textStartPtr[currentLineStart], lastSpaceByteIndex - currentLineStart);
				outWrappedLines.emplace_back(lineView, Rectangle{ 0, currentOffsetY, currentLineWidth - wordWidthAccumulator, lineHeight });

				currentOffsetY += lineHeight;
				currentLineStart = lastSpaceByteIndex + 1;
				i = currentLineStart;
				currentLineWidth = 0.0f;
				wordWidthAccumulator = 0.0f;
				lastSpaceByteIndex = -1;
				continue;
			}
			else {
				std::string_view lineView(&textStartPtr[currentLineStart], i - currentLineStart);
				outWrappedLines.emplace_back(lineView, Rectangle{ 0, currentOffsetY, currentLineWidth, lineHeight });

				currentOffsetY += lineHeight;
				currentLineStart = i;
				currentLineWidth = charWidth + spacing;
				lastSpaceByteIndex = -1;
			}
		}
		else {
			currentLineWidth += (charWidth + spacing);
		}

		i += codepointByteSize;
	}

	if (currentLineStart < textLength) {
		std::string_view lineView(&textStartPtr[currentLineStart], textLength - currentLineStart);
		outWrappedLines.emplace_back(lineView, Rectangle{ 0, currentOffsetY, currentLineWidth, lineHeight });
	}
}

#include "raylib.h"
#include <string>

// Returns a truncated string with "..." if it exceeds max_width
std::string Text::GetElidedText(Font font, const std::string& text, float max_width, float fontSize, float spacing, int padding,  Vector2 & stripedTextSize) {

	// 1. Check if the full text already fits inside the button
	stripedTextSize = MeasureTextEx(font, text.c_str(), fontSize, spacing);
	if (stripedTextSize.x <= max_width) {
		return text;
	}

	// Calculate maximum visual real estate left for actual text characters
	float available_width = max_width - (padding * 2);

	std::string result = "";

	// 2. Loop through and accumulate characters until we hit the pixel ceiling
	for (char c : text) {
		std::string test_str = result + c;
		Vector2 tsize = MeasureTextEx(font, test_str.c_str(), fontSize, spacing);
		if (stripedTextSize.x > available_width) {
			break; // Stop immediately; adding this character exceeds available room
		}
		stripedTextSize = tsize;
		result += c;
	}

	return result;
}

#pragma endregion

#pragma region Event Listeners

	void Text::OnLayoutChangeHandler()
	{
		UIElement::OnLayoutChangeHandler();
		UpdateTextLocation();
	}
#pragma endregion


#pragma region  Clean Up

	Text::~Text()
	{
		if (m_isPendingDestroy)return;

		m_sharedfont.reset();
	//	LOG("%s Text Deleted!", m_id.c_str());
	}

	void Text::Destroy() {
		UIElement::Destroy();
	}



#pragma endregion
}
