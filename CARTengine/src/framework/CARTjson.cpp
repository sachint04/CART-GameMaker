#include "CARTjson.h"

namespace cart
{
	json CARTjson::m_envsetting = {};
	json CARTjson::m_app_config = {};
	json CARTjson::m_config = {};
	json CARTjson::m_userdata = {};
	json CARTjson::m_templateinfo = {};
	json CARTjson::m_sessiondata = {};

	json& CARTjson::readAPPData(const char* _file)
	{
		char* strm = LoadFileText(_file);
		m_config = json::parse(strm);
		delete strm;
		return m_config;

	}

	json& CARTjson::readAPPData(const std::string& strm)
	{
		m_config = json::parse(strm);
		return m_config;
	}

	json& CARTjson::readUserData(const char* _file)
	{
		char* strm = LoadFileText(_file);
		m_userdata = json::parse(strm);
		delete strm;
		return m_userdata;
	}

	json& CARTjson::readUserData(const std::string& strm)
	{
		m_userdata = json::parse(strm);
		return m_userdata;
	}

	json& CARTjson::readAppConfigData(const std::string& strm)
	{
		m_app_config = json::parse(strm);
		return m_app_config;
	}

	json& CARTjson::readTemplateInfo(const std::string& strm)
	{

		m_templateinfo = json::parse(strm);
		return m_templateinfo;

	}
	
	json& CARTjson::readTemplateInfo(const char* _file)
	{
		char* strm = LoadFileText(_file);
		m_templateinfo = json::parse(strm);
		delete strm;
		return m_templateinfo;
	}

	json& CARTjson::GetUserData() { 
		return m_userdata;
	}

	json& CARTjson::GetSessionData()
	{
		return m_sessiondata;
	}
	
	void CARTjson::UpdateUserData(const json& json) {
		m_userdata = json;
	}

	json& CARTjson::GetTemplateInfo()
	{
		return m_templateinfo;
	}

	json& CARTjson::readEnvSettings(std::string _info)
	{		
		m_envsetting = json::parse(_info);
		return m_envsetting;		
	}

	json& CARTjson::GetEnvSettings()
	{
		return m_envsetting;
	}

	std::string CARTjson::GetUserDataString() {
		return m_userdata.dump();
	}

	CARTjson::~CARTjson()
	{
		m_userdata.clear();
		m_sessiondata.clear();
		m_config.clear();
	}
}