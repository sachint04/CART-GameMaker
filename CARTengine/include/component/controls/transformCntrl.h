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
		TransformCntrl(World* _owningworld, const std::string& id, weak<UIElement> target, Vector2 minsize, Vector2 maxsize);
		~TransformCntrl();

		void Init() override;
		void Reset();
		void Close();
		Delegate<Rectangle>onScaled;
		Delegate<float>onRotated;
		Delegate<Vector2> onMoved;


	private:
		weak<UIElement> m_target;
		weak<UIButton> m_topleftCntrl;
		weak<UIButton>  m_toprightCntrl;
		weak<UIButton>  m_bottomleftCntrl;
		weak<UIButton>  m_bottomrightCntrl;
		weak<UIButton> m_translateCntrl;
		weak<Shape> m_outline;

		void onScaleHandler(weak<Object> btn, Vector2 pos);


		void TransformTarget(Rectangle rect);
		void UpdateCntrls();

		void onDragStart(weak<Object>, Vector2 pos);
		void onDragEnd(weak<Object>, Vector2 pos);
		void onDragOut(weak<Object>);
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