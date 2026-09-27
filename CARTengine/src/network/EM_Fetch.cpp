#include "network/EM_Fetch.h"
#include <cstdio> 
#include "Object.h"
#include "Logger.h"
#include "AssetManager.h"


namespace cart {

	Dictionary<std::string, std::function<bool(std::string, std::string, std::string, ASYNC_CALLBACK_STATUS, int)>> EM_Fetch::mCallbacks{};


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

  /*  void EM_Fetch::HTTPCallback(std::string uid, ASYNC_CALLBACK_STATUS status, const char* data, int progress, int totalbytes) {


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
    }*/


#ifdef __EMSCRIPTEN__
	void EM_Fetch::Fetch_Succeeded(struct emscripten_fetch_t* fetch)
	{
		FetchContext* context = static_cast<FetchContext*>(fetch->userData);
		if (fetch->data != NULL && fetch->numBytes > 0)
		{
			const char* vpath = context->virtualpath.c_str(); // "assets/cards/template01/card.glb"
			const char* dirPath = GetDirectoryPath(vpath);   // returns "assets/cards/template01"

			if (dirPath != nullptr && strlen(dirPath) > 0)
			{
				// Recursively create the full directory tree
				// This is safe even if some parts of the path already exist
				EM_ASM({
					try {
						FS.mkdirTree(UTF8ToString($0));
					}
					 catch (e) {
						 // Silently handle cases where path already exists or is root
					}
				}, dirPath);
			}

			// 1. Save to the virtual filesystem (MEMFS/IDBFS)
			void* dataPtr = const_cast<char*>(fetch->data);
			if(SaveFileData(context->virtualpath.c_str(), dataPtr, static_cast<int>(fetch->numBytes)))
			{
				Logger::Get()->Trace(std::format(" EM_Fetch::Fetch_Succeeded() SUCCESS | url {} | virtual path {}", context->url, context->virtualpath));
				
				if (FileExists(vpath))
				{
					// Verify file size matches expected download size
					int savedSize = GetFileLength(vpath);

					if (savedSize == (int)fetch->numBytes) {
						Logger::Get()->Trace(std::format("VERIFIED | {} exists and size matches ({} bytes)", vpath, savedSize));
					}
					else {
						Logger::Get()->Error(std::format("VERIFICATION FAILED | Size mismatch for {}: expected {}, got {}", vpath, fetch->numBytes, savedSize));
					}
				}
				else {
					Logger::Get()->Error(std::format("VERIFICATION FAILED | {} not found in FS after save", vpath));
				}
			}
			else {
				Logger::Get()->Error(std::format(" EM_Fetch::Fetch_Succeeded() FAILED | url {} | virtual path {}", context->url, context->virtualpath));
			}
			
		}

		//FILE* fp = fopen(vpath.c_str(), "wb"); // Or a cleaned filename
		//if (fp) {
		//	fwrite(fetch->data, 1, fetch->numBytes, fp);
		//	fclose(fp);
		//	
		//	EM_ASM({
		//		FS.syncfs(false, function(err) {
		//			if (err) 
		//			{
		//				console.error("EM_Fetch::Fetch_Succeeded() | Failed to sync to IndexedDB", err);
		//				//Logger::Get()->Trace(std::format("EM_Fetch::Fetch_Succeeded() Failed to sync to IndexedDB url {} | vpath {}", url, vpath));
		//			}
		//		});
		//	});
		//	Logger::Get()->Trace(std::format("EM_Fetch::Fetch_Succeeded() url {} | vpath {}", url, vpath));
		//}
		//else {
		//	Logger::Get()->Trace(std::format("EM_Fetch::Fetch_Succeeded() FAILED to fwrite url {} | vpath {}", url, vpath));
		//} 
		if (EM_Fetch::mCallbacks.contains(context->url)) {
			EM_Fetch::mCallbacks[context->url](context->uid, context->url, context->virtualpath, OK,  100);
			EM_Fetch::mCallbacks.erase(context->url);
		}
		delete context; // Clean up the heap-allocated string
		emscripten_fetch_close(fetch);
	
	}

	void EM_Fetch::Fetch_Failed(struct emscripten_fetch_t* fetch)
	{
		FetchContext* context = static_cast<FetchContext*>(fetch->userData);
		ASYNC_CALLBACK_STATUS status = FAILED;
		if (EM_Fetch::mCallbacks.contains(context->url)) {
			EM_Fetch::mCallbacks[context->url](context->uid, context->url, context->virtualpath, FAILED, 0);
			EM_Fetch::mCallbacks.erase(context->url);
		}
		emscripten_fetch_close(fetch); // Also free data on failure.
	}
	void EM_Fetch::Fetch_Progress(emscripten_fetch_t* fetch)
	{
		FetchContext* context = static_cast<FetchContext*>(fetch->userData);
		if (!context) return;

		// Standard safety check: status 0 often means "in progress" before header arrival
		if (fetch->status >= 400) return;

		if (fetch->totalBytes > 0) {
			// Use double for precision, then cast to int
			int per = static_cast<int>((static_cast<double>(fetch->numBytes) / fetch->totalBytes) * 100.0);

			std::string url(fetch->url);
			ASYNC_CALLBACK_STATUS status = PROGRESS;
			Logger::Get()->Trace(std::format("EM_Fetch::Fetch_Progress() PROGRESS url {} | per {}", url, per));
			// Note: fetch->data contains the partial data downloaded so far.
			// Usually, we don't pass partial data to the object until it's 100% complete.			
		}
		else {
			// Fallback for when totalBytes is unknown (Streamed data)
			std::string url(fetch->url);			
			Logger::Get()->Trace(std::format("EM_Fetch::Fetch_Progress() PROGRESS UNKNOWN url {}", url));
		}
		
	}
#endif // __EMSCRIPTEN__
}
