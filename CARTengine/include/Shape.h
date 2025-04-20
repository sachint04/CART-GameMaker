#pragma once
#include <raylib.h>
#include "Types.h"
#include "UIElement.h"

namespace cart {

	class Shape : public UIElement {
	public:
		Shape(World* _owningworld, const std::string& _id, Vector2 _location, int width, int height, Color color, SHAPE_TYPE shape );
		~Shape() override;
		void Init() override;
		void Update(float _deltaTime)  override;
		void Draw(float _deltaTime)  override;


	protected:
		
		SHAPE_TYPE m_shapeType;
	};
}