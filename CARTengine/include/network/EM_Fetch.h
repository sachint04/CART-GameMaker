#pragma once
/*
* Cart engine Netwok For EMScripten (WEB) class
*
*
*
*/

#include <stdio.h>
#include <string.h>

#ifdef  __EMSCRIPTEN__
#include <emscripten.h>
#include <emscripten/val.h>
#include <emscripten/fetch.h>
#endif //  __EMSCRIPTEN__


#include <functional>
#include "Core.h"
#include <raylib.h>
#include "Types.h"
#include "Logger.h"
namespace cart
{

	
	class Object;
	class EM_Fetch{

	public:
		EM_Fetch();
		~EM_Fetch();

	static std::string GetHost();
	static std::string GetURL();
	static std::string GetPort();
	static std::string GetPath();

		template<typename ClassName>
		std::string LoadAsset(std::string url, weak<Object> obj, void(ClassName::* callback)(std::string, ASYNC_CALLBACK_STATUS, const char* , int, int));

#ifdef  __EMSCRIPTEN__
		static void Fetch_Succeeded(struct emscripten_fetch_t* fetch);
		static void Fetch_Failed(struct emscripten_fetch_t* fetch);
		static void Fetch_Progress(struct emscripten_fetch_t* fetch);
#endif //  __EMSCRIPTEN__
		
		static void HTTPCallback(std::string uid, ASYNC_CALLBACK_STATUS status, const char* data, int progress, int totalbytes);
	private:
		
		static Dictionary<std::string, std::function<bool(std::string, ASYNC_CALLBACK_STATUS, const char*, int, int)>> mCallbacks;
		int requestCount;

	};



	//std::string uid, ASYNC_CALLBACK_STATUS status, const char* data, int progress, int totalBytes
	template<typename ClassName>
	std::string EM_Fetch::LoadAsset(std::string url, weak<Object> obj, void(ClassName::* callback)(std::string, ASYNC_CALLBACK_STATUS, const char* , int, int))
	{
		Logger::Get()->Trace(std::format("EM_Fetch | LoadAssets() location {}", url));
		std::function<bool(std::string, ASYNC_CALLBACK_STATUS, const char*, int, int)> callbackFunc = [obj, callback](std::string uid, ASYNC_CALLBACK_STATUS status, const char* data, int progress, int totalbytes)->bool
		{
			if (!obj.expired())
			{
				(static_cast<ClassName*>(obj.lock().get())->*callback)(uid,status, data, progress, totalbytes);
				return true;
			}

			return false;
		};
#ifdef __EMSCRIPTEN__

		EM_Fetch::mCallbacks.insert({ url, callbackFunc});
		
		emscripten_fetch_attr_t attr;
		emscripten_fetch_attr_init(&attr);
		strcpy(attr.requestMethod, "GET");
	
		attr.attributes = EMSCRIPTEN_FETCH_LOAD_TO_MEMORY;		
		attr.onprogress = Fetch_Progress;
		attr.onsuccess = Fetch_Succeeded;
		attr.onerror = Fetch_Failed;
		emscripten_fetch(&attr, url.c_str());
		Logger::Get()->Trace(std::format("Fetch LoadAsset fetch id {} ", url));
#endif // __EMSCRIPTEN__

		return url;
	}


	 
}