#pragma  once
#include <string>
#include <raylib.h>
#include "Sprite2D.h"
namespace cart {

	class World;
	class PatternSprite : public Sprite2D {

	public :
		PatternSprite(World* _owningworld, const std::string& _id, int m_patternIndex, std::vector<Rectangle>& patterndata);
		~PatternSprite();

		void Init()override;
		
		void Draw(float _deltaTime) override;
	
	private:
			void DrawTextureTiled( Rectangle source, Rectangle dest, Vector2 origin, float rotation, float scale, Color tint);
		std::vector<Rectangle> m_recPattern;
		int m_patternIndex;
	};
}