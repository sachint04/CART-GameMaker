#include "AssetManager.h"
#include <iostream> 
#include <raylib.h>
#include <string>
#include <memory>
#include <stdexcept>
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
     //   LOG("|>=-=ASSETMANAGER =-=<| LoadTextureAsset() %s", path.c_str());
       return LoadTexture( path, m_textureLoadedMap);
    }
    
    shared<Texture2D > AssetManager::LoadTexture(const std::string &path, Dictionary<std::string, shared<Texture2D >>& constainer)
    {
        auto found = constainer.find(path);
        if (found != constainer.end())
        {
        //   LOG("|>=-=ASSETMANAGER =-=<| %s Texture Found in Memory!", path.c_str());
          //  shared<Texture2D> weak_texture = found->second;
            return found->second;
        }
       
        Image image = LoadImage(path.c_str());     // Loaded in CPU memory (RAM)
        Texture2D texture = LoadTextureFromImage(image);         // Image converted to texture, GPU memory (VRAM)       
        UnloadImage(image);// Loaded in CPU memory (RAM)
        
        //LOG("IO %s Texture Loaded!", path.c_str());
        try {
            constainer.insert({ path, std::make_shared<Texture2D>(texture) });
           // shared<Texture2D> t = constainer.find(path)->second;
         //   LOG("|>=-=ASSETMANAGER =-=<| Creating  TextureMap  %s User Count  %lu ", path.c_str(), constainer.find(path)->second.use_count());
            return constainer.find(path)->second;

        }
        catch(const std::runtime_error& e){
            LOG("ERROR!! cannot fine Image with path %s", path.c_str());
        }
       
        return shared<Texture2D >{nullptr};

        
           
        

    }
    
    bool AssetManager::UnloadTextureAsset(const std::string& path)
    {
        auto found = m_textureLoadedMap.find(path);
        if (found != m_textureLoadedMap.end())
        {   
            UnloadTexture(*found->second);
            found->second.reset();
            m_textureLoadedMap.erase(found);
            return true;
        }
        LOG("AssetManager |UnloadTextureAsset()| ERROR! Failed to unload Texture %s ", path.c_str());
        return false;
    }
#pragma region LOAD FONT
    shared<Font> AssetManager::LoadFontAsset(const std::string& path, int fontSize) {
        return LoadFontMap(path, fontSize, m_fontLoadedMap);
    }

    shared<Font> AssetManager::LoadFontMap(const std::string& path, int fontSize, Dictionary<std::string, shared<Font>>& constainer)
    {
        std::string strsize = std::to_string(fontSize);
        auto found = m_fontLoadedMap.find(path + strsize);
        if (found != m_fontLoadedMap.end())
        {
            //LOG(" font Found in memory%s ", path.c_str());
            return found->second;
        }
      
        Font font = LoadFontEx(path.c_str(), fontSize, 0, 250);
      //  Font font = LoadFont(path.c_str());
        if(font.baseSize > 0){
            shared<Font> fontptr = std::make_shared<Font>(font);
             m_fontLoadedMap.insert({ path + strsize, fontptr});
             return fontptr; 
        }
        
        return shared<Font> {nullptr};
    }
#pragma endregion

#pragma region  Cleanup


    AssetManager::~AssetManager()
    {

    }
    void AssetManager::Unload()
    {
        ClearTextureMap();
        ClearFontMap();
    }

    void AssetManager::CleanCycle()
    {

        LOG("|>=-=ASSETMANAGER =-=<| CleanCycle()  ");
       for (auto iter = m_textureLoadedMap.begin(); iter != m_textureLoadedMap.end();)
        {
                LOG("TextureMap  %s User Count  %lu ", iter->first.c_str(), iter->second.use_count());
            if (iter->second.use_count() == 1) {

                LOG("Cleaning TextureMap  %s", iter->first.c_str());
                UnloadTexture(*iter->second);
                iter->second.reset();
                
                iter = m_textureLoadedMap.erase(iter);
            }
            else {
                ++iter;
            }
        }

       /*
       for (auto iter = m_fontLoadedMap.begin(); iter != m_fontLoadedMap.end();)
       {
           LOG("Font Map  %s User Count  %lu ", iter->first.c_str(), iter->second.use_count());
           if (iter->second.use_count() == 1) {

               LOG("Cleaning Font Map  %s", iter->first.c_str());
               //            UnloadFont(fnt);
               iter->second.reset();
               iter = m_fontLoadedMap.erase(iter);

           }
           else {
               ++iter;
           }
       }
       */
       LOG("Current Live Texture count %zu", m_textureLoadedMap.size());
       LOG("Current Live Font count %zu", m_fontLoadedMap.size());

       LOG("|>=-=ASSETMANAGER =-=<| CleanCycle() END!! ");
    }

    void AssetManager::ClearTextureMap()
    {
        for (auto iter = m_textureLoadedMap.begin(); iter != m_textureLoadedMap.end();)
        {
            UnloadTexture(*iter->second);
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
