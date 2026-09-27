#include "DefaultMaterial.h"

namespace cart {


    void DefaultMaterial::Apply()
    {
    }
    void DefaultMaterial::Detach()
    {
    }
    bool DefaultMaterial::IsReady()
    {
        return true;
    }
    void DefaultMaterial::SetMaterialActive(bool flag)
    {
        m_active = flag;
    }

    bool DefaultMaterial::IsMaterialActive()
    {
        return true;
    }
    void DefaultMaterial::Destroy()
    {
        m_bPendingDestroy = true;
    }
    bool DefaultMaterial::IsPendingDestroy()
    {
        return  m_bPendingDestroy;
    }
}