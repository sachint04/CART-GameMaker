#pragma once
#include "tweenBuilderBase.h"
#include "Core.h"
#include "tweenMove.h"
#include "tweenRotate.h"
#include "tweenColor.h"
#include "tweenScale.h"

namespace cart {
	
	class TweenBuilder : public TweenBuilderBase {
	
	public:
		
		TweenBuilder(const std::string& id, weak<Actor> actor) ;
		~TweenBuilder() override;
	};


}
