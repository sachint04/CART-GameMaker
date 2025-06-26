#include "CARTjson.h"

namespace cart
{
	json CARTjson::m_config = {};
	json CARTjson::m_userdata = {};

	void CARTjson::read(const char* _file)
	{
		char* strm = LoadFileText(_file);
		m_config = json::parse(strm);
	}

	void CARTjson::read(const std::string& strm)
	{
		m_config = json::parse(strm);
	}

	json& CARTjson::GetUserData() { 
		return m_userdata;
	}
	
	void CARTjson::UpdateUserData(const json& json) {
		m_userdata = json;
	}
}