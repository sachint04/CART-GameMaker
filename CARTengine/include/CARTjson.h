#pragma once
#include <nlohmann/json.hpp>
#include <raylib.h>
#include <string>
using json = nlohmann::json;
namespace cart
{
	class CARTjson {
		public:
			static json& readAPPData(const char* _file);
			static json& readAPPData(const std::string& strm);

			static json& readUserData(const char* _file);
			static json& readUserData(const std::string& strm);

			static json& readTemplateInfo(const char* _file);
			static json& readTemplateInfo(const std::string& strm);

			static json& GetAppData() { return m_config; };
			static json& GetUserData();
			static json& GetSessionData();			
			static void UpdateUserData(const json& data);
			static std::string GetUserDataString();
			static json& GetTemplateInfo();
			static json& readEnvSettings(std::string _info);
			static json& GetEnvSettings();
			~CARTjson();
		private:
			static json m_envsetting;
			static json m_config;
			static json m_userdata;
			static json m_sessiondata;
			static json m_templateinfo;
	};

}