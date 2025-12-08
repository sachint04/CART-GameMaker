#include "component/controls/transformCntrl.h"
#include <memory>
#include "Types.h"
#include "UIButton.h"
#include "Shape.h"
#include "MathUtility.h"
#include "World.h"
#include "Logger.h"

namespace cart {
#pragma region  Constructor & Init


	TransformCntrl::TransformCntrl(World* _owningworld, const std::string& id)
		: UIElement{ _owningworld, id },
		 m_topleftCntrl{},
		 m_toprightCntrl{},
		  m_bottomleftCntrl{},
		m_bottomrightCntrl{},
		m_outline{},
		m_translateCntrl{},
		cntrlsize{ 12.f },
		cntrlhalf{ cntrlsize /2},
		curDragCntrl{""},
		m_isScaling{false},
		m_isTranslating{false},
		m_tempTargetLoc{},
		m_targetInitState{},
		m_aspectRatio{},
		m_isfixedAspectRatio{},
		m_tmpPivot{},
		m_MinMaxRect{}
	{
		
	
	}

	void TransformCntrl::Init()
	{
		
		UIElement::Init();

	}

	void TransformCntrl::Start()
	{




		SetLocation({ m_targetInitState.x, m_targetInitState.y });


		std::string btnid = "translate-btn";
		Btn_Text_Properties trn_prop = {};
		trn_prop.location = { m_location.x + 10.f , m_location.y + 10.f };
		trn_prop.size = { m_targetInitState.width - 20.f, m_targetInitState.height - 20.f};
		trn_prop.color = { 0 };
		trn_prop.btncol = { 0 };
		trn_prop.overcol = { 0 };
		trn_prop.downcol = {0};
		m_translateCntrl = AddButton(btnid, trn_prop);// Translate Control

		m_translateCntrl.lock()->onButtonDown.BindAction(GetWeakRef(), &TransformCntrl::onTranslateStart);
		m_translateCntrl.lock()->onButtonDrag.BindAction(GetWeakRef(), &TransformCntrl::onTranslateContinue);
		m_translateCntrl.lock()->onButtonOut.BindAction(GetWeakRef(), &TransformCntrl::onTranslateEnd);
		m_translateCntrl.lock()->onButtonUp.BindAction(GetWeakRef(), &TransformCntrl::onButtonClick);
		
		//======================================

		std::string id = "outline-cntrl";
		UI_Properties lnui = {};
		lnui.location = m_location;
		lnui.size = { m_targetInitState.width, m_targetInitState.height };
		lnui.color = BLUE;
		lnui.shapetype = SHAPE_TYPE::LINE;
		lnui.linewidth = 1;
		m_outline = m_owningworld->SpawnActor<Shape>(id);
		m_outline.lock()->SetUIProperties(lnui);
		m_outline.lock()->SetVisible(true);
		m_outline.lock()->Init();
		AddChild(m_outline); // Outline
		//======================================

		Btn_Text_Properties cntrlui = {};
		cntrlui.size = { cntrlsize, cntrlsize };
		cntrlui.location = { m_targetInitState.x, m_targetInitState.y };
		cntrlui.color = SKYBLUE;
		cntrlui.btncol = SKYBLUE;
		cntrlui.overcol = ORANGE;
		cntrlui.downcol = ORANGE;
		cntrlui.pivot = { cntrlhalf, cntrlhalf };
		cntrlui.shapetype = SHAPE_TYPE::CIRCLE;
		id = "topleft-cntrl";
		m_topleftCntrl = AddButton(id, cntrlui);// TOP LEFT control
		m_topleftCntrl.lock()->onButtonDrag.BindAction(GetWeakRef(), &TransformCntrl::onScaleHandler);
		m_topleftCntrl.lock()->onButtonDown.BindAction(GetWeakRef(), &TransformCntrl::onDragStart);
		m_topleftCntrl.lock()->onButtonUp.BindAction(GetWeakRef(), &TransformCntrl::onDragEnd);
		m_topleftCntrl.lock()->onButtonOut.BindAction(GetWeakRef(), &TransformCntrl::onDragOut);

		//====================================== 

		id = "topright-cntrl";
		cntrlui.location = { m_targetInitState.x + m_targetInitState.width , m_targetInitState.y };
		m_toprightCntrl = AddButton(id, cntrlui);// TOP RIGHT control
		m_toprightCntrl.lock()->onButtonDrag.BindAction(GetWeakRef(), &TransformCntrl::onScaleHandler);
		m_toprightCntrl.lock()->onButtonDown.BindAction(GetWeakRef(), &TransformCntrl::onDragStart);
		m_toprightCntrl.lock()->onButtonUp.BindAction(GetWeakRef(), &TransformCntrl::onDragEnd);
		m_toprightCntrl.lock()->onButtonOut.BindAction(GetWeakRef(), &TransformCntrl::onDragOut);

		//==============================================

		id = "bottomleft-cntrl";
		cntrlui.location = { m_targetInitState.x, m_targetInitState.y + m_targetInitState.height };
		m_bottomleftCntrl = AddButton(id, cntrlui);// BOTTOM LEFT control
		m_bottomleftCntrl.lock()->onButtonDrag.BindAction(GetWeakRef(), &TransformCntrl::onScaleHandler);
		m_bottomleftCntrl.lock()->onButtonDown.BindAction(GetWeakRef(), &TransformCntrl::onDragStart);
		m_bottomleftCntrl.lock()->onButtonUp.BindAction(GetWeakRef(), &TransformCntrl::onDragEnd);
		m_bottomleftCntrl.lock()->onButtonOut.BindAction(GetWeakRef(), &TransformCntrl::onDragOut);

		//==============================================

		id = "bottomright-cntrl";
		cntrlui.location = { m_targetInitState.x + m_targetInitState.width , m_targetInitState.y + m_targetInitState.height };
		m_bottomrightCntrl = AddButton(id, cntrlui);// BOTTOM RIGHT control
		m_bottomrightCntrl.lock()->onButtonDrag.BindAction(GetWeakRef(), &TransformCntrl::onScaleHandler);
		m_bottomrightCntrl.lock()->onButtonDown.BindAction(GetWeakRef(), &TransformCntrl::onDragStart);
		m_bottomrightCntrl.lock()->onButtonUp.BindAction(GetWeakRef(), &TransformCntrl::onDragEnd);
		m_bottomrightCntrl.lock()->onButtonOut.BindAction(GetWeakRef(), &TransformCntrl::onDragOut);
		//=========================================================
		id = "";
		cntrlui = {};


		Actor::Start();

	}
	
#pragma endregion
#pragma region Event Handler
	void TransformCntrl::onScaleHandler(weak<Object> btn, Vector2 pos)
	{	
		
		Vector2 center = {m_topleftCntrl.lock()->GetLocation().x +  ((m_bottomrightCntrl.lock()->GetLocation().x - m_topleftCntrl.lock()->GetLocation().x)  / 2.f),						m_topleftCntrl.lock()->GetLocation().y + ((m_bottomrightCntrl.lock()->GetLocation().y - m_topleftCntrl.lock()->GetLocation().y) / 2.f) };
		Vector2 dir  = Direction(	{m_targetInitState.x + m_targetInitState.width / 2.f, m_targetInitState.y + m_targetInitState.height / 2.f }, pos);
		double len =  GetRawVectorLength(dir);

		if (len < 0)return;
		float tlxpos = center.x + cos(DegreesToRadians(135.f)) * len;
		float tlypos = center.y - sin(DegreesToRadians(135.f)) * len;
		
		float trxpos = center.x + cos(DegreesToRadians(45.f)) * len;
		float trypos = center.y - sin(DegreesToRadians(45.f)) * len;	


		float blxpos = center.x + cos(DegreesToRadians(225.f)) * len;
		float blypos = center.y - sin(DegreesToRadians(225.f)) * len;


		float brxpos = center.x + cos(DegreesToRadians(315.f)) * len;
		float brypos = center.y - sin(DegreesToRadians(315.f)) * len;
		
		double width = GetVectorLength(Direction({ trxpos, trypos }, { tlxpos, tlypos }));
		double height = GetVectorLength(Direction({ blxpos, blypos }, { tlxpos, tlypos }));

		if (width < m_MinMaxRect.x|| height < m_MinMaxRect.y || width > m_MinMaxRect.width || height > m_MinMaxRect.height)return;

		m_topleftCntrl.lock()->SetLocation({ tlxpos, tlypos });
		m_toprightCntrl.lock()->SetLocation({ trxpos, trypos});
		m_bottomleftCntrl.lock()->SetLocation({ blxpos, blypos });
		m_bottomrightCntrl.lock()->SetLocation({ brxpos, brypos });
		

		//m_center = { m_topleftCntrl.lock()->GetLocation().x + (float)width / 2.f,  m_topleftCntrl.lock()->GetLocation().y + (float)height / 2.f };
		//m_target.lock()->SetLocation(m_center);
		onScaled.Broadcast({ m_topleftCntrl.lock()->GetLocation().x,  m_topleftCntrl.lock()->GetLocation().y },
							{ (float)width, (float)height }, { (float)width / 2.f, (float)height / 2.f });

		m_outline.lock()->SetLocation({ m_topleftCntrl.lock()->GetLocation().x ,  m_topleftCntrl.lock()->GetLocation().y });		
		m_outline.lock()->SetSize({ (float)width ,  (float)height });
		m_translateCntrl.lock()->SetLocation({ m_topleftCntrl.lock()->GetLocation().x + 10.f ,  m_topleftCntrl.lock()->GetLocation().y + 10.f });
		m_translateCntrl.lock()->SetSize({ (float)width - 20.f,  (float)height - 20.f });

		m_tempTargetLoc = pos;

	}
	void TransformCntrl::onDragStart(weak<Object> btn, Vector2 pos)
	{
		if (m_isScaling)return;
		m_isScaling = true;	
		curDragCntrl = btn.lock()->GetId();
	}
	void TransformCntrl::onDragEnd(weak<Object> btn, Vector2 pos)
	{
		m_isScaling = false;
		curDragCntrl = "";
		onButtonUp.Broadcast(pos);
		//Logger::Get()->Trace(std::format(" TransformCntrl::onDragOut() m_center x {} | y {}  \n", m_center.x, m_center.y));
	}
	void TransformCntrl::onDragOut(weak<Object> btn)
	{
		m_isScaling = false;
		curDragCntrl = "";
		onStop.Broadcast();
	}
	void TransformCntrl::onTranslateStart(weak<Object>, Vector2 pos) {
		if (m_isScaling)return;
		m_tempTargetLoc = pos;
		m_isTranslating = true;		
	}
	void TransformCntrl::onTranslateContinue(weak<Object>, Vector2 pos) {

		if (m_isScaling)return;
		Vector2 offset = { pos.x - m_tempTargetLoc.x, pos.y - m_tempTargetLoc.y };
		//SetLocation({m_location.x + offset.x - m_pivot.x , m_location.y + offset.y - m_pivot.y });

		float width = GetVectorLength(Direction(m_toprightCntrl.lock()->GetLocation(), m_topleftCntrl.lock()->GetLocation()));
		float height = GetVectorLength(Direction(m_bottomleftCntrl.lock()->GetLocation(), m_topleftCntrl.lock()->GetLocation()));		
		m_translateCntrl.lock()->Offset(offset);
		m_outline.lock()->Offset(offset);
		m_topleftCntrl.lock()->Offset(offset);
		m_toprightCntrl.lock()->Offset(offset);
		m_bottomleftCntrl.lock()->Offset(offset);
		m_bottomrightCntrl.lock()->Offset(offset);
		onMoved.Broadcast({ m_topleftCntrl.lock()->GetLocation().x,  m_topleftCntrl.lock()->GetLocation().y });
		m_tempTargetLoc = pos;
	}
	void TransformCntrl::onTranslateEnd(weak<Object>) {
		m_isTranslating = false;
		onStop.Broadcast();
	}
	void TransformCntrl::onButtonClick(weak<Object> btn, Vector2 pos)
	{
		onButtonUp.Broadcast(pos);
	}
#pragma endregion

#pragma region  Helper
	bool TransformCntrl::IsActiveCtrl(std::string _cntrl) {

		return curDragCntrl  == _cntrl;
	}
	void TransformCntrl::Draw(float _deltaTime)
	{
	
	}
	void TransformCntrl::Reset(){
		//m_center = { m_targetInitState.x + m_targetInitState.width / 2.f, m_targetInitState.y + m_targetInitState.height / 2.f };
		onScaleHandler(m_topleftCntrl, { m_targetInitState.x, m_targetInitState.y });
		m_tempTargetLoc = { m_targetInitState.x , m_targetInitState.y };
	
	}

	Rectangle TransformCntrl::GetBounds() 
	{
		Vector2 lt = m_topleftCntrl.lock()->GetLocation();
		Vector2 br = m_bottomrightCntrl.lock()->GetLocation();
		return { lt.x , lt.y,  br.x - lt.x , br.y - lt.y };

	}
	void TransformCntrl::SetDefaultRect(Rectangle rect)
	{
		m_targetInitState = rect;
		if (!m_translateCntrl.expired()) {
			m_translateCntrl.lock()->SetLocation({ m_targetInitState.x + 10.f , m_targetInitState.y + 10.f });
			m_translateCntrl.lock()->SetSize({ m_targetInitState.width - 20.f, m_targetInitState.height - 20.f});
		}
		if (!m_outline.expired())
		{
			m_outline.lock()->SetLocation({ m_targetInitState.x , m_targetInitState.y });
			m_outline.lock()->SetSize({ m_targetInitState.width , m_targetInitState.height });
		}
		if (!m_topleftCntrl.expired())
				m_topleftCntrl.lock()->SetLocation({ m_targetInitState.x , m_targetInitState.y });
		
		if (!m_toprightCntrl.expired())
				m_toprightCntrl.lock()->SetLocation({ m_targetInitState.x + m_targetInitState.width , m_targetInitState.y });
	
		if (!m_bottomleftCntrl.expired()) 
				m_bottomleftCntrl.lock()->SetLocation({ m_targetInitState.x , m_targetInitState.y + m_targetInitState.height });
		
		if (!m_bottomrightCntrl.expired())
				m_bottomrightCntrl.lock()->SetLocation({ m_targetInitState.x + m_targetInitState.width, m_targetInitState.y + m_targetInitState.height });

		
		SetPivot({ m_targetInitState.width * 0.5f, m_targetInitState.height * 0.5f });
			/*m_center = { m_targetInitState.x + m_targetInitState.width/2.f,
					m_targetInitState.y + m_targetInitState.height/2.f};*/
	}

	void TransformCntrl::SetMinMaxRect(Rectangle rect)
	{
		m_MinMaxRect = rect;
	}


#pragma endregion
#pragma region  Loop


	void TransformCntrl::Update(float _deltaTime)
	{
		
	}
	
#pragma endregion
#pragma region CleanUp
	void TransformCntrl::Destroy()
	{
		if (m_isPendingDestroy)return;

		UIElement::Destroy();
		m_topleftCntrl.reset();
		m_toprightCntrl.reset();
		m_bottomleftCntrl.reset();
		m_bottomrightCntrl.reset();
		m_translateCntrl.reset();
		m_outline.reset();
	}
	TransformCntrl::~TransformCntrl()
	{
		std::string log_str = "Transfrom Control Destroyed";
	}
#pragma endregion
}