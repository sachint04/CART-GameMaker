#include "CARTjson.h"

namespace cart
{
	json CARTjson::m_config = {};
	json CARTjson::m_userdata = {};

	void CARTjson::readAPPData(const char* _file)
	{
		char* strm = LoadFileText(_file);
		m_config = json::parse(strm);
		delete strm;
	}

	void CARTjson::readAPPData(const std::string& strm)
	{
		m_config = json::parse(strm);
	}

	void CARTjson::readUserData(const char* _file)
	{
		char* strm = LoadFileText(_file);
		m_userdata = json::parse(strm);
		delete strm;
	}

	void CARTjson::readUserData(const std::string& strm)
	{
		m_userdata = json::parse(strm);
	}

	json& CARTjson::GetUserData() { 
		return m_userdata;
	}
	
	void CARTjson::UpdateUserData(const json& json) {
		m_userdata = json;
	}
}