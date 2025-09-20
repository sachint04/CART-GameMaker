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
       Logger::Get()->Push(std::format("HTTPCallback with id {} executed ", uid));
        for (auto iter = mCallbacks.begin(); iter != mCallbacks.end();)
        {
            if (iter->first.compare(uid) == 0)
            {
                if ((iter->second)(response, data))
                {
                    mCallbacks.erase(iter);
                //    Logger::Get()->Push(std::format("HTTPCallback Callback function fould for Id {}", uid));
                    break;
                }
                
            }
        }
    }

    void network::LoadAssetHTTPCallback(std::string& uid,  std::string filename, unsigned char* data, int size) {
        
        if (size == 0) {
            Logger::Get()->Push(std::format("ERROR!! Network | LoadAssetHTTPCallback() data size {} ", size));
            return;
        }
        for (auto iter = m_LoadAssetCallbacks.begin(); iter != m_LoadAssetCallbacks.end();)
        {
            if (iter->first.compare(uid) == 0)
            {
                if ((iter->second)(filename, data, size))
                {
                    m_LoadAssetCallbacks.erase(iter);
                    //    Logger::Get()->Push(std::format("HTTPCallback Callback function fould for Id {}", uid));
                    break;
                }

            }
        }
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