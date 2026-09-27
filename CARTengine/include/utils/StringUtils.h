#pragma once
#include <string>
#include <algorithm>
#include <cctype>

namespace cart {
	class StringUtils {
	public:
        /**
    * @brief Converts a string to lowercase for case-insensitive map keys.
    * Useful for cross-platform file paths and material names.
    */
        static std::string ToLower(std::string data) {
            std::transform(data.begin(), data.end(), data.begin(),
                [](unsigned char c) { return std::tolower(c); });
            return data;
        }

        /**
         * @brief Normalizes file paths for cross-platform compatibility.
         * Replaces backslashes with forward slashes (Web/Linux/iOS requirement).
         */
        static std::string NormalizePath(std::string path) {
            std::string result = ToLower(path);
            std::replace(result.begin(), result.end(), '\\', '/');
            return result;
        }

        /**
         * @brief Remove white space and trim.         
         */
        static std::string Trim(std::string s)
        {
            s.erase(0, s.find_first_not_of(" \t\n\r\f\v"));
            s.erase(s.find_last_not_of(" \t\n\r\f\v") + 1);
            return s;
        };
	};	

}