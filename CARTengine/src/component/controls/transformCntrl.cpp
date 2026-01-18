#include "component/controls/transformCntrl.h"
#include <memory>
#include "Types.h"
#include "UIButton.h"
#include "Shape.h"
#include "MathUtility.h"
#include "World.h"
#include "UICanvas.h"
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
		cntrlsize{ 24.f },
		cntrlhalf{ cntrlsize /2},
		curDragCntrl{""},
		m_isScaling{false},
		m_isTranslating{false},
		m_tempTargetLoc{},
		//m_targetInitState{},
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

		m_translateCntrl = m_owningworld->SpawnActor<UIButton>(std::string{ "translatebtn" });
		m_translateCntrl.lock().get()->onButtonDown.BindAction(GetWeakRef(), &TransformCntrl::onTranslateStart);
		m_translateCntrl.lock().get()->onButtonDrag.BindAction(GetWeakRef(), &TransformCntrl::onTranslateContinue);
		m_translateCntrl.lock().get()->onButtonOut.BindAction(GetWeakRef(), &TransformCntrl::onTranslateEnd);
		m_translateCntrl.lock().get()->onButtonUp.BindAction(GetWeakRef(), &TransformCntrl::onButtonClick);
		AddChild(m_translateCntrl);


		m_outline = m_owningworld->SpawnActor<Shape>(std::string{ "outline-cntrl" });
		AddChild(m_outline); // Outline


		m_topleftCntrl = m_owningworld->SpawnActor<UIButton>(std::string{ "topleftcntrl" });;// TOP LEFT control
		AddChild(m_topleftCntrl);
		m_topleftCntrl.lock()->onButtonDrag.BindAction(GetWeakRef(), &TransformCntrl::onScaleHandler);
		m_topleftCntrl.lock()->onButtonDown.BindAction(GetWeakRef(), &TransformCntrl::onDragStart);
		m_topleftCntrl.lock()->onButtonUp.BindAction(GetWeakRef(), &TransformCntrl::onDragEnd);
		m_topleftCntrl.lock()->onButtonOut.BindAction(GetWeakRef(), &TransformCntrl::onDragOut);

		m_toprightCntrl = m_owningworld->SpawnActor<UIButton>(std::string{ "toprightcntrl" });// TOP RIGHT control
		AddChild(m_toprightCntrl);
		m_toprightCntrl.lock()->onButtonDrag.BindAction(GetWeakRef(), &TransformCntrl::onScaleHandler);
		m_toprightCntrl.lock()->onButtonDown.BindAction(GetWeakRef(), &TransformCntrl::onDragStart);
		m_toprightCntrl.lock()->onButtonUp.BindAction(GetWeakRef(), &TransformCntrl::onDragEnd);
		m_toprightCntrl.lock()->onButtonOut.BindAction(GetWeakRef(), &TransformCntrl::onDragOut);

		m_bottomleftCntrl = m_owningworld->SpawnActor<UIButton>(std::string{ "bottumleftcntrl" });
		AddChild(m_bottomleftCntrl);
		m_bottomleftCntrl.lock()->onButtonDrag.BindAction(GetWeakRef(), &TransformCntrl::onScaleHandler);
		m_bottomleftCntrl.lock()->onButtonDown.BindAction(GetWeakRef(), &TransformCntrl::onDragStart);
		m_bottomleftCntrl.lock()->onButtonUp.BindAction(GetWeakRef(), &TransformCntrl::onDragEnd);
		m_bottomleftCntrl.lock()->onButtonOut.BindAction(GetWeakRef(), &TransformCntrl::onDragOut);

		m_bottomrightCntrl = m_owningworld->SpawnActor<UIButton>(std::string{ "bottomrightcntrl" });// BOTTOM RIGHT control
		AddChild(m_bottomrightCntrl);
		m_bottomrightCntrl.lock()->onButtonDrag.BindAction(GetWeakRef(), &TransformCntrl::onScaleHandler);
		m_bottomrightCntrl.lock()->onButtonDown.BindAction(GetWeakRef(), &TransformCntrl::onDragStart);
		m_bottomrightCntrl.lock()->onButtonUp.BindAction(GetWeakRef(), &TransformCntrl::onDragEnd);
		m_bottomrightCntrl.lock()->onButtonOut.BindAction(GetWeakRef(), &TransformCntrl::onDragOut);


		//SetLocation({ m_targetInitState.x, m_targetInitState.y });
		Rectangle rect = GetBounds();
		std::string btnid = "translate-btn";
		Btn_Text_Properties trn_prop = {};
		trn_prop.location = { rect.x + rect.width * 0.5f, rect.y + rect.height * 0.5f };
		trn_prop.pivot = { 0.5f, 0.5f};
		trn_prop.size = { rect.width - 20.f, rect.height - 20.f};
		trn_prop.color = { 255,255,0,0 };
		trn_prop.btncol = { 255,255,0,0 };
		trn_prop.overcol = { 255,255,0,0 };
		trn_prop.downcol = {255,255,0,0 };
		m_translateCntrl.lock().get()->SetUIProperties(trn_prop);
		m_translateCntrl.lock().get()->SetVisible(true);
		m_translateCntrl.lock().get()->Init();
	
		

		//======================================

		std::string id = "outline-cntrl";
		UI_Properties lnui = {};
		lnui.location = { rect.x + rect.width * 0.5f, rect.y + rect.height * 0.5f };
		lnui.size = { rect.width, rect.height };
		lnui.pivot = { 0.5f, 0.5f };
		lnui.color = {255,255,255,0};
		lnui.shapetype = SHAPE_TYPE::LINE;
		lnui.linewidth = 2;
		lnui.bordercol = BLUE;

		m_outline.lock()->SetUIProperties(lnui);
		m_outline.lock()->SetVisible(true);
		m_outline.lock()->Init();
		//======================================

		Btn_Text_Properties cntrlui = {};
		cntrlui.size = { cntrlsize, cntrlsize };
		cntrlui.location = { rect.x, rect.y };
		cntrlui.color = SKYBLUE;
		cntrlui.btncol = SKYBLUE;
		cntrlui.overcol = ORANGE;
		cntrlui.downcol = ORANGE;
		cntrlui.pivot = { 0.5f, 0.5f };
		cntrlui.shapetype = SHAPE_TYPE::RECTANGLE;
		//====================================== 
		// top left cntrl;
		m_topleftCntrl.lock()->SetButtonProperties(cntrlui);
		m_topleftCntrl.lock()->SetVisible(true);
		m_topleftCntrl.lock()->Init();
		//====================================== 

		// top right cntrl;
		cntrlui.location = { rect.x + rect.width , rect.y };
		m_toprightCntrl.lock()->SetButtonProperties(cntrlui);
		m_toprightCntrl.lock()->SetVisible(true);
		m_toprightCntrl.lock()->Init();
		
		//==============================================

		// bottom left cntrl;
		cntrlui.location = { rect.x, rect.y + rect.height };
		m_bottomleftCntrl.lock()->SetButtonProperties(cntrlui);
		m_bottomleftCntrl.lock()->SetVisible(true);
		m_bottomleftCntrl.lock()->Init();

		//==============================================

		// bottom right cntrl;
		cntrlui.location = { rect.x + rect.width , rect.y + rect.height };
		m_bottomrightCntrl.lock()->SetButtonProperties(cntrlui);
		m_bottomrightCntrl.lock()->SetVisible(true);
		m_bottomrightCntrl.lock()->Init();
		//=========================================================
		cntrlui = {};


		Actor::Start();

	}
	
#pragma endregion
#pragma region Event Handler
	void TransformCntrl::onScaleHandler(weak<Object> btn, Vector2 pos)
	{	
		float scrScale = World::UI_CANVAS.get()->Scale();
		Rectangle rect = { m_rawlocation.x, m_rawlocation.y, m_defaultSize.x, m_defaultSize.y };
		Rectangle curRect = GetBounds();
		Vector2 center = { curRect.x + curRect.width * 0.5f , curRect.y + curRect.height * 0.5f };
		Vector2 dir  = Direction( center, pos);
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
		
		Rectangle newRect = { tlxpos + (trxpos - tlxpos) * 0.5f,
								tlypos + (blypos - tlypos) * 0.5f,
								trxpos - tlxpos, blypos - tlypos
		};

		//m_center = { m_topleftCntrl.lock()->GetLocation().x + (float)width / 2.f,  m_topleftCntrl.lock()->GetLocation().y + (float)height / 2.f };
		//m_target.lock()->SetLocation(m_center);
		onScaled.Broadcast(newRect);

		SetSize({ newRect.width,  newRect.height });
		m_outline.lock()->SetLocation({ newRect.x , newRect.y });
		m_outline.lock()->SetSize({ newRect.width ,  newRect.height });
		m_translateCntrl.lock()->SetLocation({newRect.x + 10.f , newRect.y + 10.f });
		m_translateCntrl.lock()->SetSize({ newRect.width , newRect.height });

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


		Rectangle topRect = m_topleftCntrl.lock()->GetBounds();
		Rectangle rect = GetBounds();
		
		m_location = { topRect.x + (topRect.width * 0.5f) +  (rect.width * 0.5f),  topRect.y + (topRect.height * 0.5f ) + (rect.height * 0.5f) };
		onMoved.Broadcast(m_location);
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
		UIElement::Draw(_deltaTime);
	}
	void TransformCntrl::Reset(){
		//m_center = { m_targetInitState.x + m_targetInitState.width / 2.f, m_targetInitState.y + m_targetInitState.height / 2.f };
		onScaleHandler(m_topleftCntrl, m_rawlocation);
		m_tempTargetLoc = m_rawlocation;
	
	}

	Rectangle TransformCntrl::GetBounds() 
	{
		return UIElement::GetBounds();
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