#include "component/controls/transformCntrl.h"
#include <memory>
#include "Types.h"
#include "UIButton.h"
#include "Shape.h"
#include "MathUtility.h"
namespace cart {
	TransformCntrl::TransformCntrl(World* _owningworld, const std::string& id, Vector2 minsize, Vector2 maxsize , Rectangle targetInitState)
		: UIElement{ _owningworld, id },
		 m_topleftCntrl{},
		 m_toprightCntrl{},
		  m_bottomleftCntrl{},
		m_bottomrightCntrl{},
		m_outline{},
		m_translateCntrl{},
		cntrlsize{ 8.f },
		cntrlhalf{ cntrlsize /2},
		curDragCntrl{""},
		m_isScaling{false},
		m_tempTargetLoc{},
		m_minSize{ minsize },
		m_maxSize{maxsize},
		m_targetInitState{ targetInitState }
	{
		
		m_center = { targetInitState.x + targetInitState.width/2.f,
					targetInitState.y + targetInitState.height/2.f};
	}

	void TransformCntrl::Init()
	{
		
		Btn_Text_Properties cntrlui = {};		
		cntrlui.color = GRAY;
		cntrlui.size = { cntrlsize, cntrlsize };
		cntrlui.btncol = GRAY;
		cntrlui.overcol = GRAY;
		cntrlui.downcol = ORANGE;

		
		
		m_location = { m_targetInitState.x, m_targetInitState.y };

		std::string btnid = "translate-btn";
		Btn_Text_Properties btnprop = {};
		btnprop.location = { m_location.x + cntrlsize, m_location.y + cntrlsize };
		btnprop.size = { m_targetInitState.width, m_targetInitState.height };
		btnprop.color = {0};
		btnprop.btncol = {0};
		btnprop.overcol = {0};
		btnprop.downcol = {0};
		btnprop.size = { m_targetInitState.width - cntrlsize *2 , m_targetInitState.height - cntrlsize * 2 };

		m_translateCntrl = AddButton(btnid, btnprop);
		m_translateCntrl.lock()->onButtonDown.BindAction(GetWeakRef(), &TransformCntrl::onTranslateStart);
		m_translateCntrl.lock()->onButtonDrag.BindAction(GetWeakRef(), &TransformCntrl::onTranslateContinue);
		m_translateCntrl.lock()->onButtonOut.BindAction(GetWeakRef(), &TransformCntrl::onTranslateEnd);
		m_translateCntrl.lock()->onButtonUp.BindAction(GetWeakRef(), &TransformCntrl::onButtonClick);



		std::string id = "outline-cntrl";

		m_outline = m_owningworld->SpawnActor<Shape>(id, m_location, m_targetInitState.width, m_targetInitState.height, BLUE, SHAPE_TYPE::LINE, 2);
		m_outline.lock()->SetVisible(true);
		m_outline.lock()->Init();
		AddChild(m_outline);


		cntrlui.location = { m_targetInitState.x, m_targetInitState.y };
		cntrlui.color = SKYBLUE;
		cntrlui.btncol = SKYBLUE;
		cntrlui.overcol = ORANGE;
		cntrlui.downcol = ORANGE;
		cntrlui.pivot = { cntrlhalf, cntrlhalf };
		id = "topleft-cntrl";
		m_topleftCntrl = AddButton(id, cntrlui,SHAPE_TYPE::CIRCLE);
		m_topleftCntrl.lock()->onButtonDrag.BindAction(GetWeakRef(), &TransformCntrl::onScaleHandler);
		m_topleftCntrl.lock()->onButtonDown.BindAction(GetWeakRef(), &TransformCntrl::onDragStart);
		m_topleftCntrl.lock()->onButtonUp.BindAction(GetWeakRef(), &TransformCntrl::onDragEnd);
		m_topleftCntrl.lock()->onButtonOut.BindAction(GetWeakRef(), &TransformCntrl::onDragOut);
	


		

		id = "topright-cntrl";
		cntrlui.location = { m_targetInitState.x + m_targetInitState.width , m_targetInitState.y  };
		m_toprightCntrl = AddButton(id, cntrlui,SHAPE_TYPE::CIRCLE);
		m_toprightCntrl.lock()->onButtonDrag.BindAction(GetWeakRef(), &TransformCntrl::onScaleHandler);
		m_toprightCntrl.lock()->onButtonDown.BindAction(GetWeakRef(), &TransformCntrl::onDragStart);
		m_toprightCntrl.lock()->onButtonUp.BindAction(GetWeakRef(), &TransformCntrl::onDragEnd);
		m_toprightCntrl.lock()->onButtonOut.BindAction(GetWeakRef(), &TransformCntrl::onDragOut);

	
		id = "bottomleft-cntrl";
		cntrlui.location = { m_targetInitState.x, m_targetInitState.y + m_targetInitState.height  };
		m_bottomleftCntrl = AddButton(id, cntrlui,SHAPE_TYPE::CIRCLE);
		m_bottomleftCntrl.lock()->onButtonDrag.BindAction(GetWeakRef(), &TransformCntrl::onScaleHandler);
		m_bottomleftCntrl.lock()->onButtonDown.BindAction(GetWeakRef(), &TransformCntrl::onDragStart);
		m_bottomleftCntrl.lock()->onButtonUp.BindAction(GetWeakRef(), &TransformCntrl::onDragEnd);
		m_bottomleftCntrl.lock()->onButtonOut.BindAction(GetWeakRef(), &TransformCntrl::onDragOut);

		
		id = "bottomright-cntrl";
		cntrlui.location = { m_targetInitState.x + m_targetInitState.width , m_targetInitState.y + m_targetInitState.height  };
		m_bottomrightCntrl = AddButton(id, cntrlui,SHAPE_TYPE::CIRCLE);
		m_bottomrightCntrl.lock()->onButtonDrag.BindAction(GetWeakRef(), &TransformCntrl::onScaleHandler);
		m_bottomrightCntrl.lock()->onButtonDown.BindAction(GetWeakRef(), &TransformCntrl::onDragStart);
		m_bottomrightCntrl.lock()->onButtonUp.BindAction(GetWeakRef(), &TransformCntrl::onDragEnd);
		m_bottomrightCntrl.lock()->onButtonOut.BindAction(GetWeakRef(), &TransformCntrl::onDragOut);
	
		id = "";
		btnprop = {};
		cntrlui = {};

	}
	void TransformCntrl::onScaleHandler(weak<Object> btn, Vector2 pos)
	{	
		Vector2 p = m_center;
		Vector2 dir  = Direction(m_center, pos );
		double len =  GetRawVectorLength(dir);
		
	
		


		if (len < 0)return;
		float tlxpos = p.x + cos(DegreesToRadians(135.f)) * len;
		float tlypos = p.y - sin(DegreesToRadians(135.f)) * len;
		
		float trxpos = p.x + cos(DegreesToRadians(45.f)) * len;
		float trypos = p.y - sin(DegreesToRadians(45.f)) * len;	


		float blxpos = p.x + cos(DegreesToRadians(225.f)) * len;
		float blypos = p.y - sin(DegreesToRadians(225.f)) * len;


		float brxpos = p.x + cos(DegreesToRadians(315.f)) * len;
		float brypos = p.y - sin(DegreesToRadians(315.f)) * len;
		
		float width = GetVectorLength(Direction({ tlxpos, tlypos }, { trxpos, trypos }));
		float height = GetVectorLength(Direction({ blxpos, blypos }, { tlxpos, tlypos }));

		if (width < m_minSize.x || height < m_minSize.y)return;

		m_topleftCntrl.lock()->SetLocation({ tlxpos, tlypos });
		m_toprightCntrl.lock()->SetLocation({ trxpos, trypos});
		m_bottomleftCntrl.lock()->SetLocation({ blxpos, blypos });
		m_bottomrightCntrl.lock()->SetLocation({ brxpos, brypos });
		

	//	LOG("OnDragTopLeft() width %.2f  height %.2f m_center x %.2f  y %.2f", width, height, m_center.x , m_center.y);
		//m_target.lock()->SetSize({width,height});
		//m_target.lock()->SetPivot({width/2.f,height/2.f});

		m_center = { m_topleftCntrl.lock()->GetLocation().x + width / 2.f,  m_topleftCntrl.lock()->GetLocation().y + height / 2.f };
		//m_target.lock()->SetLocation(m_center);
		onScaled.Broadcast(m_center, { width, height }, { width / 2.f, height / 2.f });

		m_outline.lock()->SetLocation({ m_topleftCntrl.lock()->GetLocation().x ,  m_topleftCntrl.lock()->GetLocation().y });
		m_outline.lock()->SetSize({ width,  height});

		m_translateCntrl.lock()->SetLocation({ m_topleftCntrl.lock()->GetLocation().x ,  m_topleftCntrl.lock()->GetLocation().y });
		m_translateCntrl.lock()->SetSize({ width,  height });	
	
		LOG("onScaleHandler() |width %.2f", width);
	}
	

	void TransformCntrl::onDragStart(weak<Object> btn, Vector2 pos)
	{
		if (m_isScaling)return;
		//LOG("Active Control %s ", btn.lock()->GetID().c_str());
		m_isScaling = true;	
		curDragCntrl = btn.lock()->GetID();
	}
	void TransformCntrl::onDragEnd(weak<Object> btn, Vector2 pos)
	{
		//LOG("Drag ended %s ", btn.lock()->GetID().c_str());
		m_isScaling = false;
		curDragCntrl = "";
		onButtonUp.Broadcast(pos);
		
	}
	void TransformCntrl::onDragOut(weak<Object> btn)
	{
		//LOG("Drag out end %s ", btn.lock()->GetID().c_str());
		m_isScaling = false;
		curDragCntrl = "";
		onStop.Broadcast();
	}
	void TransformCntrl::onTranslateStart(weak<Object>, Vector2 pos) {
		if (m_isScaling)return;
		m_tempTargetLoc = pos;
		//LOG("Translate Control Start");
	}
	void TransformCntrl::onTranslateContinue(weak<Object>, Vector2 pos) {

		if (m_isScaling)return;
		Vector2 offset = { pos.x - m_tempTargetLoc.x, pos.y - m_tempTargetLoc.y };
		SetLocation({m_location.x + offset.x, m_location.y + offset.y});

		float width = GetVectorLength(Direction(m_toprightCntrl.lock()->GetLocation(), m_topleftCntrl.lock()->GetLocation()));
		float height = GetVectorLength(Direction(m_bottomleftCntrl.lock()->GetLocation(), m_topleftCntrl.lock()->GetLocation()));
		m_center = { m_topleftCntrl.lock()->GetLocation().x + width / 2.f,  m_topleftCntrl.lock()->GetLocation().y + height / 2.f };
	//	LOG("onTranslateContinue() | offset x %.2f y %.2f  | width %.2f  height %.2f | center x %.2f  y %.2f", offset.x, offset.y, width, height, m_center.x, m_center.y);
		
		//m_target.lock()->SetLocation(m_center);
		m_outline.lock()->SetLocation({ m_topleftCntrl.lock()->GetLocation().x ,  m_topleftCntrl.lock()->GetLocation().y });
		m_translateCntrl.lock()->SetLocation({ m_topleftCntrl.lock()->GetLocation().x ,  m_topleftCntrl.lock()->GetLocation().y });
		onMoved.Broadcast(m_center);
		m_tempTargetLoc = pos;
	}
	void TransformCntrl::onTranslateEnd(weak<Object>) {
	//	LOG("Translate Control End");
		onStop.Broadcast();
	}
	void TransformCntrl::onButtonClick(weak<Object> btn, Vector2 pos)
	{
		onButtonUp.Broadcast(pos);
	}

	bool TransformCntrl::IsActiveCtrl(std::string _cntrl) {

		return curDragCntrl  == _cntrl;
	}
	void TransformCntrl::Draw(float _deltaTime)
	{
	
	}
	void TransformCntrl::Reset(){
		m_center = { m_targetInitState.x + m_targetInitState.width / 2.f, m_targetInitState.y + m_targetInitState.height / 2.f };
		onScaleHandler(m_topleftCntrl, { m_targetInitState.x, m_targetInitState.y });
		m_tempTargetLoc = { m_targetInitState.x , m_targetInitState.y };
	
	}

	void TransformCntrl::Close()
	{

		m_outline.lock()->SetVisible(false);

		m_topleftCntrl.lock()->Destroy();
		m_toprightCntrl.lock()->Destroy();
		m_bottomleftCntrl.lock()->Destroy();
		m_bottomrightCntrl.lock()->Destroy();
		m_translateCntrl.lock()->Destroy();
		m_outline.lock()->Destroy();
		SetVisible(false);
	}
	TransformCntrl::~TransformCntrl()
	{
		LOG("Transfrom Control Destroyed");
	}
}