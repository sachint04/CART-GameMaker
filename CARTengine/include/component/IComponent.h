#pragma once
#include <string>
#include "Types.h"
namespace cart {

	class IComponent{
	public:
		virtual void Destroy() = 0;
		virtual void Update() = 0;

		IComponent(const std::string& id, COMPONENT_TYPE type) :m_Id{ id }, m_type{type}, m_isEnabled { true } {};
		COMPONENT_TYPE GetType() {
			return m_type;
		};

		const std::string& GetId()const {
			return m_Id;
		};
		bool IsEnabled() { return m_isEnabled; };

	protected :
		bool m_isEnabled;
		std::string m_Id;
		COMPONENT_TYPE m_type = COMPONENT_TYPE::NO_COMPONENT;


	};

}