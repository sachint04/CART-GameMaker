#include "AssetManager.h"
#include <iostream> 
#include <raylib.h>
#include <string>
#include <memory>
#include <stdexcept>
#include "Logger.h"

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
      //  Logger::Get()->Push("AssetManager Deleted!");
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
          //  Logger::Get()->Push(std::format("Working directory changed to {} syccessfully.",directory));
        }
        else {            
         //   Logger::Get()->Push(std::format("FAILED!! TO change working directory  {}.", directory));
        }
     }
#pragma endregion

#pragma region LOAD/UNLOAD TEXTURE ASSET
    shared<Texture2D> AssetManager::LoadTextureAsset(const std::string &path, TEXTURE_DATA_STATUS status){
     //   Logger::Get()->Push("|>=-=ASSETMANAGER =-=<| LoadTextureAsset() %s", path.c_str());
       return LoadTexture(path, m_textureLoadedMap, status);
    } 
    shared<Texture2D > AssetManager::LoadTexture(const std::string &path, Dictionary<std::string, TextureData>& constainer, TEXTURE_DATA_STATUS status)
    {
        auto found = constainer.find(path);
        if (found != constainer.end())
        {
         //  Logger::Get()->Push("|>=-=ASSETMANAGER =-=<| %s Texture Found in Memory! %s ", path.c_str());       
            return found->second.texture;
        }
       
        Image* image = new Image{ LoadImage(path.c_str()) };     // Loaded in CPU memory (RAM)
        if (image->data == NULL || image->width == 0 || image->height == 0) {
            std::string log_str = "ERROR!! cannot recognize file data , might be corrupted. Try another Image ";
            Logger::Get()->Push(std::format("ERROR!! cannot recognize file data  {}", path));
            return shared<Texture2D> {nullptr};
        }
        ImageFormat(image, 7);
        Texture2D texture = LoadTextureFromImage(*image);         // Image converted to texture, GPU memory (VRAM)       
        try {           
            constainer.insert({ path,  { std::make_shared<Texture2D>(texture) , status } });
            
          //  Logger::Get()->Push(std::format("ASSETMANAGER | LoadTexture() found {} ", path));
        }
        catch(const std::runtime_error& e){
            Logger::Get()->Push(std::format("ERROR!! cannot fine Image with path {}", path));
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
         //   Logger::Get()->Push(std::format("ERROR!! cannot Add Texture with path {}", path.c_str()));
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
                std::string log_str = "ERROR! MASKED ELEMENT REQUIRED BASE IMAGE POINTER!";
              //  Logger::Get()->Push(log_str);
                return false;

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
          //  Logger::Get()->Push(std::format("AssetManage | UnloadTextureAsset() ERROR! Failed to unloded Texture  {} , \n Asset Not found or is LOCKED.", path));
            return false;
        }

        if (found != m_textureLoadedMap.end())
        {               
           // Logger::Get()->Push(std::format("AssetManager UnloadTextureAsset() Texture  {} ", found->first));

            auto foundimg = m_imageLoadedMap.find(path);
            if (foundimg != m_imageLoadedMap.end()) {                
               // Logger::Get()->Push(std::format("AssetManager UnloadTextureAsset() Image  {} ", foundimg->first));
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
           //Logger::Get()->Push(" font Found in memory {} ", path);
            return found->second;
        }
      
        Font font = LoadFontEx(path.c_str(), fontSize, 0, 250);
      //  Font font = LoadFont(path.c_str());
        if(font.baseSize > 0){
            shared<Font> fontptr = std::make_shared<Font>(font);
             m_fontLoadedMap.insert({ path + strsize, fontptr});
       //     Logger::Get()->Push(" font Found %s ", path.c_str()); 
             return fontptr; 
        }
   //     Logger::Get()->Push("Failed to load font {} ", path);
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
            //Logger::Get()->Push("AssetManager | UnloadFont() | {}  {} | Success!", path, fontsize);
            return true;
        }
        // Logger::Get()->Push("AssetManager |UnloadFont()| ERROR! Failed to unload Font {} ", path);

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

     //   Logger::Get()->Push("|>=-=ASSETMANAGER =-=<| CleanCycle()  ");
       for (auto iter = m_textureLoadedMap.begin(); iter != m_textureLoadedMap.end();)
        {
       //         Logger::Get()->Push("TextureMap  %s User Count  %lu ", iter->first.c_str(), iter->second.use_count());
            if (iter->second.texture.use_count() == 1 && iter->second.status == TEXTURE_DATA_STATUS::UNLOCKED) {
                auto foundimg = m_imageLoadedMap.find(iter->first);
                if (foundimg != m_imageLoadedMap.end()) {
                    UnloadImage(*foundimg->second);
                  foundimg->second = nullptr;
                    m_imageLoadedMap.erase(foundimg);  
                }                
              //  Logger::Get()->Push(std::format("AssetManager CleanCycle()  Texture  {}", iter->first));
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
           Logger::Get()->Push("Font Map  %s User Count  {} ", iter->first.c_str(), iter->second.use_count());
           if (iter->second.use_count() == 1) {

               Logger::Get()->Push("Cleaning Font Map  %s", iter->first.c_str());
               //            UnloadFont(fnt);
               iter->second.reset();
               iter = m_fontLoadedMap.erase(iter);

           }
           else {
               ++iter;
           }
       }
       */
    //   Logger::Get()->Push("Current Live Texture count %zu", m_textureLoadedMap.size());
    //   Logger::Get()->Push("Current Live Font count %zu", m_fontLoadedMap.size());

    //   Logger::Get()->Push("|>=-=ASSETMANAGER =-=<| CleanCycle() END!! ");
    }
    void AssetManager::ClearTextureMap()
    {
        for (auto iter = m_textureLoadedMap.begin(); iter != m_textureLoadedMap.end();)
        {            
          //  Logger::Get()->Push(std::format("AssetManager ClearTextureMap()  Texture {}", iter->first));
            UnloadTexture(*iter->second.texture);
             iter->second.texture.reset();
             //Logger::Get()->Push("%s Cleared Texture Map.", iter->first.c_str());
               iter = m_textureLoadedMap.erase(iter);
          
        }
     
    }
    void AssetManager::ClearFontMap()
    {
        for (auto iter = m_fontLoadedMap.begin(); iter != m_fontLoadedMap.end();)
        {

            UnloadFont(*iter->second);            
           // Logger::Get()->Push(std::format("AssetManager | ClearFontMap() | {}  | Success!", iter->first));
            iter->second.reset();
            //Logger::Get()->Push("%s Cleared Font Map.", iter->first.c_str());
            iter = m_fontLoadedMap.erase(iter);

        }
    }
#pragma endregion
 }
