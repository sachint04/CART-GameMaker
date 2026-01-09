#pragma once
#include "UIElement.h"
#include "Core.h"
namespace cart {

	class World;
	class Sprite2D;
	class View : public UIElement
	{
	public:
		View(World* owningworld, const std::string& _id);
		~View();
		void Init() override;
		void Start() override;
		void Destroy() override;

	private:
		
	};
}