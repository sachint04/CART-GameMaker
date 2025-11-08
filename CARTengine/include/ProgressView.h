/*
* ProgressView class will managed by HUD and World 
*/

#pragma once
#include <string>
#include "UIElement.h"
#include "Types.h"
#include "Core.h"
namespace cart {

	class World;
	class Text;
	class ProgressView : public UIElement{
	public :
		ProgressView(World* _owningworld, const std::string& _id, bool isExcludedFromParentAutoControl = false, PROGRESS_VIEW_TYPE _type = P_BAR);
		~ProgressView();
		void Init()override;
		void Start()override;
		void Update(float deltaTime)override;
		void Draw(float deltaTime)override;
		void SetUIProperties(ProgressVeiw_Properties  _prop);

		void Destroy()override;

		void ShowProgress(float v, const std::string& m);


	private:
		float m_pValue;// 0.0 - 1.0 % value
		std::string m_pMessge;
		PROGRESS_VIEW_TYPE m_pType;
		Rectangle m_barRect;
		Color m_barCol;
		Color m_barFill;
		Color m_txtCol;
	
		void DrawBar();
		void DrawDisc();
		void DrawClock();
	};
}