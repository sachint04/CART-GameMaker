#pragma once
#include <functional>
#include "Object.h"
#include "Core.h"
#include "Logger.h"
#include "Types.h"
namespace cart
{

	class Object;

	template<typename... Args>
	class Delegate
	{
	public:
		
		std::vector<std::pair<weak<Object>, std::function<bool(Args...)>>> mCallbacks;
		//List<std::function<bool(Args...)>> mCallbacks;

		template<typename ClassName>
		void BindAction(weak<Object> obj, void(ClassName::* callback)(Args...))
		{
			//auto& snapshot = mCallbacks;
			std::function<bool(Args...)> callbackFunc = [obj, callback](Args... args)->bool
			{
				auto sharedObj = obj.lock();
				if (sharedObj)
				{
					try
					{
						ClassName* target = dynamic_cast<ClassName*>(sharedObj.get());
						if (target) {
							(target->*callback)(args...);
							return true;
						}
						//(static_cast<ClassName*>(sharedObj.get())->*callback)(args...);
						//return true;
					}
					catch (const std::exception& e)
					{
						// Log the failure via your preferred logging system
						// Returning false ensures the crashed listener is pruned
						return false;
					}
					catch (...)
					{
						return false;
					}
				}

				return false;
			};
			
			mCallbacks.push_back({ obj, callbackFunc });
			int size = mCallbacks.size();
		//	if (size > 100) {
			//	Logger::Get()->Trace(std::format("Delegate::BindAction() mCallbacks size {} | callee {} ", size, obj.lock()->GetId()));
			//}
		}
		
		void Broadcast(Args... args)
		{
			std::erase_if(mCallbacks, [](const auto& pair) {
				return pair.first.expired();
			});

			for (std::pair<weak<Object>, std::function<bool(Args...)>> & iter  : mCallbacks)
			{
				if (auto lock = iter.first.lock()) {
					if (!iter.second(args...))
					{
						iter.first.reset();
					}
				}
			}

			

			/*try {
				if (!mCallbacks.empty()) {
					for (auto iter = mCallbacks.begin(); iter != mCallbacks.end(); )
					{
						if(!iter->first.lock())
						{
							iter = mCallbacks.erase(iter);
						}
						else
						{
							iter++;
						}
					}
				}
			}
			catch (const std::exception& e) {
				Logger::Get()->Trace("Delegate Callback Error!");
			}*/
			
		}

		void RemoveActionsByObjectId(std::string Id)
		{
			
			for (auto& iter : mCallbacks)
			{
				if (auto lock = iter.first.lock())
				{
					if (lock->GetId() == Id)
					{
						iter.first.reset();
					}
				
				}
			}

			for (auto iter = mCallbacks.begin(); iter != mCallbacks.end(); )
			{
				if (!iter->first.lock())
				{
					iter = mCallbacks.erase(iter);
				}
				else
				{
					iter++;
				}
			}
		}

		inline void Destroy()
		{
			mCallbacks.clear();
		}
		
		
	private:
	};
}