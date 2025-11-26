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


	TransformCntrl::TransformCntrl(World* _owningworld, const std::string& id, Vector2 minsize, Vector2 maxsize , Rectangle targetInitState)
		: UIElement{ _owningworld, id },
		 m_topleftCntrl{},
		 m_toprightCntrl{},
		  m_bottomleftCntrl{},
		m_bottomrightCntrl{},
		m_outline{},
		m_translateCntrl{},
		cntrlsize{ 6.f },
		cntrlhalf{ cntrlsize /2},
		curDragCntrl{""},
		m_isScaling{false},
		m_isTranslating{false},
		m_tempTargetLoc{},
		m_minSize{ minsize },
		m_maxSize{maxsize},
		m_targetInitState{ targetInitState },
		m_aspectRatio{},
		m_isfixedAspectRatio{},
		m_tmpPivot{}
	{
		
		m_center = { targetInitState.x + targetInitState.width/2.f,
					targetInitState.y + targetInitState.height/2.f};
	}

	void TransformCntrl::Init()
	{
		
		UIElement::Init();

	}

	void TransformCntrl::Start()
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
		btnprop.location = { m_location.x , m_location.y };
		btnprop.size = { m_targetInitState.width, m_targetInitState.height };
		btnprop.color = { 0 };
		btnprop.btncol = { 0 };
		btnprop.overcol = { 0 };
		btnprop.downcol = { 0 };
		btnprop.component = LAYOUT_COMPONENT;
		btnprop.size = { m_targetInitState.width   , m_targetInitState.height };

		m_translateCntrl = AddButton(btnid, btnprop);
		m_translateCntrl.lock()->onButtonDown.BindAction(GetWeakRef(), &TransformCntrl::onTranslateStart);
		m_translateCntrl.lock()->onButtonDrag.BindAction(GetWeakRef(), &TransformCntrl::onTranslateContinue);
		m_translateCntrl.lock()->onButtonOut.BindAction(GetWeakRef(), &TransformCntrl::onTranslateEnd);
		m_translateCntrl.lock()->onButtonUp.BindAction(GetWeakRef(), &TransformCntrl::onButtonClick);



		std::string id = "outline-cntrl";
		UI_Properties lnui = {};
		lnui.location = m_location;
		lnui.size = { m_targetInitState.width, m_targetInitState.height };
		lnui.color = BLUE;
		lnui.shapetype = SHAPE_TYPE::LINE;
		lnui.linewidth = 1;
		lnui.component = LAYOUT_COMPONENT;
		m_outline = m_owningworld->SpawnActor<Shape>(id);
		m_outline.lock()->SetUIProperties(lnui);
		m_outline.lock()->SetVisible(true);
		m_outline.lock()->Init();
		AddChild(m_outline);


		cntrlui.location = { m_targetInitState.x, m_targetInitState.y };
		cntrlui.color = SKYBLUE;
		cntrlui.btncol = SKYBLUE;
		cntrlui.overcol = ORANGE;
		cntrlui.downcol = ORANGE;
		cntrlui.pivot = { cntrlhalf, cntrlhalf };
		cntrlui.shapetype = SHAPE_TYPE::CIRCLE;
		cntrlui.component = LAYOUT_COMPONENT;
		id = "topleft-cntrl";
		m_topleftCntrl = AddButton(id, cntrlui);
		m_topleftCntrl.lock()->onButtonDrag.BindAction(GetWeakRef(), &TransformCntrl::onScaleHandler);
		m_topleftCntrl.lock()->onButtonDown.BindAction(GetWeakRef(), &TransformCntrl::onDragStart);
		m_topleftCntrl.lock()->onButtonUp.BindAction(GetWeakRef(), &TransformCntrl::onDragEnd);
		m_topleftCntrl.lock()->onButtonOut.BindAction(GetWeakRef(), &TransformCntrl::onDragOut);





		id = "topright-cntrl";
		cntrlui.location = { m_targetInitState.x + m_targetInitState.width , m_targetInitState.y };
		m_toprightCntrl = AddButton(id, cntrlui);
		m_toprightCntrl.lock()->onButtonDrag.BindAction(GetWeakRef(), &TransformCntrl::onScaleHandler);
		m_toprightCntrl.lock()->onButtonDown.BindAction(GetWeakRef(), &TransformCntrl::onDragStart);
		m_toprightCntrl.lock()->onButtonUp.BindAction(GetWeakRef(), &TransformCntrl::onDragEnd);
		m_toprightCntrl.lock()->onButtonOut.BindAction(GetWeakRef(), &TransformCntrl::onDragOut);


		id = "bottomleft-cntrl";
		cntrlui.location = { m_targetInitState.x, m_targetInitState.y + m_targetInitState.height };
		m_bottomleftCntrl = AddButton(id, cntrlui);
		m_bottomleftCntrl.lock()->onButtonDrag.BindAction(GetWeakRef(), &TransformCntrl::onScaleHandler);
		m_bottomleftCntrl.lock()->onButtonDown.BindAction(GetWeakRef(), &TransformCntrl::onDragStart);
		m_bottomleftCntrl.lock()->onButtonUp.BindAction(GetWeakRef(), &TransformCntrl::onDragEnd);
		m_bottomleftCntrl.lock()->onButtonOut.BindAction(GetWeakRef(), &TransformCntrl::onDragOut);


		id = "bottomright-cntrl";
		cntrlui.location = { m_targetInitState.x + m_targetInitState.width , m_targetInitState.y + m_targetInitState.height };
		m_bottomrightCntrl = AddButton(id, cntrlui);
		m_bottomrightCntrl.lock()->onButtonDrag.BindAction(GetWeakRef(), &TransformCntrl::onScaleHandler);
		m_bottomrightCntrl.lock()->onButtonDown.BindAction(GetWeakRef(), &TransformCntrl::onDragStart);
		m_bottomrightCntrl.lock()->onButtonUp.BindAction(GetWeakRef(), &TransformCntrl::onDragEnd);
		m_bottomrightCntrl.lock()->onButtonOut.BindAction(GetWeakRef(), &TransformCntrl::onDragOut);

		id = "";
		btnprop = {};
		cntrlui = {};

		UIElement::Start();
	}
	
#pragma endregion
#pragma region Event Handler
	void TransformCntrl::onScaleHandler(weak<Object> btn, Vector2 pos)
	{	
		
		Vector2 p = {m_topleftCntrl.lock()->GetLocation().x +  ((m_bottomrightCntrl.lock()->GetLocation().x - m_topleftCntrl.lock()->GetLocation().x)  / 2.f),  m_topleftCntrl.lock()->GetLocation().y + ((m_bottomrightCntrl.lock()->GetLocation().y - m_topleftCntrl.lock()->GetLocation().y) / 2.f) };
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
		
		double width = GetVectorLength(Direction({ trxpos, trypos }, { tlxpos, tlypos }));
		double height = GetVectorLength(Direction({ blxpos, blypos }, { tlxpos, tlypos }));

		if (width < m_minSize.x || height < m_minSize.y || width > m_maxSize.x || height > m_maxSize.y)return;

		m_topleftCntrl.lock()->SetLocation({ tlxpos, tlypos });
		m_toprightCntrl.lock()->SetLocation({ trxpos, trypos});
		m_bottomleftCntrl.lock()->SetLocation({ blxpos, blypos });
		m_bottomrightCntrl.lock()->SetLocation({ brxpos, brypos });
		

		m_center = { m_topleftCntrl.lock()->GetLocation().x + (float)width / 2.f,  m_topleftCntrl.lock()->GetLocation().y + (float)height / 2.f };
		//m_target.lock()->SetLocation(m_center);
		onScaled.Broadcast(m_center, { (float)width, (float)height }, { (float)width / 2.f, (float)height / 2.f });

		m_outline.lock()->SetLocation({ m_topleftCntrl.lock()->GetLocation().x ,  m_topleftCntrl.lock()->GetLocation().y });
		
		m_outline.lock()->SetSize({ (float)width ,  (float)height });

		m_translateCntrl.lock()->SetLocation({ m_topleftCntrl.lock()->GetLocation().x ,  m_topleftCntrl.lock()->GetLocation().y });
		m_translateCntrl.lock()->SetSize({ (float)width,  (float)height });
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
		Logger::Get()->Trace(std::format(" TransformCntrl::onDragOut() m_center x {} | y {}  \n", m_center.x, m_center.y));
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
		SetLocation({m_location.x + offset.x, m_location.y + offset.y});

		float width = GetVectorLength(Direction(m_toprightCntrl.lock()->GetLocation(), m_topleftCntrl.lock()->GetLocation()));
		float height = GetVectorLength(Direction(m_bottomleftCntrl.lock()->GetLocation(), m_topleftCntrl.lock()->GetLocation()));
		m_center = { m_topleftCntrl.lock()->GetLocation().x + width / 2.f,  m_topleftCntrl.lock()->GetLocation().y + height / 2.f };
		//m_target.lock()->SetLocation(m_center);
		m_outline.lock()->SetLocation({ m_topleftCntrl.lock()->GetLocation().x ,  m_topleftCntrl.lock()->GetLocation().y });
		m_translateCntrl.lock()->SetLocation({ m_topleftCntrl.lock()->GetLocation().x ,  m_topleftCntrl.lock()->GetLocation().y });
		onMoved.Broadcast(m_center);
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
	Rectangle TransformCntrl::GetBounds() 
	{
		Vector2 lt = m_topleftCntrl.lock()->GetLocation();
		Vector2 br = m_bottomrightCntrl.lock()->GetLocation();
		return { lt.x , lt.y,  br.x - lt.x , br.y - lt.y };

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
		m_topleftCntrl.lock()->Destroy();
		m_toprightCntrl.lock()->Destroy();
		m_bottomleftCntrl.lock()->Destroy();
		m_bottomrightCntrl.lock()->Destroy();
		m_translateCntrl.lock()->Destroy();
		m_outline.lock()->Destroy();
		UIElement::Destroy();
	}
	TransformCntrl::~TransformCntrl()
	{
		std::string log_str = "Transfrom Control Destroyed";
	}
#pragma endregion
}