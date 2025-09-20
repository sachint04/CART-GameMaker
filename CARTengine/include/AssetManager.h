#pragma once
#include <map>
#include <iostream>
#include <memory>
#include <string>
#include <functional>
#include  <raylib.h>
#include "Types.h"

#ifdef __EMSCRIPTEN__
#include "network/EM_Fetch.h"
#endif // __EMSCRIPTEN__
#include "Object.h"

namespace cart {



    // asset manager 
    class AssetManager : public Object{
    public:
        static AssetManager& Get();
        static void Release();
        void Unload();
        void CleanCycle();
        void SetAssetRootDirectory(const std::string& directory);

        /// <summary>
        /// Load Texture2D from local FileSystem / EMSCRIPTEN preloaded Files
        /// </summary>
        /// <param name="path"></param>
        /// <param name="status"></param>
        /// <returns></returns>
        shared<Texture2D> LoadTextureAsset(const std::string &path, TEXTURE_DATA_STATUS status = TEXTURE_DATA_STATUS::UNLOCKED);

        /// <summary>
        /// Load font  from local File system / EMSCRIPTEN preloaded files
        /// </summary>
        /// <param name="path"></param>
        /// <param name="fontSize"></param>
        /// <returns></returns>
        shared<Font> LoadFontAsset(const std::string& path, int fontSize);
        /// <summary>
        /// Add Texture2D to current pool of Textures
        /// </summary>
        /// <param name="image"></param>
        /// <param name="path"></param>
        /// <param name="status"></param>
        /// <returns></returns>
        shared<Texture2D> AddTexture( Image& image, std::string& path, TEXTURE_DATA_STATUS status = TEXTURE_DATA_STATUS::UNLOCKED);
        bool UpdateTextureFromData(const std::string& path, Rectangle rect, Color* pixels);
        bool ResizeImage(const std::string& path, int width, int height);
        bool ResizeTexture(const std::string& path, int width, int height);
        Image* GetImage(const std::string& path);
        bool UnloadTextureAsset(const std::string& path);        
        bool UnloadFontAsset(const std::string& path , int fontsize);
        void ClearTextureMap();
        void ClearFontMap();
        void SetTextureStatus(const std::string& path, TEXTURE_DATA_STATUS status = TEXTURE_DATA_STATUS::UNLOCKED );


        ~AssetManager();
        AssetManager();
    protected:

    private:
        static shared<AssetManager> assetManager;
        std::string m_RootDirectory;
        Dictionary<std::string, TextureData> m_textureLoadedMap;
        Dictionary<std::string, Image*> m_imageLoadedMap;

        shared<Texture2D> LoadTexture(const std::string& path, Dictionary<std::string, TextureData>& constainer, TEXTURE_DATA_STATUS status = TEXTURE_DATA_STATUS::UNLOCKED);
        List<Font> fonts= { };
        
        Dictionary<std::string, shared<Font>> m_fontLoadedMap;
        shared<Font> LoadFontMap(const std::string& path, int fontSize, Dictionary<std::string, shared<Font>>& constainer);              
    };

   
}