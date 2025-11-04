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
        bool UpdateTextureFromData(const std::string& path, Rectangle rect, Color* pixels);
        bool ResizeImage(const std::string& path, int width, int height);
        bool ResizeTexture(const std::string& path, int width, int height);
        Image* GetImage(const std::string& path);
        bool UnloadTextureAsset(const std::string& path);        
        bool UnloadFontAsset(const std::string& path , int fontsize);
        void ClearTextureMap();
        void ClearFontMap();
        void SetTextureStatus(const std::string& path, TEXTURE_DATA_STATUS status = TEXTURE_DATA_STATUS::UNLOCKED );
        bool IsTextureAlive(const std::string& path);
        bool IsImageAlive(const std::string& path);
        ~AssetManager();
        AssetManager();


        template<typename ClassName>
        void LoadAssetList(std::string id, std::vector<std::string>& list, weak<Object> obj, void(ClassName::* callback)());

    protected:

    private:
        List<Font> fonts= { };
        static shared<AssetManager> assetManager;       
        Dictionary<std::string, TextureData> m_textureLoadedMap;
        Dictionary<std::string, Image* > m_imageLoadedMap;
        Dictionary<std::string, shared<Font>> m_fontLoadedMap;
        Dictionary<std::string, std::function<bool()>> mPreloadCallbacks;
        std::vector<std::string> m_preloadlist;
        int preloadId = 0;
        void LoadAsset_Async(std::string id);
        shared<Texture2D> LoadTexture(const std::string& path, Dictionary<std::string, TextureData>& constainer, TEXTURE_DATA_STATUS status = TEXTURE_DATA_STATUS::UNLOCKED);        
        void OnPreloadAssetItemLoaded(std::string id, std::string path, unsigned char* data, int size);
        shared<Font> LoadFontMap(const std::string& path, int fontSize, Dictionary<std::string, shared<Font>>& constainer);    
    };


    template<typename ClassName>
    void AssetManager::LoadAssetList(std::string id, std::vector<std::string>& list, weak<Object> obj, void(ClassName::* callback)())
    {
        m_preloadlist = list;
        std::function<bool()> callbackFunc = [obj, callback]()->bool
        {
           Logger::Get()->Push(std::format("Async_Load Callback() wrapper expired {} \n", obj.expired()));
            if (!obj.expired())
            {
                (static_cast<ClassName*>(obj.lock().get())->*callback)();
                Logger::Get()->Push("Async_Load Callback() \n");
                return true;
            }
            Logger::Get()->Push("Async_Load Callback() Object expired!\n");
            return false;
        };
        std::string uid = std::to_string(preloadId++);

        mPreloadCallbacks.insert({ uid , callbackFunc});
#ifdef _WIN32 // Sync execution
        std::vector<std::string>::iterator iter;
        
        for (iter = list.begin(); iter != list.end();)
        {
            if (IsTextureAlive(*iter))
            {
                iter = list.erase(iter);
            }
            else {
                shared<Texture2D> texture= LoadTextureAsset(*iter, LOCKED);
                if (texture != nullptr) {
                    iter = list.erase(iter);
                }
                else {
                    iter++;
                }
            }

        }

        for (auto iter = mPreloadCallbacks.begin(); iter != mPreloadCallbacks.end();)
        {
            if (iter->first.compare(uid) == 0)
            {
                if ((iter->second)())
                {
                    mPreloadCallbacks.erase(iter);
                    m_preloadlist.clear();

                    break;
                }
            }
            else {
                iter++;
            }
        }
        #endif // _WIN32


        #ifdef __EMSCRIPTEN__
                LoadAsset_Async(uid);
        #endif // __EMSCRIPTEN___
                // Logger::Get()->Push(std::format("AssetManager | LoadAssetList() count {}", m_preloadlist.size()));

    }
}