#pragma once
#include <nlohmann/json.hpp>
#include <raylib.h>
#include <string>
#include "Core.h"
#include "Logger.h"
namespace cart
{

    struct ResultStatus {
        bool success = true;
        std::string error_message = "";
    };

    // Wrapper container for the returned value and error tracking
    template <typename T>
    struct GetValueResult {
        T value{};
        ResultStatus status;
    };


	class CARTjson {
		public:
			static json& readAPPData(const char* _file);
			static json& readAPPData(const std::string& strm);

			static json& readUserData(const char* _file);
			static json& readUserData(const std::string& strm);


			static json& readTemplateInfo(const char* _file);
			static json& readTemplateInfo(const std::string& strm);

			static json& GetAppData();
			static json& GetUserData();
			static json& GetSessionData();			
			static void UpdateUserData(const json& data);
			static std::string GetUserDataString();
			static std::string GetAppDataString();
			static json& GetTemplateInfo();
			static json& readEnvSettings(std::string _info);
			static json& GetEnvSettings();

            static std::string_view ParseArrayAccess(std::string_view segment, size_t& out_index, bool& has_array);

            template <typename T>
           static GetValueResult<T> GetValue(const json& jsonObj, std::string_view prop_path);

			~CARTjson();
		private:
			static json m_envsetting;
			static json m_app_config;
			static json m_config;
			static json m_userdata;
			static json m_sessiondata;
			static json m_templateinfo;
	};

    inline std::string_view CARTjson::ParseArrayAccess(std::string_view segment, size_t& out_index, bool& has_array) {
        has_array = false;
        size_t open_bracket = segment.find('[');
        size_t close_bracket = segment.find(']');

        if (open_bracket != std::string_view::npos && close_bracket != std::string_view::npos && close_bracket > open_bracket) {
            std::string_view index_str = segment.substr(open_bracket + 1, close_bracket - open_bracket - 1);

            // Convert string_view to integer safely
            out_index = 0;
            for (char c : index_str) {
                if (!std::isdigit(c)) throw std::runtime_error("Validation Error: Invalid array index notation");
                out_index = out_index * 10 + (c - '0');
            }

            has_array = true;
            return segment.substr(0, open_bracket); // Return just the key name (e.g., "data")
        }
        return segment; // No array found, return original segment
    }

    template <typename T>
    GetValueResult<T> CARTjson::GetValue(const json& jsonObj, std::string_view prop_path) {
        size_t delimiter_pos = prop_path.find('.');
        std::string_view current_segment = (delimiter_pos == std::string_view::npos) ? prop_path : prop_path.substr(0, delimiter_pos);
        std::string_view remaining_path = (delimiter_pos == std::string_view::npos) ? "" : prop_path.substr(delimiter_pos + 1);

        // Parse for potential array access like "data[0]"
        size_t array_index = 0;
        bool has_array = false;
        std::string current_key(ParseArrayAccess(current_segment, array_index, has_array));

        // 1. Validate existence of the base key
        if (!jsonObj.contains(current_key)) {
            throw std::runtime_error("Validation Error: Missing property '" + current_key + "'");
        }

        const auto& current_node = jsonObj.at(current_key);

        // 2. Handle node navigation (Array vs Object)
        if (has_array) 
        {
            if (!current_node.is_array()) {
                Logger::Get()->Error(std::format("Validation Error: Property {} is expected to be an array", current_key));
                throw std::runtime_error("Validation Error: Property '" + current_key + "' is expected to be an array");
            }
            if (array_index >= current_node.size()) {
                Logger::Get()->Error(std::format("Validation Error: Array index [ {} ] out of bounds for {} ", array_index, current_key));
                throw std::runtime_error("Validation Error: Array index [" + std::to_string(array_index) + "] out of bounds for '" + current_key + "'");
            }

            // Move the target pointer to the specific array element
            const auto& array_element = current_node.at(array_index);

            // Check if we are at the end of the total path string
            if (remaining_path.empty()) {
                try {
                    return GetValueResult<T>{ array_element.get<T>() };
                }
                catch (const json::type_error& e) {
                    Logger::Get()->Error(std::format("Type Error at key {} | index [ {} ]': ", current_key, array_index));
                    throw std::runtime_error("Type Error at '" + current_key + "[" + std::to_string(array_index) + "]': " + e.what());
                }
            }

            // Otherwise, tail-recurse deeper into the array element object
            return GetValue<T>(array_element, remaining_path);
        }

        // 3. Handle standard object navigation (No arrays involved)
        if (remaining_path.empty()) {
            try {
                return GetValueResult<T>{ current_node.get<T>() };
            }
            catch (const json::type_error& e) {
                Logger::Get()->Error(std::format("Type Error for key {} ", array_index ));
                throw std::runtime_error("Type Error for '" + current_key + "': " + e.what());
            }
        }

        if (!current_node.is_object()) {
            Logger::Get()->Error(std::format("Validation Error : Property ' {} ' must be an object to access subproperties ", current_key));
            throw std::runtime_error("Validation Error: Property '" + current_key + "' must be an object to access subproperties");
        }

        return GetValue<T>(current_node, remaining_path);
    }
}