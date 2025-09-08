#include "network/network.h"
#include "Logger.h"
namespace cart {
	network::network()
        :requestCount{0}
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

  
}