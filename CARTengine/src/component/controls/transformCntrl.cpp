#include "component/controls/transformCntrl.h"
#include <memory>
#include "Types.h"
#include "UIButton.h"
#include "Shape.h"
namespace cart {
	TransformCntrl::TransformCntrl(World* _owningworld, const std::string& id, weak<UIElement> target, Vector2 minsize, Vector2 maxsize)
		: UIElement{ _owningworld, id },
		 m_topleftCntrl{},
		  m_topcenterCntrl{},
		  m_toprightCntrl{},
		  m_middleleftCntrl{},
		  m_middlerightCntrl{},
		  m_bottomleftCntrl{},
		  m_bottomcenterCntrl{},
		  m_bottomrightCntrl{},
		m_outline{},
		m_translateCntrl{},
		m_target{ target },
		cntrlsize{ 8.f },
		cntrlhalf{ cntrlsize /2},
		curDragCntrl{""},
		m_isScaling{false},
		m_isfixedAspectRatio{true},
		m_aspectRatio{1},
		m_tmpPivot{},
		m_tempTargetLoc{},
		m_minSize{ minsize },
		m_maxSize{maxsize}
	{
		m_aspectRatio = { m_target.lock()->GetBounds().width / m_target.lock()->GetBounds().height };
	}

	void TransformCntrl::Init()
	{
		
		Btn_Text_Properties cntrlui = {};		
		cntrlui.color = GRAY;
		cntrlui.size = { cntrlsize, cntrlsize };
		cntrlui.btncol = GRAY;
		cntrlui.overcol = GRAY;
		cntrlui.downcol = ORANGE;


		m_targetInitState = m_target.lock()->GetBounds();

		Rectangle rect = m_targetInitState;// m_target.lock()->GetBounds();
		
		m_location = { rect.x, rect.y };

		std::string btnid = "translate-btn";
		Btn_Text_Properties btnprop = {};
		btnprop.location = m_location;
		btnprop.size = { rect.width, rect.height };
		btnprop.btncol = {0};
		btnprop.overcol = {0};
		btnprop.downcol = { 255,255,255,50 };
		btnprop.size = { rect.width, rect.height };

		m_translateCntrl = AddButton(btnid, btnprop);
		m_translateCntrl.lock()->onButtonDown.BindAction(GetWeakRef(), &TransformCntrl::onTranslateStart);
		m_translateCntrl.lock()->onButtonDrag.BindAction(GetWeakRef(), &TransformCntrl::onTranslateContinue);
		m_translateCntrl.lock()->onButtonOut.BindAction(GetWeakRef(), &TransformCntrl::onTranslateEnd);

		std::string id = "outline-cntrl";
		m_outline = m_owningworld->SpawnActor<Shape>(id, m_location, rect.width, rect.height, BLUE, SHAPE_TYPE::LINE);	
		m_outline.lock()->SetVisible(true);
		m_outline.lock()->Init();
		AddChild(m_outline);

		id = "topleft-cntrl";
		cntrlui.location = { rect.x -cntrlhalf, rect.y - cntrlhalf };
		m_topleftCntrl = AddButton(id, cntrlui);
		m_topleftCntrl.lock()->onButtonDrag.BindAction(GetWeakRef(), &TransformCntrl::onDragTopLeft);
		m_topleftCntrl.lock()->onButtonDown.BindAction(GetWeakRef(), &TransformCntrl::onDragStart);
		m_topleftCntrl.lock()->onButtonUp.BindAction(GetWeakRef(), &TransformCntrl::onDragEnd);
		m_topleftCntrl.lock()->onButtonOut.BindAction(GetWeakRef(), &TransformCntrl::onDragOut);

		id = "topcenter-cntrl";
		cntrlui.location = { rect.x + rect.width / 2.f -cntrlhalf, rect.y - cntrlhalf };
		m_topcenterCntrl = AddButton(id, cntrlui);
		m_topcenterCntrl.lock()->onButtonDrag.BindAction(GetWeakRef(), &TransformCntrl::onDragTopCenter);
		m_topcenterCntrl.lock()->onButtonDown.BindAction(GetWeakRef(), &TransformCntrl::onDragStart);
		m_topcenterCntrl.lock()->onButtonUp.BindAction(GetWeakRef(), &TransformCntrl::onDragEnd);
		m_topcenterCntrl.lock()->onButtonOut.BindAction(GetWeakRef(), &TransformCntrl::onDragOut);

		id = "topright-cntrl";
		cntrlui.location = { rect.x + rect.width -cntrlhalf, rect.y - cntrlhalf };
		m_toprightCntrl = AddButton(id, cntrlui);
		m_toprightCntrl.lock()->onButtonDrag.BindAction(GetWeakRef(), &TransformCntrl::onDragTopRight);
		m_toprightCntrl.lock()->onButtonDown.BindAction(GetWeakRef(), &TransformCntrl::onDragStart);
		m_toprightCntrl.lock()->onButtonUp.BindAction(GetWeakRef(), &TransformCntrl::onDragEnd);
		m_toprightCntrl.lock()->onButtonOut.BindAction(GetWeakRef(), &TransformCntrl::onDragOut);

		id = "middleleft-cntrl";
		cntrlui.location = { rect.x -cntrlhalf, rect.y + (rect.height / 2.f) - cntrlhalf };
		m_middleleftCntrl = AddButton(id, cntrlui);
		m_middleleftCntrl.lock()->onButtonDrag.BindAction(GetWeakRef(), &TransformCntrl::onDragMiddleLeft);
		m_middleleftCntrl.lock()->onButtonDown.BindAction(GetWeakRef(), &TransformCntrl::onDragStart);
		m_middleleftCntrl.lock()->onButtonUp.BindAction(GetWeakRef(), &TransformCntrl::onDragEnd);
		m_middleleftCntrl.lock()->onButtonOut.BindAction(GetWeakRef(), &TransformCntrl::onDragOut);

		id = "middleRight-cntrl";
		cntrlui.location = { rect.x + rect.width -cntrlhalf, rect.y + (rect.height / 2.f ) - cntrlhalf };
		m_middlerightCntrl = AddButton(id, cntrlui);
		m_middlerightCntrl.lock()->onButtonDrag.BindAction(GetWeakRef(), &TransformCntrl::onDragMiddleRight);
		m_middlerightCntrl.lock()->onButtonDown.BindAction(GetWeakRef(), &TransformCntrl::onDragStart);
		m_middlerightCntrl.lock()->onButtonUp.BindAction(GetWeakRef(), &TransformCntrl::onDragEnd);
		m_middlerightCntrl.lock()->onButtonOut.BindAction(GetWeakRef(), &TransformCntrl::onDragOut);

		id = "bottomleft-cntrl";
		cntrlui.location = { rect.x -cntrlhalf, rect.y + rect.height - cntrlhalf };
		m_bottomleftCntrl = AddButton(id, cntrlui);
		m_bottomleftCntrl.lock()->onButtonDrag.BindAction(GetWeakRef(), &TransformCntrl::onDragBottomLeft);
		m_bottomleftCntrl.lock()->onButtonDown.BindAction(GetWeakRef(), &TransformCntrl::onDragStart);
		m_bottomleftCntrl.lock()->onButtonUp.BindAction(GetWeakRef(), &TransformCntrl::onDragEnd);
		m_bottomleftCntrl.lock()->onButtonOut.BindAction(GetWeakRef(), &TransformCntrl::onDragOut);

		id = "bottomcenter-cntrl";
		cntrlui.location = { rect.x + rect.width /2.f -cntrlhalf, rect.y + rect.height - cntrlhalf };
		m_bottomcenterCntrl = AddButton(id, cntrlui);
		m_bottomcenterCntrl.lock()->onButtonDrag.BindAction(GetWeakRef(), &TransformCntrl::onDragBottomCenter);
		m_bottomcenterCntrl.lock()->onButtonDown.BindAction(GetWeakRef(), &TransformCntrl::onDragStart);
		m_bottomcenterCntrl.lock()->onButtonUp.BindAction(GetWeakRef(), &TransformCntrl::onDragEnd);
		m_bottomcenterCntrl.lock()->onButtonOut.BindAction(GetWeakRef(), &TransformCntrl::onDragOut);

		id = "bottomright-cntrl";
		cntrlui.location = { rect.x + rect.width -cntrlhalf, rect.y + rect.height - cntrlhalf };
		m_bottomrightCntrl = AddButton(id, cntrlui);
		m_bottomrightCntrl.lock()->onButtonDrag.BindAction(GetWeakRef(), &TransformCntrl::onDragBottomRight);
		m_bottomrightCntrl.lock()->onButtonDown.BindAction(GetWeakRef(), &TransformCntrl::onDragStart);
		m_bottomrightCntrl.lock()->onButtonUp.BindAction(GetWeakRef(), &TransformCntrl::onDragEnd);
		m_bottomrightCntrl.lock()->onButtonOut.BindAction(GetWeakRef(), &TransformCntrl::onDragOut);


	
		id = "";
		btnprop = {};
		cntrlui = {};

	}
	void TransformCntrl::onDragTopLeft(weak<Object> btn, Vector2 pos)
	{	
		if (!IsActiveCtrl(m_topleftCntrl.lock()->GetID()))return;
		Rectangle rect = m_target.lock()->GetBounds();
		float rectmaxH = rect.x + rect.width;
		float rectmaxY = rect.y + rect.height;

		if (rectmaxH < pos.x || rectmaxY < pos.y)return;

		float width =  rectmaxH - pos.x;
		float height = rectmaxY - pos.y;

	
		if (width < m_minSize.x || height < m_minSize.y)return;
		//if (rectmaxY < pos.y)return;
		TransformTarget({ pos.x , pos.y, width , height });
		//UpdateCntrls({ pos.x , pos.y, width , height });		
		UpdateCntrls();		
	}
	void TransformCntrl::onDragTopCenter(weak<Object> btn, Vector2 pos)
	{
		if(!IsActiveCtrl(m_topcenterCntrl.lock()->GetID()))return;

		Rectangle rect = m_target.lock()->GetBounds();
		pos.x =  rect.x;
		float rectmaxH = rect.x + rect.width;
		float rectmaxY = rect.y + rect.height;

		if (rectmaxH < pos.x || rectmaxY < pos.y)return;

		float width = rectmaxH - pos.x;
		float height = rectmaxY - pos.y;
		if (width < m_minSize.x || height < m_minSize.y)return;
		//if (rectmaxY < pos.y)return;

		TransformTarget({ pos.x , pos.y, width , height });
		//UpdateCntrls({ pos.x , pos.y, width , height });
		UpdateCntrls();
	}
	void TransformCntrl::onDragTopRight(weak<Object> btn, Vector2 pos)
	{
		if (!IsActiveCtrl(m_toprightCntrl.lock()->GetID()))return;
		Rectangle rect = m_target.lock()->GetBounds();
		float rectminH = rect.x;
		float rectmaxY = rect.y + rect.height;

		if (rectminH > pos.x || rectmaxY < pos.y)return;

		float width = pos.x - rectminH;
		float height = rectmaxY - pos.y;

		if (width < m_minSize.x || height < m_minSize.y)return;
		//if (rectmaxY < pos.y)return;
		TransformTarget({ rectminH , pos.y, width , height });
	//	UpdateCntrls({ rectminH , pos.y, width , height });
		UpdateCntrls();
	}
	void TransformCntrl::onDragMiddleLeft(weak<Object> btn, Vector2 pos)
	{
		if (!IsActiveCtrl(m_middleleftCntrl.lock()->GetID()))return;
		Rectangle rect = m_target.lock()->GetBounds();
		pos.y = rect.y;		
		float rectmaxH = rect.x + rect.width;
		float rectmaxY = rect.y + rect.height;

		if (rectmaxH < pos.x || rectmaxY < pos.y)return;

		float width = rectmaxH - pos.x;
		float height = rectmaxY - pos.y;
		if (width < m_minSize.x || height < m_minSize.y)return;
		TransformTarget({ pos.x , pos.y, width , height });
		//UpdateCntrls({ pos.x , pos.y, width , height });
		UpdateCntrls();
	}
	void TransformCntrl::onDragMiddleRight(weak<Object> btn, Vector2 pos)
	{
		if (!IsActiveCtrl(m_middlerightCntrl.lock()->GetID()))return;
		Rectangle rect = m_target.lock()->GetBounds();
		pos.y = rect.y;
		float rectminH = rect.x;
		float rectmaxY = rect.y + rect.height;

		if (rectminH > pos.x || rectmaxY < pos.y)return;

		float width = pos.x - rectminH;
		float height = rectmaxY - pos.y;

		if (width < m_minSize.x || height < m_minSize.y)return;
		//if (rectmaxY < pos.y)return;
		TransformTarget({ rectminH , pos.y, width , height });
		//UpdateCntrls({ rectminH , pos.y, width , height });
		UpdateCntrls();
	}
	void TransformCntrl::onDragBottomLeft(weak<Object>btn, Vector2 pos)
	{
		if (!IsActiveCtrl(m_bottomleftCntrl.lock()->GetID()))return;
		Rectangle rect = m_target.lock()->GetBounds();
		float rectmaxH = rect.x + rect.width;
		float rectminY = rect.y;

		if (rectmaxH < pos.x || pos.y < rectminY)return;

		float width = rectmaxH - pos.x;
		float height =  pos.y - rectminY;

		if (width < m_minSize.x || height < m_minSize.y)return;
		TransformTarget({ pos.x , rectminY, width , height });
		//UpdateCntrls({ pos.x , rectminY, width , height });
		UpdateCntrls();
	}
	void TransformCntrl::onDragBottomCenter(weak<Object> btn, Vector2 pos)
	{
		if (!IsActiveCtrl(m_bottomcenterCntrl.lock()->GetID()))return;
		Rectangle rect = m_target.lock()->GetBounds();
		pos.x = rect.x;
		float rectmaxH = rect.x + rect.width;
		float rectminY = rect.y;

		if (rectmaxH < pos.x || pos.y < rectminY)return;

		float width = rectmaxH - pos.x;
		float height = pos.y - rectminY;

		if (width < m_minSize.x || height < m_minSize.y)return;
		TransformTarget({ pos.x , rectminY, width , height });
		//UpdateCntrls({ pos.x , rectminY, width , height });
		UpdateCntrls();
	}
	void TransformCntrl::onDragBottomRight(weak<Object> btn, Vector2 pos)
	{
		if (!IsActiveCtrl(m_bottomrightCntrl.lock()->GetID()))return;
		Rectangle rect = m_target.lock()->GetBounds();
		float rectminH = rect.x;
		float rectminY = rect.y;

		if (rectminH > pos.x || rectminY > pos.y)return;

		float width = pos.x - rectminH;
		float height = pos.y - rectminY;

		if (width < m_minSize.x || height < m_minSize.y)return;
		TransformTarget({ rectminH ,rectminY, width , height });
		//UpdateCntrls({ rectminH ,rectminY, width , height });
		UpdateCntrls();
	}	
	void TransformCntrl::onDragStart(weak<Object> btn, Vector2 pos)
	{
		if (m_isScaling)return;
		//LOG("Active Control %s ", btn.lock()->GetID().c_str());
		m_isScaling = true;
		Rectangle rect = m_target.lock()->GetBounds();
		m_tmpPivot = { rect.x + rect.width / 2.f , rect.y + rect.height / 2.f };
		curDragCntrl = btn.lock()->GetID();
	}
	void TransformCntrl::onDragEnd(weak<Object> btn, Vector2 pos)
	{
		//LOG("Drag ended %s ", btn.lock()->GetID().c_str());
		m_isScaling = false;
		curDragCntrl = "";
	}
	void TransformCntrl::onDragOut(weak<Object> btn)
	{
		//LOG("Drag out end %s ", btn.lock()->GetID().c_str());
		m_isScaling = false;
		curDragCntrl = "";
	}
	void TransformCntrl::onTranslateStart(weak<Object>, Vector2 pos) {
		if (m_isScaling)return;
		m_tempTargetLoc = pos;
		//LOG("Translate Control Start");
	}
	void TransformCntrl::onTranslateContinue(weak<Object>, Vector2 pos) {

		if (m_isScaling)return;
		Vector2 offset = { pos.x - m_tempTargetLoc.x, pos.y - m_tempTargetLoc.y };
		m_target.lock()->Offset(offset);			
		//UpdateCntrls(m_target.lock()->GetBounds());
		UpdateCntrls();
		m_tempTargetLoc = pos;
	}
	void TransformCntrl::onTranslateEnd(weak<Object>) {
	//	LOG("Translate Control End");
	}
	void TransformCntrl::TransformTarget(Rectangle rect) {

		if (m_isfixedAspectRatio) {
			if (rect.height < rect.width) {
				rect.height = rect.width / m_aspectRatio;
			}
			else {
				rect.width = rect.height * m_aspectRatio;
			}
			rect.x = m_tmpPivot.x - rect.width / 2.f;
			rect.y = m_tmpPivot.y - rect.height / 2.f;
		}
		
		m_target.lock()->SetLocation({ rect.x + m_target.lock()->GetPivot().x, rect.y+ m_target.lock()->GetPivot().y });
		m_target.lock()->SetSize({ rect.width, rect.height });
		m_outline.lock()->SetLocation({ rect.x, rect.y });
		m_outline.lock()->SetSize({ rect.width, rect.height });

	}

	void TransformCntrl::UpdateCntrls()
	{
		Rectangle rect = m_target.lock()->GetBounds();
		m_translateCntrl.lock()->SetLocation({ rect.x, rect.y });
		m_translateCntrl.lock()->SetSize({ rect.width, rect.height });

		m_outline.lock()->SetLocation({rect.x, rect.y});
		m_outline.lock()->SetSize({rect.width, rect.height});

		m_topleftCntrl.lock()->SetLocation({ rect.x - cntrlhalf, rect.y - cntrlhalf});
		m_topcenterCntrl.lock()->SetLocation({ rect.x + (rect.width/2) - cntrlhalf, rect.y - cntrlhalf });
		m_toprightCntrl.lock()->SetLocation({ rect.x + rect.width - cntrlhalf, rect.y - cntrlhalf });
		m_middleleftCntrl.lock()->SetLocation({ rect.x - cntrlhalf, rect.y + (rect.height /2) - cntrlhalf });
		m_middlerightCntrl.lock()->SetLocation({ rect.x + rect.width - cntrlhalf, rect.y + (rect.height /2) - cntrlhalf });
		m_bottomleftCntrl.lock()->SetLocation({ rect.x - cntrlhalf, rect.y + rect.height - cntrlhalf });
		m_bottomcenterCntrl.lock()->SetLocation({ rect.x + (rect.width /2 ) - cntrlhalf, rect.y + rect.height - cntrlhalf });
		m_bottomrightCntrl.lock()->SetLocation({ rect.x + rect.width - cntrlhalf, rect.y + rect.height - cntrlhalf });

	}

	bool TransformCntrl::IsActiveCtrl(std::string _cntrl) {

		return curDragCntrl  == _cntrl;
	}
	void TransformCntrl::Reset(){
		m_tmpPivot = { m_targetInitState.x + (m_targetInitState.width /2.f) , m_targetInitState.y + (m_targetInitState.height /2.f)};
		TransformTarget({ m_targetInitState.x , m_targetInitState.y, m_targetInitState.width , m_targetInitState.height });
		UpdateCntrls();
	}

	void TransformCntrl::Close()
	{
		m_target.lock()->SetVisible(false);
		m_outline.lock()->SetVisible(false);

		m_target.lock()->Destroy();
		m_topleftCntrl.lock()->Destroy();
		m_topcenterCntrl.lock()->Destroy();
		m_toprightCntrl.lock()->Destroy();
		m_middleleftCntrl.lock()->Destroy();
		m_middlerightCntrl.lock()->Destroy();
		m_bottomleftCntrl.lock()->Destroy();
		m_bottomcenterCntrl.lock()->Destroy();
		m_bottomrightCntrl.lock()->Destroy();
		m_translateCntrl.lock()->Destroy();
		m_outline.lock()->Destroy();
		SetVisible(false);
	}
	TransformCntrl::~TransformCntrl()
	{
		
	}
}