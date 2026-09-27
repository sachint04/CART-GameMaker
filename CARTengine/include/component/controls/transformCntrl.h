/**********************************************************************************************
* CART Engine UI Element Transform control
* Input 1) Minimum Scaled  size 2) Max scaled size 3) Current Rectangle information of target UI
* 
* Important: This control doesnot directly communicates with target UI, Listent to "onScaled" & "onMoved" Events 
* and  update target from outside
**********************************************************************************************/

#pragma once
#include "UIElement.h"
namespace  cart {

	typedef struct{
		std::string name;
		Rectangle rect;

	}Transform_Controls;

	class Actor;
	class UIButton;
	class ImageButton;
	class World;
	class Shape;
	class World;
	class TransformCntrl : public UIElement {


	public:
		TransformCntrl(World* _owningworld, const std::string& id);
		~TransformCntrl();

		void Init() override;
		void Start() override;
		//void Update(float _deltatime) override;
		void Draw(float _deltatime) override;
		void Destroy()override;
		void Reset();
		Rectangle GetBounds() override;

		void UpdateUIControls();
		
		//void SetDefaultRect(Rectangle rect);
		void SetMinMaxRect(Rectangle rect);
		Delegate<Rectangle>onScaled;
		Delegate<float>onRotated;
		Delegate<Vector2> onMoved;
		Delegate<Vector2> onButtonUp;
		Delegate<>onStop;
	private:
		weak<ImageButton> m_topleftCntrl;
		weak<ImageButton>  m_toprightCntrl;
		weak<ImageButton>  m_bottomleftCntrl;
		weak<ImageButton>  m_bottomrightCntrl;
		weak<ImageButton> m_translateCntrl;
		weak<Shape> m_outline;

		void onScaleHandler(weak<Object> btn, Vector2 pos);
		
		void OnDragStartHandler(weak<Object>, Vector2 pos);
		void OnDragEndHandler(weak<Object>, Vector2 pos);
		void OnDragOutHandler(weak<Object>);
		void OnButtonClickHandler(weak<Object>, Vector2 pos);
		void OnTranslateStartHandler(weak<Object>, Vector2 pos);
		void OnTranslateContinueHandler(weak<Object>, Vector2 pos);
		void OnTranslateEndHandler(weak<Object>);
		void SetScale(Vector2 pos);
		bool IsActiveCtrl(std::string _cntrl);

		float cntrlsize;
		float cntrlhalf;
		Rectangle m_MinMaxRect;
		float m_aspectRatio;
		std::string curDragCntrl;
		bool m_isScaling;
		bool m_isTranslating;
		bool m_isfixedAspectRatio;
		Vector2 m_tmpPivot;
		//Vector2 m_tempTargetLoc;

		
	};
}