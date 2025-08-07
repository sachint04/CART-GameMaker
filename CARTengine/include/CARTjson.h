#pragma once
#include <nlohmann/json.hpp>
#include <raylib.h>
#include <string>
using json = nlohmann::json;
namespace cart
{
	class CARTjson {
		public:
			static void readAPPData(const char* _file);
			static void readAPPData(const std::string& strm);

			static void readUserData(const char* _file);
			static void readUserData(const std::string& strm);

			static json& GetAppData() { return m_config; };
			static json& GetUserData();
			static json& GetSessionData();
			static void UpdateUserData(const json& data);
			static std::string GetUserDataString();
			~CARTjson();
		private:
			static json m_config;
			static json m_userdata;
			static json m_sessiondata;
	};

}