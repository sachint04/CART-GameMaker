/**********************************************************************************************
* CART Engine UI Element Transform control
* Input 1) Minimum Scaled  size 2) Max scaled size 3) Current Rectangle information of target UI
* 
* Important: This control doesnot directly communicates with target UI, Listent to "onScaled" & "onMoved" Events 
* and  update target from outside
**********************************************************************************************/

#pragma once
#include <raylib.h>
#include "UIElement.h"
namespace  cart {
	class Actor;
	class UIButton;
	class World;
	class Shape;
	class TransformCntrl : public UIElement {

	public:
		TransformCntrl(World* _owningworld, const std::string& id, Vector2 minsize, Vector2 maxsize, Rectangle targetInitState);
		~TransformCntrl();

		void Init() override;
		void Reset();
		void Close();
		Delegate<Vector2, Vector2, Vector2>onScaled;
		Delegate<float>onRotated;
		Delegate<Vector2> onMoved;
		Delegate<Vector2> onButtonUp;
		Delegate<>onStop;
	private:
		weak<UIButton> m_topleftCntrl;
		weak<UIButton>  m_toprightCntrl;
		weak<UIButton>  m_bottomleftCntrl;
		weak<UIButton>  m_bottomrightCntrl;
		weak<UIButton> m_translateCntrl;
		weak<Shape> m_outline;

		void onScaleHandler(weak<Object> btn, Vector2 pos);
		
		void onDragStart(weak<Object>, Vector2 pos);
		void onDragEnd(weak<Object>, Vector2 pos);
		void onDragOut(weak<Object>);
		void onButtonClick(weak<Object>, Vector2 pos);
		void onTranslateStart(weak<Object>, Vector2 pos);
		void onTranslateContinue(weak<Object>, Vector2 pos);
		void onTranslateEnd(weak<Object>);
		bool IsActiveCtrl(std::string _cntrl);

		void Draw(float _deltaTime)override;

		float cntrlsize;
		float cntrlhalf;
		Vector2 m_minSize;
		Vector2 m_maxSize;
		float m_aspectRatio;
		std::string curDragCntrl;
		bool m_isScaling;
		bool m_isfixedAspectRatio;
		Vector2 m_tmpPivot;
		Vector2 m_tempTargetLoc;
		Rectangle m_targetInitState;
		Vector2 m_center;
		
	};
}