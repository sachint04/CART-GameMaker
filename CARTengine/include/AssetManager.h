#pragma once
#include <map>
#include <iostream>
#include  <raylib.h>
#include "Core.h"
#include "Types.h"
namespace cart {


    // asset manager 
    class AssetManager {
    public:
        static AssetManager& Get();
        static void Release();
        void Unload();
        void CleanCycle();
        void SetAssetRootDirectory(const std::string& directory);
        shared<Texture2D> LoadTextureAsset(const std::string &path, TEXTURE_DATA_STATUS status = TEXTURE_DATA_STATUS::UNLOCKED);
        shared<Font> LoadFontAsset(const std::string& path, int fontSize);
        shared<Texture2D> AddTexture( Image image, std::string& path, TEXTURE_DATA_STATUS status = TEXTURE_DATA_STATUS::UNLOCKED);
        bool UpdateTextureFromData(const std::string& path, Rectangle rect, Color* pixels);
        bool ResizeImage(const std::string& path, int width, int height);
        Image* GetImage(const std::string& path);
        bool UnloadTextureAsset(const std::string& path);        
        bool UnloadFontAsset(const std::string& path , int fontsize);
        void ClearTextureMap();
        void ClearFontMap();
        void SetTextureStatus(const std::string& path, TEXTURE_DATA_STATUS status = TEXTURE_DATA_STATUS::UNLOCKED );
        ~AssetManager();
    protected:
        AssetManager();

    private:
        std::string m_RootDirectory;
        static unique<AssetManager> assetManager;
        Dictionary<std::string, TextureData> m_textureLoadedMap;
        Dictionary<std::string, Image*> m_imageLoadedMap;
        shared<Texture2D> LoadTexture(const std::string& path, Dictionary<std::string, TextureData>& constainer, TEXTURE_DATA_STATUS status = TEXTURE_DATA_STATUS::UNLOCKED);
        List<Font> fonts= { };
        
        Dictionary<std::string, shared<Font>> m_fontLoadedMap;
        shared<Font> LoadFontMap(const std::string& path, int fontSize, Dictionary<std::string, shared<Font>>& constainer);       

        
    };

}