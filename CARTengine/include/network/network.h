/*
* Cart engine Netwok class
* 
* 
* 
*/

#pragma once
#include <cstdlib>
#include <cstring>
#include <string>
#include <iostream>
#include <functional>
#include <stdexcept>
#include "Object.h"
#include "Core.h"
#include <memory>
#include "Logger.h"
#include "Types.h"
#include "network/EM_Fetch.h"


#ifdef __cplusplus
extern "C" { 
#endif

#ifdef __EMSCRIPTEN__
    extern void GetHTTP(const char* uid, const char* url, const char* where);
    extern void PostHTTP(const char* uid,  const char* url, const char* data, const char* where );
    extern void Upload(const char* uid, const char* url, const char* dir, const uintptr_t* imageData, int w, int h, const char* filename);
    extern void LoadAssetHTTP(const char* uid, const char* url);
#endif

#ifdef __cplusplus
}
#endif

namespace cart {
    class network :public Object{
    public:
        network();
        ~network();

        void HTTPCallback(std::string& uid, std::string& response, std::string& data);
        void LoadAssetHTTPCallback(std::string& uid, std::string url,  unsigned char* data, int size);
        std::string GetUID();

        template<typename ClassName>
        std::string GET(std::string url, std::string where, weak<Object> obj, void(ClassName::* callback)(std::string, std::string));

        template<typename ClassName>
        std::string POST(std::string url, std::string data, std::string where, weak<Object> obj, void(ClassName::* callback)(std::string, std::string));

        template<typename ClassName>
        std::string UploadImage(std::string url, const uintptr_t* imageData, int width, int height, std::string dir, std::string filename, weak<Object> obj, void(ClassName::* callback)(std::string, std::string));

        template<typename ClassName>
        void LoadAsset(std::string id, std::string url, weak<Object> obj, void(ClassName::* callback)(std::string, std::string, unsigned char*, int));

      /*  template<typename ClassName>
        bool FetchAsset_Async(Async_Call_Header url, weak<Object> obj, void(ClassName::* callback)(Async_Call_Response));*/

        std::string GetHost();
        std::string GetURL();
        std::string GetPort();
        std::string GetPath();

    private:
        
        Dictionary<std::string, std::function<bool(std::string, std::string)>> mCallbacks;
        Dictionary<std::string, std::function<bool(std::string, std::string, unsigned char*, int)>> m_LoadAssetCallbacks;
        int requestCount;

    };


    template<typename ClassName>
    std::string network::GET(std::string url, std::string where, weak<Object> obj, void(ClassName::* callback)(std::string, std::string))
    {
        std::function<bool(std::string, std::string)> callbackFunc = [obj, callback](std::string response, std::string data)->bool
        {
            if (!obj.expired())
            {
                (static_cast<ClassName*>(obj.lock().get())->*callback)(response, data);
                return true;
            }

            return false;
        };
        //  if (!mCallbacks)mCallbacks = {};
        std::string uid = GetUID();
#ifdef __EMSCRIPTEN__
        const char* urlptr = url.c_str();
        const char* idptr = uid.c_str();
        const char* conptr = where.c_str();
        GetHTTP(idptr, urlptr, conptr);
        mCallbacks.insert({ uid, callbackFunc });
#endif
        return uid;
    }
    template<typename ClassName>
    std::string network::POST(std::string url,std::string data, std::string where, weak<Object> obj, void(ClassName::* callback)(std::string, std::string))
    {

        std::function<bool(std::string, std::string)> callbackFunc = [obj, callback](std::string response, std::string data)->bool
        {
            if (!obj.expired())
            {
                (static_cast<ClassName*>(obj.lock().get())->*callback)(response, data);
                return true;
            }

            return false;
        };
        //  if (!mCallbacks)mCallbacks = {};
        std::string uid = GetUID();
#ifdef __EMSCRIPTEN__
         const char* idptr = uid.c_str();
         const char* urlptr = url.c_str();
         const char* dataptr = data.c_str();
         const char* conptr = where.c_str();
         PostHTTP(idptr, urlptr, dataptr, conptr);
        mCallbacks.insert({ uid, callbackFunc });
#endif
        return uid;
    }

    template<typename ClassName>
    std::string network::UploadImage(std::string url, const uintptr_t* imageData, int width, int height, std::string dir, std::string filename, weak<Object> obj, void(ClassName::* callback)(std::string, std::string))
    {
        std::function<bool(std::string, std::string)> callbackFunc = [obj, callback](std::string response, std::string data)->bool
        {
            if (!obj.expired())
            {
                (static_cast<ClassName*>(obj.lock().get())->*callback)(response, data);
                return true;
            }

            return false;
        };
        //  if (!mCallbacks)mCallbacks = {};
        std::string uid = GetUID();
#ifdef __EMSCRIPTEN__
        const char* urlptr = url.c_str();
        const char* idptr = uid.c_str();
       // const char* dirptr = dir.c_str();
        Upload(idptr, urlptr, dir.c_str(), imageData, width, height, filename.c_str());
        mCallbacks.insert({ uid, callbackFunc });
#endif
        return uid;


    }

    template<typename ClassName>
    void network::LoadAsset(std::string id, std::string url, weak<Object> obj, void (ClassName::* callback)(std::string, std::string, unsigned char*, int))
    {
        Logger::Get()->Trace(std::format("Network LoadAsset() id {} url {} \n", id, url));
        std::function<bool(std::string, std::string, unsigned char*, int)> callbackFunc = [obj, callback](std::string id, std::string filename, unsigned char* data, int size)->bool
        {
            if (!obj.expired())
            {
                (static_cast<ClassName*>(obj.lock().get())->*callback)(id,filename, data, size);
                return true;
            }

            return false;
        };
        std::string nid = GetUID();
        std::string uid = std::string{ id + "," + nid};
        const char* urlptr = url.c_str();
        const char* idptr = uid.c_str();
        
        m_LoadAssetCallbacks.insert({ nid, callbackFunc });
#ifdef __EMSCRIPTEN__
        LoadAssetHTTP(idptr, urlptr);
#endif
       
    }

   // /// <summary>
   ///// Load Texture Async from URL string. Only User incase of '__EMSCRIPTEN__'
   ///// </summary>
   ///// <typeparam name="ClassName"></typeparam>
   ///// <param name="url"></param>
   ///// <param name="obj"></param>
   ///// <param name="callback"></param>
   ///// <returns></returns>
   // template<typename ClassName>
   // bool network::FetchAsset_Async(Async_Call_Header header, weak<Object> obj, void(ClassName::* callback)(Async_Call_Response))
   // {
   //     std::function<bool(Async_Call_Response)> callbackFunc = [obj, callback](Async_Call_Response response)->bool
   //     {
   //         if (!obj.expired())
   //         {
   //             (static_cast<ClassName*>(obj.lock().get())->*callback)(response);
   //             return true;
   //         }

   //         return false;
   //     };

   //     m_LoadAssetCallbacks.insert({ header.id, { header.id, header.location, header.texture_status, callbackFunc } });

   //     EM_Fetch f;
   //    
   //     try
   //     {
   //         f.LoadAsset(header.location, GetWeakRef(), &network::OnAsync_Fetch_Handler);
   //     }
   //     catch (const std::runtime_error& e) {
   //         std::cerr << "Caught runtime error: " << e.what() << std::endl;
   //         const char* error_message_cstr = e.what();
   //         
   //     }
   //     catch (const std::exception& e) { // Catches any other std::exception
   //         std::cerr << "Caught generic std::exception: " << e.what() << std::endl;
   //         const char* error_message_cstr = e.what();
   //        
   //     }
   //     catch (...) { // Catches any other type of exception
   //         std::cerr << "Caught unknown exception" << std::endl;
   //         Logger::Get()->Push("Caught unknown exception: ");
   //     }
   //     return true;
   // }


    inline std::string network::GetUID() {
        requestCount++;
        return  std::to_string(requestCount);       
    }
}