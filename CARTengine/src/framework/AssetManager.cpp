#include "AssetManager.h"
#include <iostream> 
#include <raylib.h>
#include <string>
#include <memory>
#include <stdexcept>

 namespace cart {

#pragma region CONSTRUCTOR & INIT


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
        m_RootDirectory = directory + "/";
        if (ChangeDirectory(directory.c_str()))
        {
            LOG("Working directory changed to %s syccessfully.", directory.c_str());
        }
        else {

            LOG("FAILED!! TO change working directory  %s.", directory.c_str());
        }
     }
#pragma endregion

#pragma region LOAD/UNLOAD TEXTURE ASSET
    shared<Texture2D> AssetManager::LoadTextureAsset(const std::string &path, TEXTURE_DATA_STATUS status){
     //   LOG("|>=-=ASSETMANAGER =-=<| LoadTextureAsset() %s", path.c_str());
       return LoadTexture(path, m_textureLoadedMap, status);
    } 
    shared<Texture2D > AssetManager::LoadTexture(const std::string &path, Dictionary<std::string, TextureData>& constainer, TEXTURE_DATA_STATUS status)
    {
        auto found = constainer.find(path);
        if (found != constainer.end())
        {
         //  LOG("|>=-=ASSETMANAGER =-=<| %s Texture Found in Memory! %s ", path.c_str());       
            return found->second.texture;
        }
       
        Image* image = new Image{ LoadImage(path.c_str()) };     // Loaded in CPU memory (RAM)
        if (image->data == NULL || image->width == 0 || image->height == 0) {
            LOG("ERROR!! cannot recognize file data , might be corrupted. Try another Image ");
            return shared<Texture2D> {nullptr};
        }
        ImageFormat(image, 7);
        Texture2D texture = LoadTextureFromImage(*image);         // Image converted to texture, GPU memory (VRAM)       
        try {           
            constainer.insert({ path,  { std::make_shared<Texture2D>(texture) , status } });
            LOG("ASSETMANAGER | LoadTexture() found %s ", path.c_str());
        }
        catch(const std::runtime_error& e){
            LOG("ERROR!! cannot fine Image with path %s", path.c_str());
        }
        if (status == TEXTURE_DATA_STATUS::LOCKED) {
            auto imgfound = m_imageLoadedMap.find(path);
            if (imgfound == m_imageLoadedMap.end()) {
                m_imageLoadedMap.insert({ path, image });
            }
            else {
                UnloadImage(*image);// Unload from CPU memory (RAM)
            }
        }
        else {
            UnloadImage(*image);// Unload from CPU memory (RAM)
         }
       return constainer.find(path)->second.texture;

    }

    shared<Texture2D> AssetManager::AddTexture( Image image, std::string& path, TEXTURE_DATA_STATUS status)
    {
        auto found = m_textureLoadedMap.find(path);
        if (found != m_textureLoadedMap.end())
        {                 
            return found->second.texture;
        }
        try {
            Texture2D texture = LoadTextureFromImage(image);
            m_textureLoadedMap.insert({ path,  { std::make_shared<Texture2D>(texture) , status} });
            return m_textureLoadedMap.find(path)->second.texture;
        }
        catch (const std::runtime_error& e) {
            LOG("ERROR!! cannot Add Texture with path %s", path.c_str());
        }
        return shared<Texture2D >{nullptr};
    }

    bool AssetManager::UpdateTextureFromData(const std::string& path, Rectangle rect, Color* pixels)
    {           
        auto found = m_textureLoadedMap.find(path);
        if (found != m_textureLoadedMap.end())
        {                   
            UpdateTextureRec(*found->second.texture, rect, pixels);
           return true;
        }
        return false;
    }

    bool AssetManager::ResizeImage(const std::string& path, int width, int height)
    {
       
        auto foundimg = m_imageLoadedMap.find(path);
        if (foundimg != m_imageLoadedMap.end())
        {
            Image copy = ImageCopy(*foundimg->second);

            ImageResize(&copy, width, height);
            
            if ((copy.data == NULL) || (copy.width == 0) || (copy.height == 0)) {

                LOG("ERROR! MASKED ELEMENT REQUIRED BASE IMAGE POINTER!"); return false;

            };
            auto found = m_textureLoadedMap.find(path);
            if (found != m_textureLoadedMap.end())
            {
                UnloadTexture(*found->second.texture);
                found->second.texture.reset();
                Texture2D texture = LoadTextureFromImage(copy);
                found->second.texture = std::make_shared<Texture2D>(texture);
            }
            UnloadImage(copy);
            return true;
        }

        return false;
    }
   
    Image* AssetManager::GetImage(const std::string& path) {
        auto found = m_imageLoadedMap.find(path);
        if (found != m_imageLoadedMap.end()) {               
            return found->second;// Image found in list
        }        
        shared<Texture2D> tex = LoadTexture(path, m_textureLoadedMap, TEXTURE_DATA_STATUS::LOCKED);// try to load fresh image
        if (tex) {
            tex.reset();
            return GetImage(path);
        }
        return nullptr;
    }

    bool AssetManager::UnloadTextureAsset(const std::string& path)
    {
        auto found = m_textureLoadedMap.find(path);
        if (found == m_textureLoadedMap.end() || found->second.status == TEXTURE_DATA_STATUS::LOCKED) {
            LOG("AssetManage | UnloadTextureAsset() ERROR! Failed to unloded Texture  %s , \n Asset Not found or is LOCKED.", path.c_str());
            return false;
        }

        if (found != m_textureLoadedMap.end())
        {   
            LOG("AssetManager UnloadTextureAsset() Texture  %s ", found->first.c_str());

            auto foundimg = m_imageLoadedMap.find(path);
            if (foundimg != m_imageLoadedMap.end()) {
                LOG("AssetManager UnloadTextureAsset() Image  %s ", foundimg->first.c_str());
                UnloadImage(*foundimg->second);
                delete foundimg->second;
                m_imageLoadedMap.erase(foundimg);
            }

            UnloadTexture(*found->second.texture);
            found->second.texture.reset();
            m_textureLoadedMap.erase(found);
      
            return true;
        }
        return false;
    }
#pragma endregion
   
#pragma region LOAD/UNLOAD FONT ASSETS
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
       //     LOG(" font Found %s ", path.c_str()); 
             return fontptr; 
        }
   //     LOG("Failed to load font %s ", path.c_str());
        return shared<Font> {nullptr};
    }
    bool AssetManager::UnloadFontAsset(const std::string& path, int fontsize)
    {
        std::string strsize = std::to_string(fontsize);
        auto found = m_fontLoadedMap.find(path + strsize);
        if (found != m_fontLoadedMap.end())
        {
            UnloadFont(*found->second);
            found->second.reset();
            m_fontLoadedMap.erase(found);
            //LOG("AssetManager | UnloadFont() | %s  %ld | Success!", path.c_str(), fontsize);
            return true;
        }
        // LOG("AssetManager |UnloadFont()| ERROR! Failed to unload Font %s ", path.c_str());

        return false;
    }
#pragma endregion
    
    
#pragma region HELPERS

    void AssetManager::SetTextureStatus(const std::string& path, TEXTURE_DATA_STATUS status) {
        auto found = m_textureLoadedMap.find(path);
        if (found != m_textureLoadedMap.end())
        {
            found->second.status = status;
        }
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

     //   LOG("|>=-=ASSETMANAGER =-=<| CleanCycle()  ");
       for (auto iter = m_textureLoadedMap.begin(); iter != m_textureLoadedMap.end();)
        {
       //         LOG("TextureMap  %s User Count  %lu ", iter->first.c_str(), iter->second.use_count());
            if (iter->second.texture.use_count() == 1 && iter->second.status == TEXTURE_DATA_STATUS::UNLOCKED) {
                auto foundimg = m_imageLoadedMap.find(iter->first);
                if (foundimg != m_imageLoadedMap.end()) {
                    UnloadImage(*foundimg->second);
                  foundimg->second = nullptr;
                    m_imageLoadedMap.erase(foundimg);  
                }

                LOG("AssetManager CleanCycle()  Texture  %s", iter->first.c_str());
                UnloadTexture(*iter->second.texture);
                iter->second.texture.reset();
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
    //   LOG("Current Live Texture count %zu", m_textureLoadedMap.size());
    //   LOG("Current Live Font count %zu", m_fontLoadedMap.size());

    //   LOG("|>=-=ASSETMANAGER =-=<| CleanCycle() END!! ");
    }
    void AssetManager::ClearTextureMap()
    {
        for (auto iter = m_textureLoadedMap.begin(); iter != m_textureLoadedMap.end();)
        {
            LOG("AssetManager ClearTextureMap()  Texture %s", iter->first.c_str());
            UnloadTexture(*iter->second.texture);
             iter->second.texture.reset();
             //LOG("%s Cleared Texture Map.", iter->first.c_str());
               iter = m_textureLoadedMap.erase(iter);
          
        }
     
    }
    void AssetManager::ClearFontMap()
    {
        for (auto iter = m_fontLoadedMap.begin(); iter != m_fontLoadedMap.end();)
        {

            UnloadFont(*iter->second);
            LOG("AssetManager | ClearFontMap() | %s  | Success!", iter->first.c_str());
            iter->second.reset();
            //LOG("%s Cleared Font Map.", iter->first.c_str());
            iter = m_fontLoadedMap.erase(iter);

        }
    }
#pragma endregion
 }
