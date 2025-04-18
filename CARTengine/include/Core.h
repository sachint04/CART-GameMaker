#pragma once
#include <iostream>
#include <string>
#include <memory>
#include <map>
#include <unordered_map>
#include <unordered_set>

extern float APP_SCALE;
extern int FONT_SIZE;
extern std::string FONT_NAME;
namespace cart {
    template<typename T>
    using unique = std::unique_ptr<T>;

    template<typename T>
    using shared = std::shared_ptr<T>;

    template<typename T>
    using weak = std::weak_ptr<T>;

    template<typename T>
    using List = std::vector<T>;

    template<typename T>
    using Set = std::unordered_set<T>;

    template<typename keyType, typename valType, typename Pr = std::less<keyType>>
    using Map = std::map<keyType, valType, Pr>;

    template<typename keyType, typename valType, typename hasher = std::hash<keyType>>
    using Dictionary = std::unordered_map<keyType, valType, hasher>;

    using uint8 = unsigned char;

    // a macro
#define LOG(M, ...) printf(M "\n", ##__VA_ARGS__)

}