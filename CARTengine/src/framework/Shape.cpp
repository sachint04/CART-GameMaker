#include "Shape.h"


namespace cart {
	
	Shape::Shape(World* _owningworld, const std::string& _id, Vector2 _location, int width, int height, Color color, SHAPE_TYPE shape)
		:UIElement{ _owningworld, _id }, m_shapeType{ shape }
	{
		//std::cout << GetID() << " ->Shape created \n";
		m_location = _location;
		m_width = width;
		m_height = height;
		m_color = color;

	}

	void Shape::Update(float _deltaTime)
	{
		UIElement::Update(_deltaTime);
	}

	void Shape::Draw(float _deltaTime)
	{
		/*if (m_location.x < 0 || m_location.x + (m_width * m_scale) > GetWindowSize().x
			|| m_location.y < 0 || m_location.y + (m_height * m_scale)> GetWindowSize().y) {			
			return;
		}*/

		Rectangle rect = { m_location.x, m_location.y ,  m_width * m_scale, m_height * m_scale};
		Vector2 origin = { m_location.x - (rect.width / 2.f) , rect.height / 2 };//{ 0.0f, 0.0f };//

		if (m_shapeType == SHAPE_TYPE::RECTANGLE) {
			DrawRectangle(m_location.x, m_location.y, rect.width, rect.height, m_color);
		}
		else if (m_shapeType == SHAPE_TYPE::CIRCLE) {
			DrawCircle(m_location.x, m_location.y, m_width * m_scale, m_color);
		}
		else if (m_shapeType == SHAPE_TYPE::LINE) {
		//	DrawRectanglePro(rect, origin, m_rotation, {0,0,0,0});
			DrawRectangleLinesEx({ m_location.x, m_location.y, m_width * m_scale,  m_height * m_scale }, 1.f, m_color);
		}

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