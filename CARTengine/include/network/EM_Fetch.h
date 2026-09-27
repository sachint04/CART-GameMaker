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

	struct FetchContext {
		std::string uid;
		std::string url;
		std::string virtualpath;
		// Add any other data you need
	};

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
		static std::string LoadAsset(std::string uid, std::string url, std::string virtualpath, weak<Object> obj, void(ClassName::* callback)(std::string, std::string, std::string, ASYNC_CALLBACK_STATUS,int));

#ifdef  __EMSCRIPTEN__
		static void Fetch_Succeeded(struct emscripten_fetch_t* fetch);
		static void Fetch_Failed(struct emscripten_fetch_t* fetch);
		static void Fetch_Progress(struct emscripten_fetch_t* fetch);
#endif //  __EMSCRIPTEN__
		/*static void HTTPCallback(std::string uid, ASYNC_CALLBACK_STATUS status, const char* data, int progress, int totalbytes);*/
	private:
		
		static Dictionary<std::string, std::function<bool(std::string, std::string, std::string, ASYNC_CALLBACK_STATUS, int)>> mCallbacks;
		int requestCount;

	};

	template<typename ClassName>
	std::string EM_Fetch::LoadAsset(std::string uid, std::string url, std::string virtualpath, weak<Object> obj, void(ClassName::* callback)(std::string, std::string, std::string, ASYNC_CALLBACK_STATUS,int))
	{
		std::function<bool(std::string, std::string, std::string, ASYNC_CALLBACK_STATUS, int)> callbackFunc = [obj, callback](std::string id, std::string url, std::string vpath,  ASYNC_CALLBACK_STATUS status, int progress)->bool
		{
			if (!obj.expired())
			{
				(static_cast<ClassName*>(obj.lock().get())->*callback)(id, url, vpath, status, progress);
				return true;
			}
			return false;
		};
		EM_Fetch::mCallbacks.insert({ url, callbackFunc});
#ifdef __EMSCRIPTEN__
		emscripten_fetch_attr_t attr;
		emscripten_fetch_attr_init(&attr);
		strcpy(attr.requestMethod, "GET");
		attr.attributes = EMSCRIPTEN_FETCH_LOAD_TO_MEMORY;	

		FetchContext* context = new FetchContext();
		context->uid = uid;
		context->url = url;
		context->virtualpath = virtualpath;

		attr.userData = static_cast<void*>(context);
		attr.onprogress = Fetch_Progress;
		attr.onsuccess = Fetch_Succeeded;
		attr.onerror = Fetch_Failed;
		emscripten_fetch(&attr, url.c_str());
		Logger::Get()->Trace(std::format("Fetch LoadAsset fetch id {} ", url));
#endif // __EMSCRIPTEN__

		return url;
	}


	 
}