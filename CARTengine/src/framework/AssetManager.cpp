#include "AssetManager.h"
#include <iostream> 
#include <raylib.h>
#include <string>
#include <memory>
 namespace cart {
    unique<AssetManager> AssetManager::assetManager{ nullptr };

    AssetManager& AssetManager::Get()
     {
         if (!assetManager) {
             assetManager = unique<AssetManager> {new AssetManager};
         }
         return *assetManager;
     }

    void AssetManager::Release()
    {
      //  LOG("AssetManager Deleted!");
        assetManager = nullptr;
    }

    AssetManager::AssetManager() : m_RootDirectory{}
     {

     }

    void AssetManager::SetAssetRootDirectory(const std::string& directory)
     {
         m_RootDirectory = directory;
         ChangeDirectory(m_RootDirectory.c_str());
     }

    shared<Texture2D> AssetManager::LoadTextureAsset(const std::string &path){  
       return LoadTexture( path, m_textureLoadedMap);
    }
    
    shared<Texture2D > AssetManager::LoadTexture(const std::string &path, Dictionary<std::string, shared<Texture2D >>& constainer)
    {
        auto found = constainer.find(path);
        if (found != constainer.end())
        {
           // LOG("%s Texture Found in Memory!", path.c_str());
            return found->second;
        }
       
        Image image = LoadImage(path.c_str());     // Loaded in CPU memory (RAM)
        Texture2D tex = LoadTextureFromImage(image);         // Image converted to texture, GPU memory (VRAM)

        shared<Texture2D> texture = std::make_shared<Texture2D>(tex);
        UnloadImage(image);// Loaded in CPU memory (RAM)
        if (texture)
        {
            //LOG("IO %s Texture Loaded!", path.c_str());
            constainer.insert({ path, texture });           
            return texture;
        }

        return shared<Texture2D > {nullptr};
    }
    
#pragma region LOAD FONT
    shared<Font> AssetManager::LoadFontAsset(const std::string& path, int fontSize) {
        return LoadFont(path, fontSize, m_fontLoadedMap);
    }

    AssetManager::~AssetManager()
    {
     
    }


    shared<Font> AssetManager::LoadFont(const std::string& path, int fontSize, Dictionary<std::string, shared<Font>>& constainer)
    {
        std::string strsize = std::to_string(fontSize);
        auto found = m_fontLoadedMap.find(path + strsize);
        if (found != m_fontLoadedMap.end())
        {
            //LOG(" font Found in memory%s ", path.c_str());
            return found->second;
        }
      
        Font font = LoadFontEx(path.c_str(), fontSize, 0, 250);
        if(font.baseSize > 0){
            shared<Font> fontptr = std::make_shared<Font>(font);
             m_fontLoadedMap.insert({ path + strsize, fontptr});
             return fontptr; 
        }
        
        return shared<Font> {nullptr};
    }
#pragma endregion

#pragma region  Cleanup



    void AssetManager::Unload()
    {
        ClearTextureMap();
        ClearFontMap();
    }

    void AssetManager::CleanCycle()
    {
       for (auto iter = m_textureLoadedMap.begin(); iter != m_textureLoadedMap.end();)
        {
            if (iter->second.use_count() == 1) {

                //LOG("cleaning texture %s", iter->first.c_str());
                iter = m_textureLoadedMap.erase(iter);
            }
            else {
                ++iter;
            }
        }

       for (auto iter = m_fontLoadedMap.begin(); iter != m_fontLoadedMap.end();)
       {
           if (iter->second.use_count() == 1) {

               //LOG("cleaning font %s", iter->first.c_str());
               //            UnloadFont(fnt);
               iter = m_fontLoadedMap.erase(iter);

           }
           else {
               ++iter;
           }
       }
       for (int i = 0; i < fonts.size(); i++) UnloadFont(fonts[i]);
    }

    void AssetManager::ClearTextureMap()
    {
        for (auto iter = m_textureLoadedMap.begin(); iter != m_textureLoadedMap.end();)
        {
             iter->second.reset();
             //LOG("%s Cleared Texture Map.", iter->first.c_str());
               iter = m_textureLoadedMap.erase(iter);
          
        }
    }

    void AssetManager::ClearFontMap()
    {
        for (auto iter = m_fontLoadedMap.begin(); iter != m_fontLoadedMap.end();)
        {
            iter->second.reset();
            //LOG("%s Cleared Font Map.", iter->first.c_str());
            iter = m_fontLoadedMap.erase(iter);

        }
    }
#pragma endregion
 }
