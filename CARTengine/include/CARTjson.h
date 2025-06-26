#pragma once
#include <nlohmann/json.hpp>
#include <raylib.h>
#include <string>
using json = nlohmann::json;
namespace cart
{
	class CARTjson {
		public:
			static void read(const char* _file);
			static void read(const std::string& strm);
			static json& GetAppData() { return m_config; };
			static json& GetUserData();
			static void UpdateUserData(const json& data);
		private:
			static json m_config;
			static json m_userdata;
	};

}