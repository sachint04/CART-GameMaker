#include "network/network.h"
#include "Logger.h"
#include "Object.h"
namespace cart {
	network::network()
        :Object{},
        requestCount {0}, 
        mCallbacks{}, 
        m_LoadAssetCallbacks{}
	{
	}


	network::~network()
	{
	}

    void network::HTTPCallback(std::string& uid, std::string& response, std::string& data) {        
       Logger::Get()->Trace(std::format("network::HTTPCallback id {} \n", uid));
#ifdef  __EMSCRIPTEN__
       HideSpinnerView();
#endif //  __EMSCRIPTEN__

        for (auto iter = mCallbacks.begin(); iter != mCallbacks.end();)
        {
            if (iter->first.compare(uid) == 0)
            {
                if ((iter->second)(response, data))
                {
                    mCallbacks.erase(iter);                
                    break;
                }
                
            }
        }
    }

    void network::LoadAssetHTTPCallback(std::string& uid,  std::string url, unsigned char* data, int size) {
        // Prepare correct snap location list

#ifdef  __EMSCRIPTEN__
        HideSpinnerView();
#endif //  __EMSCRIPTEN__

        auto trim = [](std::string& s)
        {
            s.erase(0, s.find_first_not_of(" \t\n\r\f\v"));
            s.erase(s.find_last_not_of(" \t\n\r\f\v") + 1);
        };
        std::string delimiter = ",";
        std::string callerID = uid;
        std::string networkID = uid;
        Logger::Get()->Trace(std::format("network::LoadAssetHTTPCallback() -> caller Id {} | network Id {} ",callerID, networkID));
        int found = uid.find(delimiter);// find Ids        
        if (found != std::string::npos) {
            try {
                networkID = uid.substr(found + 1); // Network (this) generated Id            
                callerID  = uid.substr(0, found);// Load caller provided id
            }
            catch (const std::out_of_range& e) {
                Logger::Get()->Error(std::format("network::LoadAssetHTTPCallback  Error: {}", e.what()));
            }
              trim(callerID);
              trim(networkID);
            //Logger::Get()->Trace(std::format("network::LoadAssetHTTPCallback caller callerId {} networkID {} \n", callerID, networkID));
        }
        Logger::Get()->Trace(std::format("network::LoadAssetHTTPCallback() -> caller Id {} | network Id {} ", callerID, networkID));

        found = false;
        for (auto iter = m_LoadAssetCallbacks.begin(); iter != m_LoadAssetCallbacks.end();)
        {
            Logger::Get()->Trace(std::format("network::LoadAssetHTTPCallback() -> callback func Id {} | network Id {} ", iter->first, networkID));
            if (iter->first.compare(networkID) == 0)
            {
                if ((iter->second)(callerID, url, data, size))
                {
                    Logger::Get()->Trace(" network::LoadAssetHTTPCallback Callback removed \n");
                    m_LoadAssetCallbacks.erase(iter);
                    found = true;
                    break;
                }

            }
        }

        if(!found)
            Logger::Get()->Trace(std::format("network::LoadAssetHTTPCallback() -> callback func  nor found for  caller Id {} | network Id {} ", callerID, networkID));
    }

    std::string network::GetHost() {        
        std::string result = EM_Fetch::GetHost();
        return result;
    }

    std::string network::GetURL() {
        std::string result = EM_Fetch::GetURL();
        return result;
    }

    std::string network::GetPort() {
        std::string result = EM_Fetch::GetPort();
        return result;
    }

    std::string network::GetPath() {
        std::string result = EM_Fetch::GetPath();
        return result;
    }
}