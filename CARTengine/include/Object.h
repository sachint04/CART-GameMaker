#pragma once
#include <memory>
#include <string>
#include "Core.h"
#include "Delegate.h"

namespace cart
{
	class Object : public std::enable_shared_from_this<Object>
	{

	public:
		Object();
		Object(const std::string& _id);
		virtual ~Object();
		virtual void Destroy();
		virtual std::string GetId()const;
		virtual bool IsReady() { return m_isReady; };
		bool IsPendingDestroy() const { return m_isPendingDestroy; }
		
		virtual weak<Object> GetWeakRef();
		virtual weak<const Object> GetWeakRef() const;
		Delegate<Object*> onDestory;
	protected:
		std::string m_id;
		bool m_isReady;
		bool m_isPendingDestroy;
	private:
	};
}