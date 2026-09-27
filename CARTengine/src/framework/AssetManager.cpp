#include "AssetManager.h"
#include "Application.h"
#include "World.h"
#include "Actor.h"
#include "HUD.h"
#include "network/network.h"
#include "Clock.h"
#include "utils/StringUtils.h"
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

    AssetManager::AssetManager() :Object{}, mPreloadCallbacks{}, m_isLoading{ false }, m_preloadlist{}, m_textureInfo{}
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
            if (m_textureInfo.contains(path)) {
                if (m_textureInfo[path].inqueue) {
                    Logger::Get()->Trace(std::format("AssetManager::LoadTexture() Asset is still loading path {} ", path));
                }
                return shared<Texture2D> {nullptr};
            }
#ifdef __EMSCRIPTEN__           
            Logger::Get()->Trace(std::format("AssetManager::LoadTexture() New Texture loading Web mode async mode  path {} ", path));
            m_textureInfo.insert({ path, { true, path } });
            LoadAsset_Async(path, path);
#endif
            
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
   
    shared<Texture2D> AssetManager::AddTexture( Image& image, const std::string& path, TEXTURE_DATA_STATUS status)
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
                Image copy = ImageCopy(image);
                if (imgfound == m_imageLoadedMap.end()) {
                    m_imageLoadedMap.insert({ path, new Image{copy} });
                }
                else {
                    imgfound->second = new Image{ copy };
                }
            }
            return m_textureLoadedMap.find(path)->second.texture;
        }
        catch (const std::runtime_error& e) {
            Logger::Get()->Error(std::format(" AssetManager::AddTexture() | ERROR!! cannot Add Texture with path {}", path.c_str()));
        }
        return shared<Texture2D >{nullptr};
    }

    shared<Texture2D> AssetManager::DuplicateTexture(const std::string& sourcepath, const std::string& targetpath, TEXTURE_DATA_STATUS status)
    {
        Logger::Get()->Trace(std::format("ERROR! AssetManager::DuplicateTexture() source {} target {}", sourcepath, targetpath));
        auto imgfind = m_imageLoadedMap.find(sourcepath);
        
        if (imgfind == m_imageLoadedMap.end()) {
            Logger::Get()->Trace(std::format("ERROR! AssetManager::DuplicateTexture() Image not found with source {} ", sourcepath));
            return shared<Texture2D >{nullptr};
        }
        // Image found 
        Image* img = AssetManager::Get().GetImage(sourcepath);
        
        // if target texture already existt
        auto find = m_textureLoadedMap.find(targetpath);
        if (find != m_textureLoadedMap.end()) {
            Logger::Get()->Trace(std::format("ERROR! AssetManager::DuplicateTexture() Texture with {} name is already exists source path {}", targetpath, sourcepath));
            return shared<Texture2D >{nullptr};
        }   
         return AddTexture(*img, targetpath, status);
    }

    bool AssetManager::ReplaceTextureFromImage(const std::string& path, Image& image, bool overrideImage)
    {
        bool success = false;
      //  Logger::Get()->Trace(std::format("AssetManager::ReplaceTextureFromImage() path {}", path));
        auto found = m_textureLoadedMap.find(path);
        if (found != m_textureLoadedMap.end())
        {
            UnloadTexture(*found->second.texture);
            Texture2D texture = LoadTextureFromImage(image); 
        // Image converted to texture, GPU memory (VRAM)    
            //     Logger::Get()->Trace(std::format("AssetManager::ReplaceTextureFromImage() Unloading previous Texture {}", path));
            //  GenTextureMipmaps(&texture);
            SetTextureFilter(texture, TEXTURE_FILTER_POINT);
            *found->second.texture = texture;

            success = true;

        }

        if (overrideImage) {
        //    Logger::Get()->Trace(std::format("AssetManager::ReplaceTextureFromImage() Overriding Image for the texture {}", path));
            ReplaceImage(path, image);
        }
        
        return success;
    }

    bool AssetManager::ReplaceImage(const std::string& path, Image& image)
    {
      //  Logger::Get()->Trace(std::format("AssetManager::ReplaceImage() path {}", path));
        auto found = m_imageLoadedMap.find(path);
        if (found != m_imageLoadedMap.end())
        {
       //     Logger::Get()->Trace(std::format("AssetManager::ReplaceImage() Unloading previous Image {}", path));
            UnloadImage(*found->second);
            Image copy = ImageCopy(image);           
            *found->second = copy;
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
                Logger::Get()->Trace(std::format("AssetManager UnloadTextureAsset()  Texture  {}", found->first));
                UnloadTexture(*found->second.texture);
                found->second.texture.reset();
            }
            m_textureLoadedMap.erase(found);         
        }

        auto foundimg = m_imageLoadedMap.find(path);
        if (foundimg != m_imageLoadedMap.end()) 
        { 
            if (foundimg->second != nullptr)            
            {              
                UnloadImage(*foundimg->second);                
                delete foundimg->second;             
            }
            m_imageLoadedMap.erase(foundimg);
        }
        return true;
                
    }
#pragma endregion
   
//#pragma region Load Shader 
//    shared<Shader> AssetManager::LoadShaderAsset(const std::string& vsPath, const std::string& fsPath)
//    {
//        std::string key = StringUtils::NormalizePath(fsPath); // Use FS path as primary key
//
//        if (m_shaderMap.find(key) != m_shaderMap.end()) {
//            return m_shaderMap[key];
//        }
//        try {
//
//            // Load from disk (Raylib handles the low-level I/O)
//            Shader rawShader = LoadShader(vsPath.empty() ? 0 : vsPath.c_str(), fsPath.c_str());
//
//            m_shaderMap.insert({ key, std::make_shared<Shader>(rawShader) });
//            return m_shaderMap.find(key)->second;
//        }
//        catch (const std::runtime_error& e) {
//            Logger::Get()->Error(std::format(" AssetManager::LoadShaderAsset() | ERROR!! cannot Add Shader with path {}", key.c_str()));
//        }
//        return shared<Shader >{nullptr};
//    }
//#pragma endregion


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
        if (data != nullptr)
        {        
            std::vector<Preload_Data>::iterator iter = m_preloadlist.begin();// Unload loaded/failed path from the list
            if (iter != m_preloadlist.end())//  has preload data
            {
                // preload list data found
                if (iter->uid == callbackId )
                {                  
                    auto find = std::find_if(iter->list.begin(), iter->list.end(), [path](External_Asset& asset)
                    {
                        return asset.url == path; // look for url
                    });
                    // found URL in the list
                    if (find != iter->list.end())
                    {
                        if (find->type == ASSET_IMAGE)
                        {
                            // store images and textures in memory
                            std::string filetype = { ".png" };
                            Image image = LoadImageFromMemory(filetype.c_str(), data, size);
                            ImageFormat(&image, 7);
                            if (IsImageValid(image))
                            {
                                Logger::Get()->Trace(std::format(" AssetManager::OnPreloadAssetItemLoaded()  Image valid. Add Texture  \n [{}] \0", path));
                                AddTexture(image, find->virtualpath, find->texture_status);
                            }
                            else {
                                Logger::Get()->Error(std::format(" AssetManager::OnPreloadAssetItemLoaded() Invalid image \n[{}] \0", path));
                            }
                        }
                        else
                        {
                            // Model and other assets are managed by EM_Fetch class in web mode
                        }
                        iter->list.erase(find);// remove current External_Asset from list  
                    }
                }
                          
            }
            UnloadFileData(data);
        }
        else {
            Logger::Get()->Error(std::format("AsssetManager:: OnPreloadAssetItemLoaded() FAILED item\n [{}]\0", path));
        }
            PreoadAsset_Async();// call for next load;

    }
    // IMP - will execute only in web mode
    void AssetManager::OnLoadAssetItemHandler(std::string callbackId, std::string path, unsigned char* data, int size)
    {
        if (data != nullptr)// store texture data in memory
        {
            std::string filetype = { ".png" };
            Image image = LoadImageFromMemory(filetype.c_str(), data, size);
            ImageFormat(&image, 7);
            if (IsImageValid(image))
            {
                Logger::Get()->Trace(std::format(" AssetManager::OnPreloadAssetItemLoaded()  Image valid. Add Texture  \n [{}] \0", path));
                AddTexture(image, path, UNLOCKED);
            }
            else {
                Logger::Get()->Error(std::format(" AssetManager::OnPreloadAssetItemLoaded() Invalid image \n[{}] \0", path));
            }
            UnloadFileData(data);

            //Remove Item from temp dictionary
            if (m_textureInfo.contains(callbackId)) {
                Logger::Get()->Trace(std::format("AssetManager::OnLoadAssetItemHandler() callbackId {} found", callbackId));
                m_textureInfo[callbackId].inqueue = false;
                m_textureInfo.erase(callbackId);
            }
            else {
                Logger::Get()->Trace(std::format("AssetManager::OnLoadAssetItemHandler() callbackId {} Not found", callbackId));
            }
        }
        else {
            Logger::Get()->Error(std::format("AsssetManager:: OnPreloadAssetItemLoaded() FAILED item\n [{}]\0", path));
        }
    }
  
    void AssetManager::OnPreloadAssetListHandler(std::string uid, std::string url, std::string vpath, ASYNC_CALLBACK_STATUS status, int progress)
    {
        std::vector<Preload_Data>::iterator iter = m_preloadlist.begin();// take first "Preload_Data" object from list
        if (iter != m_preloadlist.end())//  has preload data
        {
            std::vector<External_Asset>::iterator list_iter = iter->list.begin();// get first path from list
            auto find = std::find_if(iter->list.begin(), iter->list.end(), [url](External_Asset& asset)
            {
                return asset.url == url;
            });

            if (find != iter->list.end()) {
                iter->list.erase(find);
            }           
        }
        PreoadAsset_Async();
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

    bool AssetManager::IsAssetLocked(const std::string& path, Asset_Type asset_type)
    {
        bool result = false;

        if (asset_type == ASSET_IMAGE)
        {
            auto found = m_textureLoadedMap.find(path);
            if (found != m_textureLoadedMap.end()) {
                result = (found->second.status == LOCKED);
            }
        }
        else if (asset_type == ASSET_MODEL)
        {

           //TBD

        }
        else if (asset_type == ASSET_TEXT)
        {

           //TBD

        }

        return result;
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
    void AssetManager::PreoadAsset_Async()
    {
        std::vector<Preload_Data>::iterator iter = m_preloadlist.begin();// take first "Preload_Data" object from list
        if (iter != m_preloadlist.end())//  has preload data
        {
             std::vector<External_Asset>::iterator list_iter = iter->list.begin();// get first path from list
             if (iter->list.size() > 0)// list still has assets to load
             {
                 if (iter->list.at(0).type == ASSET_IMAGE)
                 {
                     std::string path = iter->list.at(0).url;// get first path                 
                     float progress = 1.f - ((float)iter->list.size() / (float)iter->count);// percentage loaded
                    // Application::app->GetCurrentWorld()->GetHUD().lock()->ShowProgress(progress, std::string{ iter->loadmessage + " - " + std::to_string((int)(progress * 100)) + "%" });

                     if (IsImageAlive(path) || IsTextureAlive(path))// texture available in memory
                     {
                         iter->list.erase(iter->list.begin());// remove first path from list                   
                         PreoadAsset_Async();// start over      
                         return;// break execution since file found.
                     }
                     else // load fresh texture
                     {
#ifdef _WIN32
                         int dataSize = 0;
                         unsigned char* img = LoadFileData(iter->list.at(0).url.c_str(), &dataSize);
                         OnPreloadAssetItemLoaded(iter->uid, iter->list.at(0).url, img, dataSize);// callback

                         return;// Offline load handler called, Job Done!
#endif // _WIN32

#ifdef __EMSCRIPTEN__
                         Application::net->LoadAsset(iter->uid, iter->list.at(0).url, GetWeakRef(), &AssetManager::OnPreloadAssetItemLoaded);
#endif // __EMSCRIPTEN__
                     }
                 }
                 else {
#ifdef _WIN32
                     
                        int bytesRead = 0;
                        std::string url = iter->list.at(0).url;
                        std::string vpath = iter->list.at(0).virtualpath;
                        unsigned char* fileData = LoadFileData(url.c_str(), &bytesRead);
                        if (fileData != NULL) {
                            if (SaveFileData(vpath.c_str(), fileData, bytesRead)) {
                                Logger::Get()->Trace(std::format(" AssetManager::PreoadAsset_Async() SUCCESS | url {} | virtual path {}", url, vpath));
                            }
                            else {
                                Logger::Get()->Error(std::format(" AssetManager::PreoadAsset_Async() FAILED | url {} | virtual path {}", url, vpath));
                            }
                            UnloadFileData(fileData);
                        }    
                        iter->list.erase(iter->list.begin());// remove first path from list                   
                        PreoadAsset_Async();// start over      
#endif // _WIN32
#ifdef __EMSCRIPTEN__
                    EM_Fetch::LoadAsset(iter->uid, iter->list.at(0).url, iter->list.at(0).virtualpath, GetWeakRef(), &AssetManager::OnPreloadAssetListHandler);
                        Logger::Get()->Trace(std::format("AssetManager::PreoadAsset_Async() url {} | vpath {}", iter->list.at(0).url, iter->list.at(0).virtualpath));
#endif // __EMSCRIPTEN__
                 }

             }
             else {// current list is empty
                 (iter->callback)();
                 m_preloadlist.erase(m_preloadlist.begin());// remove first list from "Preload_Data" list
                 PreoadAsset_Async();// start over
             }
        }
        else {  // no data pending  
            m_isLoading = false;
            Application::app->GetCurrentWorld()->GetHUD().lock()->ShowProgress(1.f, "All asset Loading complete.\0");
           // (iter->callback)();// current list is empty so Fire callback
        }
       
      
    }

    void AssetManager::LoadAsset_Async(std::string uid, std::string path)
    {
        
        #ifdef __EMSCRIPTEN__
               Application::net->LoadAsset(uid, path, GetWeakRef(), &AssetManager::OnLoadAssetItemHandler);
        #endif // __EMSCRIPTEN__
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
        ClearImageMap();
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
                Logger::Get()->Trace(std::format("AssetManager CleanCycle()  Texture  {}", iter->first));
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
            Logger::Get()->Trace(std::format("AssetManager ClearTextureMap()  Texture  {}", iter->first));
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
            if (iter->second != nullptr)
            {
                UnloadImage(*iter->second);
                delete iter->second;
            }           
            iter = m_imageLoadedMap.erase(iter);
        }
    }
#pragma endregion
 }
