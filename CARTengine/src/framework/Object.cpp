#include "Object.h"
#include "Core.h"
namespace cart
{

	Object::Object()
		:m_isPendingDestroy{ false }, m_id{ }, m_isReady{false}
		{

		}

	Object::Object(const std::string& _id) :m_isPendingDestroy{ false }, m_id{ _id }, m_isReady{false}
	{
		
	}

	Object::~Object()  {
		
	}

	weak<Object> Object::GetWeakRef()
	{
		return shared_from_this();
	}

	weak<const Object> Object::GetWeakRef() const
	{
		return shared_from_this();
	}

	void Object::Destroy()
	{
		m_isPendingDestroy = true;	
	}
	std::string Object::GetId() const	{
		return m_id;
	}
}