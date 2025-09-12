#include "network/EM_Fetch.h"
#include <cstdio> 
#include "Object.h"
#include "Logger.h"
#include "AssetManager.h"
namespace cart {

	Dictionary<std::string, std::function<bool(std::string, ASYNC_CALLBACK_STATUS, const char*, int, void*)>> EM_Fetch::mCallbacks{};

	EM_Fetch::EM_Fetch() :  requestCount { 0 }
	{

	}
	EM_Fetch::~EM_Fetch()
	{
	}

    /// <summary>
    /// Callback funcion for Emscripten_Fetch command
    /// </summary>
    /// <param name="uid"></param>
    /// <param name="status"></param>
    /// <param name="data"></param>
    /// <param name="size"></param>
    /// <param name="userdata"></param>

    void EM_Fetch::HTTPCallback(std::string uid, ASYNC_CALLBACK_STATUS status, const char* data, int size, void* userdata) {


		for (auto iter = EM_Fetch::mCallbacks.begin(); iter != EM_Fetch::mCallbacks.end(); ++iter)
		{
			std::string url = { iter->first };	
			if (url.compare(uid) == 0)
			{
				if ((iter->second)(uid, status, data, size, userdata)) {
					if (status == OK || status== FAILED) {
						EM_Fetch::mCallbacks.erase(iter);
					}
					break;
				}
			}

		}
    }

	
#ifdef __EMSCRIPTEN__


	void EM_Fetch::Fetch_Succeeded(struct emscripten_fetch_t* fetch)
	{
		std::string url = { fetch->url };
		uint64_t numbytes = fetch->numBytes;
		ASYNC_CALLBACK_STATUS status = OK;
		EM_Fetch::HTTPCallback(url , status, fetch->data, (int)numbytes, fetch->userData);
		emscripten_fetch_close(fetch); // Also free data on failure.
	}

	void EM_Fetch::Fetch_Failed(struct emscripten_fetch_t* fetch)
	{
		std::string url = { fetch->url };
		ASYNC_CALLBACK_STATUS status = FAILED;
		EM_Fetch::HTTPCallback(url, status, fetch->data, 0, fetch->userData);
		emscripten_fetch_close(fetch); // Also free data on failure.
	}
	void EM_Fetch::Fetch_Progress(emscripten_fetch_t* fetch)
	{
		if (fetch->status != 200) {
			// Handle error or non-success status
			return;
		}
		
		if (fetch->totalBytes > 0) {
			int per = fetch->dataOffset * 100.0 / fetch->totalBytes;			
			std::string url = { fetch->url };			
			ASYNC_CALLBACK_STATUS progress = PROGRESS;
			EM_Fetch::HTTPCallback(url, progress, fetch->data, per, fetch->userData);
		}
		
	}
#endif // __EMSCRIPTEN__
}
