#pragma once
#include <string>
#include "Actor.h"

namespace cart {

	class Sprite : public Actor{
	public:
		Sprite( World* _owningworld, const std::string& _id, Rectangle _size, const std::string& _path, Vector2 _location);
		~Sprite();
		void Update(float _deltaTime)  override;
		void Draw(float _deltaTime)  override;
	private:
		std::string m_texturePath;
		Rectangle m_size;
	};
}