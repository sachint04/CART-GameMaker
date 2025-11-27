#pragma once
#include <string>
namespace cart {

	class IComponent{
	public:
		virtual void Destroy() = 0;
		virtual void Update() = 0;

		IComponent(const std::string& id) :m_Id{ id }, m_isEnabled{true} {};
		std::string type() {
			return m_type;
		};

		const std::string& GetId()const {
			return m_Id;
		};
		bool IsEnabled() { return m_isEnabled; };

	protected :
		bool m_isEnabled;
		std::string m_Id;
		std::string m_type;


	};

}