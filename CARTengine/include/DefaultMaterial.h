#pragma once
#include "Material.h"
namespace cart
{
	class DefaultMaterial :public Material
	{
	public:
		void Apply() override;  // Does nothing, uses default Raylib renderer
		void Detach() override;
		bool IsReady() override;
		bool IsMaterialActive()override;
		void SetMaterialActive(bool flag)override;
		void Destroy() override;
		bool IsPendingDestroy() override;

	private:
		bool m_bPendingDestroy;
		bool m_active;
	};

}