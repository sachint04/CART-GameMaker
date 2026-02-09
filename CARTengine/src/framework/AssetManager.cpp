#include "AssetManager.h"
#include "Application.h"
#include "World.h"
#include "Actor.h"
#include "HUD.h"
#include "network/network.h"
#include "Clock.h"
namespace cart {

#pragma region CONSTRUCTOR & INIT


    shared<AssetManager> AssetManager::assetManager{ nullptr };

    AssetManager& AssetManager::Get()
     {
         if (!assetManager) {
             assetManager = shared<AssetManager> {new AssetManager};
         }
         return *assetManager.get();
     }

    void AssetManager::Release()
    {
        assetManager = nullptr;
    }

    AssetManager::AssetManager() :Object{}, mPreloadCallbacks{}, m_isLoading{ false }, m_preloadlist{}
     {
        
     }

    void AssetManager::SetAssetRootDirectory(const std::string& directory)
     {
       if (ChangeDirectory(directory.c_str()))
        {            
            Logger::Get()->Trace(std::format("Working directory changed to {} syccessfully.",directory));
        }
        else {            
            Logger::Get()->Trace(std::format("FAILED!! TO change working directory  {}.", directory));
        }
     }
#pragma endregion

#pragma region LOAD/UNLOAD TEXTURE ASSET

    shared<Texture2D> AssetManager::LoadTextureAsset(const std::string &path, TEXTURE_DATA_STATUS status){
       return LoadTexture(path, m_textureLoadedMap, status);
    } 

    shared<Texture2D > AssetManager::LoadTexture(const std::string &path, Dictionary<std::string, TextureData>& constainer, TEXTURE_DATA_STATUS status)
    {
     
        auto found = constainer.find(path);
        if (found != constainer.end())
        {            
            return found->second.texture;
        }
       
        Image* image = new Image{ LoadImage(path.c_str()) };     // Loaded in CPU memory (RAM)
        
        if (!IsImageValid(*image)) {
           // std::string log_str = "ERROR!! cannot recognize file data , might be corrupted. Try another Image ";
            Logger::Get()->Warn(std::format("ERROR!! AssetManager::LoadTexture()  | Cannot recognize texture \n --[{}]\0",  path));
            return shared<Texture2D> {nullptr};
        }
        ImageFormat(image, 7);
        Texture2D texture = LoadTextureFromImage(*image);         // Image converted to texture, GPU memory (VRAM)    
        SetTextureFilter(texture, TEXTURE_FILTER_TRILINEAR); // Makes the texture smoother when upscaled

        try {           
            constainer.insert({ path,  { std::make_shared<Texture2D>(texture) , status } });
        }
        catch(const std::runtime_error& e){
            Logger::Get()->Error(std::format("ERROR!! cannot fine Image with path {}", path));
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
   
    shared<Texture2D> AssetManager::AddTexture( Image& image, std::string& path, TEXTURE_DATA_STATUS status)
    {
        auto found = m_textureLoadedMap.find(path);
        if (found != m_textureLoadedMap.end())
        {       
            Logger::Get()->Trace(std::format("AssetManager | AddTexture() Texture already available {}", path));
            return found->second.texture;
        }
        try {

            Texture2D texture = LoadTextureFromImage(image);
            m_textureLoadedMap.insert({ path,  { std::make_shared<Texture2D>(texture) , status} });
            if (status == TEXTURE_DATA_STATUS::LOCKED) {
                auto imgfound = m_imageLoadedMap.find(path);
                if (imgfound == m_imageLoadedMap.end()) {
                    m_imageLoadedMap.insert({ path, new Image{image} });
                }
                else {
                    imgfound->second = new Image{ image };
                }
            }
            else
            {
               UnloadImage(image);// Unload from CPU memory (RAM)
            }
            
            return m_textureLoadedMap.find(path)->second.texture;
        }
        catch (const std::runtime_error& e) {
            Logger::Get()->Error(std::format(" AssetManager::AddTexture() | ERROR!! cannot Add Texture with path {}", path.c_str()));
        }
        return shared<Texture2D >{nullptr};
    }

    bool AssetManager::ReplaceTextureFromImage(const std::string& path, Image image)
    {
        auto found = m_textureLoadedMap.find(path);
        if (found != m_textureLoadedMap.end())
        {
            UnloadTexture(*found->second.texture);
            Texture2D texture = LoadTextureFromImage(image);         // Image converted to texture, GPU memory (VRAM)    
            SetTextureFilter(texture, TEXTURE_FILTER_TRILINEAR);
            found->second.texture = std::make_shared<Texture2D>(texture);          
            return true;
        }
        return false;
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
            if ((copy.data == NULL) || (copy.width == 0) || (copy.height == 0)) {             
                Logger::Get()->Error("ERROR! MASKED ELEMENT REQUIRED BASE IMAGE POINTER!");
                return false;

            };
            ImageResize(&copy, width, height);
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

    bool AssetManager::ResizeTexture(const std::string& path, int width, int height)
    {

        auto foundimg = m_textureLoadedMap.find(path);
        if (foundimg != m_textureLoadedMap.end())
        {
            foundimg->second.texture.get()->width = width;
            foundimg->second.texture.get()->height = height;
            return true;
        }

        return false;
    }
   
    Image* AssetManager::GetImage(const std::string& path) {
        auto found = m_imageLoadedMap.find(path);
        if (found != m_imageLoadedMap.end()) {               
            return found->second;// Image found in list
        }        
        //shared<Texture2D> tex = LoadTexture(path, m_textureLoadedMap, TEXTURE_DATA_STATUS::LOCKED);// try to load fresh image
        //if (tex) {
        //    tex.reset();
        //    return GetImage(path);
        //}
        return nullptr;
    }

    bool AssetManager::UnloadTextureAsset(const std::string& path)
    {
        auto found = m_textureLoadedMap.find(path);
        if (found != m_textureLoadedMap.end()) {   
            if (found->second.texture->width != 0 && found->second.texture->height != 0) {
                UnloadTexture(*found->second.texture.get());
                found->second.texture.reset();
            }
            m_textureLoadedMap.erase(found);         
        }

        auto foundimg = m_imageLoadedMap.find(path);
        if (foundimg != m_imageLoadedMap.end()) {                
            if (foundimg->second->data != NULL) {                 
                if (IsImageValid(*foundimg->second))
                {
                    UnloadImage(*foundimg->second);
                }
            }
            m_imageLoadedMap.erase(foundimg);
        }
        return true;
                
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
            return found->second;
        }
      
        Font font = LoadFontEx(path.c_str(), fontSize, 0, 250);
      //  Font font = LoadFont(path.c_str());
        if(font.baseSize > 0){
            shared<Font> fontptr = std::make_shared<Font>(font);
             m_fontLoadedMap.insert({ path + strsize, fontptr});
             return fontptr; 
        }
        Logger::Get()->Error(std::format("AssetManager::LoadFontMap() | Error!! Failed to load font {} ", path));
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
            return true;
        }
         Logger::Get()->Error(std::format("AssetManager |UnloadFont()| ERROR! Failed to unload Font {} ", path));

        return false;
    }
#pragma endregion
    
#pragma region Event Listeners

    void AssetManager::OnPreloadAssetItemLoaded(std::string callbackId, std::string path, unsigned char* data, int size)
    {     
        if (data != nullptr)// store texture data in memory
        {
            std::string filetype = { ".png" };
            Image image = LoadImageFromMemory(filetype.c_str(), data, size);
            ImageFormat(&image, 7);
            if (IsImageValid(image))
            {
                Logger::Get()->Trace(std::format(" AssetManager::OnPreloadAssetItemLoaded()  Image valid. Add Texture  \n [{}] \0", path));
                AddTexture(image, path, LOCKED);
            }
            else {
               Logger::Get()->Error(std::format(" AssetManager::OnPreloadAssetItemLoaded() Invalid image \n[{}] \0", path));
            }
            UnloadFileData(data);
        
            std::vector<Preload_Data>::iterator iter = m_preloadlist.begin();// Unload loaded/failed path from the list
            if (iter != m_preloadlist.end())//  has preload data
            {
                if (iter->uid.compare(callbackId) == 0)
                {
                    iter->list.erase(iter->list.begin());// remove first path from list  
                }
            }
        }
        else {
            Logger::Get()->Error(std::format("AsssetManager:: OnPreloadAssetItemLoaded() FAILED item\n [{}]\0", path));
        }
            LoadAsset_Async();// call for next load;

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

    bool AssetManager::IsTextureAlive(const std::string& path)
    {
        auto bfound = m_textureLoadedMap.find(path);
        
        if (bfound != m_textureLoadedMap.end())return true;

        return false;
    }

    /// <summary>
    /// Load Preload list; Currently only 'Image' asset supported
    /// </summary>
    void AssetManager::LoadAsset_Async()
    {
        std::vector<Preload_Data>::iterator iter = m_preloadlist.begin();// take first "Preload_Data" object from list
        if (iter != m_preloadlist.end())//  has preload data
        {
             std::vector<std::string>::iterator list_iter = iter->list.begin();// get first path from list
             if (iter->list.size() > 0)// list still has assets to load
             {
                 std::string path = iter->list.at(0);// get first path                 
                 float progress = 1.f - ((float)iter->list.size() / (float)iter->count);// percentage loaded
                 Application::app->GetCurrentWorld()->GetHUD().lock()->ShowProgress(progress, std::string{ iter->loadmessage+ " - " + std::to_string((int)(progress * 100)) + "%"});

                 if (IsImageAlive(path) || IsTextureAlive(path))// texture available in memory
                 {
                    iter->list.erase(iter->list.begin());// remove first path from list                   
                     LoadAsset_Async();// start over      
                     return;// break execution since file found.
                 }
                 else // load fresh texture
                 {
#ifdef _WIN32
                    int dataSize = 0;
                    unsigned char* img = LoadFileData(iter->list.at(0).c_str(), &dataSize);                                   
                    OnPreloadAssetItemLoaded(iter->uid, iter->list.at(0), img, dataSize);// callback
                    return;// Offline load handler called, Job Done!
#endif // _WIN32

#ifdef __EMSCRIPTEN__
                     Application::net->LoadAsset(iter->uid, iter->list.at(0), GetWeakRef(), &AssetManager::OnPreloadAssetItemLoaded);
#endif // __EMSCRIPTEN__
                 }                 
             }
             else {// current list is empty
                 (iter->callback)();
                 m_preloadlist.erase(m_preloadlist.begin());// remove first list from "Preload_Data" list
                 LoadAsset_Async();// start over
             }
        }
        else {  // no data pending  
            m_isLoading = false;
            Application::app->GetCurrentWorld()->GetHUD().lock()->ShowProgress(1.f, "All asset Loading complete.\0");
           // (iter->callback)();// current list is empty so Fire callback
        }
       
      
    }


    bool AssetManager::IsImageAlive(const std::string& path)
    {       
        auto found = m_imageLoadedMap.find(path);
        if (found != m_imageLoadedMap.end())
        {
            return true;
        }
        return false;
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
       for (auto iter = m_textureLoadedMap.begin(); iter != m_textureLoadedMap.end();)
        {
            if (iter->second.texture.use_count() == 1 && iter->second.status == TEXTURE_DATA_STATUS::UNLOCKED) {
                auto foundimg = m_imageLoadedMap.find(iter->first);
                if (foundimg != m_imageLoadedMap.end()) {
                    if (foundimg->second)
                    {
                        UnloadImage(*foundimg->second);                        
                        foundimg->second = nullptr;
                    }
                    m_imageLoadedMap.erase(foundimg);  
                }                
                //Logger::Get()->Tace(std::format("AssetManager CleanCycle()  Texture  {}", iter->first),LOG_WARNING);
                UnloadTexture(*iter->second.texture);
                iter->second.texture.reset();
                iter = m_textureLoadedMap.erase(iter);
            }
            else {
                ++iter;
            }
        }

    }
    void AssetManager::ClearTextureMap()
    {
        for (auto iter = m_textureLoadedMap.begin(); iter != m_textureLoadedMap.end();)
        {                      
            UnloadTexture(*iter->second.texture);
             iter->second.texture.reset();             
              iter = m_textureLoadedMap.erase(iter);
          
        }
    }
    void AssetManager::ClearFontMap()
    {
        for (auto iter = m_fontLoadedMap.begin(); iter != m_fontLoadedMap.end();)
        {

            UnloadFont(*iter->second);            
            iter->second.reset();
            iter = m_fontLoadedMap.erase(iter);

        }
    }
    void AssetManager::ClearImageMap()
    {
        Dictionary<std::string, Image* >::iterator iter;
        for ( iter = m_imageLoadedMap.begin(); iter != m_imageLoadedMap.end();)
        {
            UnloadImage(*iter->second);
            iter->second = nullptr;
            iter = m_imageLoadedMap.erase(iter);
        }
    }
#pragma endregion
 }
