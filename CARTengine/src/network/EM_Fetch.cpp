#include "network/EM_Fetch.h"
#include <cstdio> 
#include "Object.h"
#include "Logger.h"
#include "AssetManager.h"


namespace cart {

	Dictionary<std::string, std::function<bool(std::string, ASYNC_CALLBACK_STATUS, const char*, int, int)>> EM_Fetch::mCallbacks{};


	 std::string EM_Fetch::GetHost() {
		std::string result;
#ifdef __EMSCRIPTEN__
		emscripten::val location = emscripten::val::global("location");
		result = location["hostname"].as<std::string>();
#endif // __EMSCRIPTEN__
		return result;
	}

	 std::string EM_Fetch::GetURL() {
		std::string result;
#ifdef __EMSCRIPTEN__
		emscripten::val location = emscripten::val::global("location");
		result = location["href"].as<std::string>();
#endif // __EMSCRIPTEN__
		return result;
	}

	std::string EM_Fetch::GetPort() {
		std::string result;
#ifdef __EMSCRIPTEN__
		emscripten::val location = emscripten::val::global("location");
		result = location["href"].as<std::string>();
#endif // __EMSCRIPTEN__
		return result;
	}

	std::string EM_Fetch::GetPath() {
		std::string result;
#ifdef __EMSCRIPTEN__
		emscripten::val location = emscripten::val::global("location");
		result = location["pathname"].as<std::string>();
#endif // __EMSCRIPTEN__
		return result;
	}


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

    void EM_Fetch::HTTPCallback(std::string uid, ASYNC_CALLBACK_STATUS status, const char* data, int progress, int totalbytes) {


		for (auto iter = EM_Fetch::mCallbacks.begin(); iter != EM_Fetch::mCallbacks.end(); ++iter)
		{
			std::string url = { iter->first };	
			if (url.compare(uid) == 0)
			{
				if ((iter->second)(uid, status, data, progress, totalbytes)) {
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
		uint64_t totalBytes = fetch->totalBytes;
		ASYNC_CALLBACK_STATUS status = OK;
		EM_Fetch::HTTPCallback(url , status, fetch->data, 0, (int)totalBytes);
		emscripten_fetch_close(fetch); // Also free data on failure.
	}

	void EM_Fetch::Fetch_Failed(struct emscripten_fetch_t* fetch)
	{
		std::string url = { fetch->url };
		ASYNC_CALLBACK_STATUS status = FAILED;
		const char* nodata = { nullptr };
		EM_Fetch::HTTPCallback(url, status, nodata, 0, -1 );
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
			ASYNC_CALLBACK_STATUS status = PROGRESS;
			const char* nodata = { nullptr };
			EM_Fetch::HTTPCallback(url, status, nodata, per, fetch->totalBytes);
		}
		
	}
#endif // __EMSCRIPTEN__
}
