#pragma once
#include <map>
#include <iostream>
#include  <raylib.h>
#include "Core.h"

namespace cart {

// asset manager 
    class AssetManager {
    public:
        static AssetManager& Get();
        static void Release();
        void Unload();
        void CleanCycle();
        void SetAssetRootDirectory(const std::string& directory);
        shared<Texture2D> LoadTextureAsset(const std::string &path);
        shared<Font> LoadFontAsset(const std::string& path, int fontSize);
        bool UnloadTextureAsset(const std::string& path);
        ~AssetManager();
    protected:
        AssetManager();

    private:
        std::string m_RootDirectory;
        static unique<AssetManager> assetManager;
        Dictionary<std::string, shared<Texture2D>> m_textureLoadedMap;
        shared<Texture2D> LoadTexture(const std::string& path, Dictionary<std::string, shared<Texture2D>>& constainer);
        List<Font> fonts= { };
        
        Dictionary<std::string, shared<Font>> m_fontLoadedMap;
        shared<Font> LoadFontMap(const std::string& path, int fontSize, Dictionary<std::string, shared<Font>>& constainer);       
        void  ClearTextureMap();
        void  ClearFontMap();

        
    };

}