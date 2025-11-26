#include "Sprite.h"
#include "AssetManager.h"
#include "Core.h"

namespace cart {
	Sprite::Sprite( World* _owningworld, const std::string& _id, Rectangle _size, const std::string& _path, Vector2 _location)
		:Actor{_owningworld, _id }, m_size{_size}, m_texturePath{ _path }
	{
		m_rotation = 0;
		m_location = _location;
		m_color = WHITE;
		m_scale = 1.0;
		
	}
	void Sprite::Update(float _deltaTime)
	{
	}
	void Sprite::Draw(float _deltaTime)
	{
		weak<Texture2D> texture2d = AssetManager::Get().LoadTextureAsset(m_texturePath);
		if (!texture2d.expired()) {		
			DrawTextureEx(*texture2d.lock(), m_location, m_rotation, m_scale, m_color);
		}
		else {
			Logger::Get()->Error(std::format("Sprite \' {}\' Scound not load textue {}", GetId(), m_texturePath));
		}
	}
	Sprite::~Sprite()
	{
	}
}