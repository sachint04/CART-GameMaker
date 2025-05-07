#include "Object.h"
#include "Core.h"
namespace cart
{

	Object::Object()
		:m_isPendingDestroy{false},  m_id{ }
		{

		}

	Object::Object(const std::string& _id) :m_isPendingDestroy{ false }, m_id{_id}
	{
		
	}

	Object::~Object()  {
		//LOG("%s Object Destroyed ", m_id.c_str());
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
		//onDestory.Broadcast(this);
		m_isPendingDestroy = true;	
	}
	std::string Object::GetID() const	{
		return m_id;
	}
}