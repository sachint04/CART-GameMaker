#pragma once
#include <map>
#include <functional>
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include  <raylib.h>
#include "Types.h"
#include "Object.h"
#include "Logger.h"
#include "Application.h"
#include "HUD.h"
#include "Core.h"
namespace cart {



    // asset manager 
    class Actor;
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
      //  shared<Texture2D> UpdateTexture( Image& image, std::string& path, TEXTURE_DATA_STATUS status = TEXTURE_DATA_STATUS::UNLOCKED);
                     
        bool ReplaceTextureFromImage(const std::string& path, Image image);
        bool UpdateTextureFromData(const std::string& path, Rectangle rect, Color* pixels);
        bool ResizeImage(const std::string& path, int width, int height);
        bool ResizeTexture(const std::string& path, int width, int height);
        Image* GetImage(const std::string& path);
        bool UnloadTextureAsset(const std::string& path);        
        bool UnloadFontAsset(const std::string& path , int fontsize);
        void ClearTextureMap();
        void ClearFontMap();
        void ClearImageMap();
        void SetTextureStatus(const std::string& path, TEXTURE_DATA_STATUS status = TEXTURE_DATA_STATUS::UNLOCKED );
        bool IsTextureAlive(const std::string& path);
        bool IsImageAlive(const std::string& path);
        ~AssetManager();
        AssetManager();


        template<typename ClassName>
        void LoadAssetList(std::string id, std::vector<std::string>& list, const std::string& loadmessage, weak<Object> obj, void(ClassName::* callback)());

    protected:

    private:
        List<Font> fonts= { };
        static shared<AssetManager> assetManager;       
        Dictionary<std::string, TextureData> m_textureLoadedMap;
        Dictionary<std::string, Image* > m_imageLoadedMap;
        Dictionary<std::string, shared<Font>> m_fontLoadedMap;
        Dictionary<std::string, std::function<bool()>> mPreloadCallbacks;
        std::vector<Preload_Data> m_preloadlist;
      
        int preloadId = 0;
        bool m_isLoading;
        void LoadAsset_Async();

        shared<Texture2D> LoadTexture(const std::string& path, Dictionary<std::string, TextureData>& constainer, TEXTURE_DATA_STATUS status = TEXTURE_DATA_STATUS::UNLOCKED);        
        shared<Font> LoadFontMap(const std::string& path, int fontSize, Dictionary<std::string, shared<Font>>& constainer);    

        void OnPreloadAssetItemLoaded(std::string callbackId, std::string path, unsigned char* data, int size);
       
    };


    template<typename ClassName>
    void AssetManager::LoadAssetList(std::string id, std::vector<std::string>& list,  const std::string& loadmessage, weak<Object> obj, void(ClassName::* callback)())
    {
        std::string uid = std::to_string(preloadId++);

        std::function<bool()> callbackFunc = [obj, callback]()->bool
        {
            if (!obj.expired())
            {
                (static_cast<ClassName*>(obj.lock().get())->*callback)();
                return true;
            }
            return false;
        };

        //--------- Prepare data to load---------------
        Preload_Data data = {};
        data.uid = uid;
        data.count = list.size();
        data.list = list;
        data.loadmessage = loadmessage;
        data.callback = callbackFunc;
        // ------------------------
        m_preloadlist.push_back(data);// dont start  another load requrest while loading is in progress

        if (m_isLoading)return;
        m_isLoading = true;

        LoadAsset_Async();

    }
    
   
   
}