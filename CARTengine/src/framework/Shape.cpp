#include "Shape.h"
#include "World.h"

namespace cart {
	
	Shape::Shape(World* _owningworld, const std::string& _id)
		:UIElement{ _owningworld, _id }, 
		m_shapeType{  },
		m_LineWidth{  }
	{
		//std::cout << GetID() << " ->Shape created \n";
	

	}

	void Shape::Update(float _deltaTime)
	{
		UIElement::Update(_deltaTime);
	}

	void Shape::Draw(float _deltaTime)
	{
		if (!m_visible)return;
		/*if (m_location.x < 0 || m_location.x + (m_width * m_scale) > GetWindowSize().x
			|| m_location.y < 0 || m_location.y + (m_height * m_scale)> GetWindowSize().y) {			
			return;
		}*/
		Rectangle rect = GetBounds();
		
		

		if (m_shapeType == SHAPE_TYPE::RECTANGLE) {
			DrawRectangle(rect.x, rect.y, rect.width, rect.height, m_color);
		}
		else if (m_shapeType == SHAPE_TYPE::CIRCLE) {
			DrawCircle(rect.x + (rect.width * 0.5f), rect.y + (rect.height * 0.5f), (rect.width * 0.5f) * m_scale, m_color);
		}
		else if (m_shapeType == SHAPE_TYPE::LINE) {
		//	DrawRectanglePro(rect, origin, m_rotation, {0,0,0,0});
			DrawRectangleLinesEx({ rect.x, rect.y, rect.width,  rect.height},m_LineWidth, m_borderColor);
		}

	}

	void Shape::SetUIProperties(UI_Properties _prop)
	{
		UIElement::SetUIProperties(_prop);
		m_shapeType = _prop.shapetype;
		m_LineWidth = _prop.linewidth;

	}

	Shape::~Shape()
	{
		//std::cout <<this << " -> Shape deleted \n";
	}
	void Shape::Init()
	{
		UIElement::Init();
	}
}