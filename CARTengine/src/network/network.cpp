#include "network/network.h"

namespace cart {
	network::network()
        :requestCount{0}
	{
	}


	network::~network()
	{
	}

    void network::HTTPCallback(std::string& uid, std::string& response, std::string& data) {
        for (auto iter = mCallbacks.begin(); iter != mCallbacks.end();)
        {
            if (iter->first.compare(uid) == 0)
            {
                if ((iter->second)(response, data))
                {
                    mCallbacks.erase(iter);
                    LOG("HTTPCallback with id %s executed ", uid.c_str());
                    break;
                }
            }
        }
    }

  
}