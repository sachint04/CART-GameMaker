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
		TransformCntrl(World* _owningworld, const std::string& id, weak<UIElement> target);
		~TransformCntrl();

		void Init() override;
		Delegate<Rectangle>onScaled;
		Delegate<float>onRotated;
		Delegate<Vector2> onMoved;

	private:
		weak<UIElement> m_target;
		weak<UIButton> m_topleftCntrl;
		weak<UIButton>  m_topcenterCntrl;
		weak<UIButton>  m_toprightCntrl;
		weak<UIButton>  m_middleleftCntrl;
		weak<UIButton>  m_middlerightCntrl;
		weak<UIButton>  m_bottomleftCntrl;
		weak<UIButton>  m_bottomcenterCntrl;
		weak<UIButton>  m_bottomrightCntrl;
		weak<UIButton> m_translateCntrl;
		weak<Shape> m_outline;

		void onDragTopLeft(weak<Object> btn, Vector2 pos);
		void onDragTopCenter(weak<Object> btn, Vector2 pos);
		void onDragTopRight(weak<Object> btn, Vector2 pos);
		void onDragMiddleLeft(weak<Object> btn, Vector2 pos);
		void onDragMiddleRight(weak<Object> btn, Vector2 pos);
		void onDragBottomLeft(weak<Object> btn, Vector2 pos);
		void onDragBottomCenter(weak<Object> btn, Vector2 pos);
		void onDragBottomRight(weak<Object> btn, Vector2 pos);

		void TransformTarget(Rectangle rect);
		void UpdateCntrls(Rectangle rect);

		void onDragStart(weak<Object>, Vector2 pos);
		void onDragEnd(weak<Object>, Vector2 pos);
		void onDragOut(weak<Object>);
		void onTranslateStart(weak<Object>, Vector2 pos);
		void onTranslateContinue(weak<Object>, Vector2 pos);
		void onTranslateEnd(weak<Object>);
		bool IsActiveCtrl(std::string _cntrl);

		float cntrlsize;
		float cntrlhalf;
		std::string curDragCntrl;
		bool m_isScaling;
		Vector2 tempTargetLoc;
		
	};
}