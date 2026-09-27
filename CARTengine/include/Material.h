#pragma once
#include <raylib.h>

namespace cart {
    class Material {
    public:
        virtual ~Material() = default;

        // Pure virtual methods to be implemented by specific materials
        virtual void Apply() = 0;
        virtual void Detach() = 0;
        virtual bool IsReady() = 0;
        virtual bool IsMaterialActive() = 0;
        virtual void SetMaterialActive(bool flag) = 0;
        virtual void Destroy() = 0;
        virtual bool IsPendingDestroy() = 0;
    };
}