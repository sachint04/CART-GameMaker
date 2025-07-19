#pragma once
#include <raylib.h>
#include "Types.h"
#include "UIElement.h"

namespace cart {
	class World;
	class Shape : public UIElement {
	public:
		Shape(World* _owningworld, const std::string& _id);
		~Shape() override;
		void Init() override;
		void Update(float _deltaTime)  override;
		void Draw(float _deltaTime)  override;
		virtual void SetUIProperties(UI_Properties _prop);

	protected:
		
		SHAPE_TYPE m_shapeType;
		int m_LineWidth;
	};
}