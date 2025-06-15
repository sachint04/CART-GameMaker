#pragma once
#include <memory>
#include <iostream>
#include <functional>
#include <map>
#include "Core.h"
#include "tweenBuilder.h"
namespace cart {
    class TweenBuilder;
    class TweenBuilderBase;
    class Actor;
    
    class Tween {
    protected:
        Tween();// default constructor

    public:
        ~Tween() {

        };

        static TweenBuilder* create(weak<Actor> actor);
        static void Update(double deltaTime);
        static int GetTweenCount();
        static void AddTween(int id, TweenBuilderBase* tween);
        static Dictionary <int, TweenBuilderBase*> tweenlist;
        static List<TweenBuilderBase*> pendingDeleteTween;
        static void CleanCycle();

        static float m_cleanCycleIter;
        static  double m_cleanCycleStartTime;

        static long GetSizeOfPendingTweens();

    };

}