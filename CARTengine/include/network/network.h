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
#include <functional>
#include "Core.h"
#include <memory>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

#ifdef __cplusplus
extern "C" { 
#endif

#ifdef __EMSCRIPTEN__
    extern void GetHTTP(const char* uid, const char* url);
    extern void POSTHTTP(const char* uid,  const char* url, const char* data );
    extern void Upload(const char* uid, const char* url, const uintptr_t* imageData, int w, int h, const char* filename);
#endif

#ifdef __cplusplus
} 
#endif

namespace cart{
    class Object;
     class network{
       public:
           network();
           ~network();

           void HTTPCallback(std::string& uid, std::string& response, std::string& data);     
           std::string GetUID();

           template<typename ClassName>
           std::string GET(std::string url, weak<Object> obj, void(ClassName::* callback)(std::string, std::string));

           template<typename T, typename ClassName>
           std::string POST(std::string url,std::string data, weak<Object> obj, void(ClassName::* callback)(std::string, std::string));

           template<typename ClassName>
           std::string UploadImage(std::string url, const uintptr_t* imageData, int width, int height, std::string filename, weak<Object> obj, void(ClassName::* callback)(std::string, std::string));
       private:
            Dictionary<std::string, std::function<bool(std::string, std::string)>> mCallbacks;
            int requestCount;
         
    };


    template<typename ClassName>
    std::string network::GET(std::string url, weak<Object> obj, void(ClassName::* callback)(std::string, std::string ))
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
        GetHTTP(idptr, urlptr);
        #endif
        mCallbacks.insert({ uid, callbackFunc });
        return uid;
    }
    template<typename T, typename ClassName>
    std::string network::POST(std::string url, std::string data, weak<Object> obj, void(ClassName::* callback)(std::string, std::string))
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
       // const char* urlptr = url.c_str();
    //    const char* idptr = uid.c_str();
    //    const char* data = data.c_str();
   //     PostHTTP(idptr, urlptr, data);
#endif
      //  mCallbacks.insert({ uid, callbackFunc });
        return uid;
    }

    template<typename ClassName>
    std::string network::UploadImage( std::string url, const uintptr_t* imageData, int width, int height, std::string filename, weak<Object> obj, void(ClassName::* callback)(std::string, std::string))
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
        Upload(idptr, urlptr, imageData, width, height, filename.c_str());
#endif
        mCallbacks.insert({ uid, callbackFunc });
        return uid;
    }

    inline std::string network::GetUID() {
        requestCount++;
        return  std::to_string(requestCount);       
    }
}