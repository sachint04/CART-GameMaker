#pragma once
#include <map>
#include <iostream>
#include <memory>
#include <functional>
#include  <raylib.h>
#include "Core.h"
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
        shared<Texture2D> AddTexture( Image image, std::string& path, TEXTURE_DATA_STATUS status = TEXTURE_DATA_STATUS::UNLOCKED);
        bool UpdateTextureFromData(const std::string& path, Rectangle rect, Color* pixels);
        bool ResizeImage(const std::string& path, int width, int height);
        bool ResizeTexture(const std::string& path, int width, int height);
        Image* GetImage(const std::string& path);
        bool UnloadTextureAsset(const std::string& path);        
        bool UnloadFontAsset(const std::string& path , int fontsize);
        void ClearTextureMap();
        void ClearFontMap();
        void SetTextureStatus(const std::string& path, TEXTURE_DATA_STATUS status = TEXTURE_DATA_STATUS::UNLOCKED );

        template<typename ClassName>
       bool LoadTextureAsync(std::string url, weak<Object> obj, void(ClassName::* callback)(std::string, ASYNC_CALLBACK_STATUS, shared<Texture2D>, float), TEXTURE_DATA_STATUS userdata = TEXTURE_DATA_STATUS::LOCKED);

        ~AssetManager();
        AssetManager();
    protected:

    private:
        static shared<AssetManager> assetManager;
        std::string m_RootDirectory;
        Dictionary<std::string, TextureData> m_textureLoadedMap;
        Dictionary<std::string, Image*> m_imageLoadedMap;

        void OnAsync_Texture_Handler(std::string uid, ASYNC_CALLBACK_STATUS status, const char* data, int size, void* userdata);
        shared<Texture2D> LoadTextureFromMemory(std::string uid, const char* data, int size, void* userdata);
        shared<Texture2D> LoadTexture(const std::string& path, Dictionary<std::string, TextureData>& constainer, TEXTURE_DATA_STATUS status = TEXTURE_DATA_STATUS::UNLOCKED);
        List<Font> fonts= { };
        
        Dictionary<std::string, shared<Font>> m_fontLoadedMap;
        shared<Font> LoadFontMap(const std::string& path, int fontSize, Dictionary<std::string, shared<Font>>& constainer);              
        Dictionary<std::string, std::function<bool(std::string, ASYNC_CALLBACK_STATUS, shared<Texture2D>, float)>> mTextureAsyncCallbacks;
    };

    /// <summary>
    /// Load Texture Async from URL string
    /// </summary>
    /// <typeparam name="ClassName"></typeparam>
    /// <param name="url"></param>
    /// <param name="obj"></param>
    /// <param name="callback"></param>
    /// <returns></returns>
    template<typename ClassName>
    bool AssetManager::LoadTextureAsync(std::string url, weak<Object> obj, void(ClassName::* callback)(std::string, ASYNC_CALLBACK_STATUS, shared<Texture2D>, float), TEXTURE_DATA_STATUS userdata)
    {
        auto found = m_textureLoadedMap.find(url);
        if (found != m_textureLoadedMap.end())
        {
              //Logger::Get()->Push("|>=-=ASSETMANAGER =-=<| %s Texture Found in Memory! %s ", url.c_str());       
            (static_cast<ClassName*>(obj.lock().get())->*callback)(url, OK, found->second.texture,100);
            return true;
        }

        std::function<bool(std::string, ASYNC_CALLBACK_STATUS, shared<Texture2D>, float)> callbackFunc = [obj, callback](std::string url, ASYNC_CALLBACK_STATUS status, shared<Texture2D> texture, float progress)->bool
        {
            if (!obj.expired())
            {
                (static_cast<ClassName*>(obj.lock().get())->*callback)(url, status, texture, progress);
                return true;
            }

            return false;
        };
        mTextureAsyncCallbacks.insert({ url, callbackFunc });
        
#ifdef __EMSCRIPTEN__
        void* userdata_ptr = reinterpret_cast<void*>(static_cast<uintptr_t>(userdata));
        EM_Fetch f;
        f.LoadAsset(url, AssetManager::Get().GetWeakRef(), &AssetManager::OnAsync_Texture_Handler, userdata_ptr);
#endif // __EMSCRIPTEN__

#ifdef  _WIN32
        // Return Fail in Destop Mode 
        //TO DO  implement ASIO 
        (static_cast<ClassName*>(obj.lock().get())->*callback)(url, FAILED, shared<Texture2D>{nullptr}, 0);
#endif //  _WIN32

      
        return false;
    }
}