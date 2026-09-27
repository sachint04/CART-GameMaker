#include <raylib.h>
#include <cmath>
#include "component/controls/transformCntrl.h"
#include <memory>
#include "Types.h"
#include "UIButton.h"
#include "ImageButton.h"
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
		//m_tempTargetLoc{},
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
#pragma region Create Childredn 

		m_translateCntrl = m_owningworld->SpawnActor<ImageButton>(std::string{ "translatebtn" });
		AddChild(m_translateCntrl);

		m_outline = m_owningworld->SpawnActor<Shape>(std::string{ "outline-cntrl" });
		AddChild(m_outline); // Outline

		m_topleftCntrl = m_owningworld->SpawnActor<ImageButton>(std::string{ "topleftcntrl" });;// TOP LEFT control
		AddChild(m_topleftCntrl);

		m_toprightCntrl = m_owningworld->SpawnActor<ImageButton>(std::string{ "toprightcntrl" });// TOP RIGHT control
		AddChild(m_toprightCntrl);

		m_bottomleftCntrl = m_owningworld->SpawnActor<ImageButton>(std::string{ "bottumleftcntrl" });
		AddChild(m_bottomleftCntrl);

		m_bottomrightCntrl = m_owningworld->SpawnActor<ImageButton>(std::string{ "bottomrightcntrl" });// BOTTOM RIGHT control
		AddChild(m_bottomrightCntrl);
		

#pragma endregion
		if (auto lock = m_translateCntrl.lock()) {
			lock->onButtonDown.BindAction(GetWeakRef(), &TransformCntrl::OnTranslateStartHandler);
			lock->onButtonDrag.BindAction(GetWeakRef(), &TransformCntrl::OnTranslateContinueHandler);
			lock->onButtonOut.BindAction(GetWeakRef(), &TransformCntrl::OnTranslateEndHandler);
			lock->onButtonUp.BindAction(GetWeakRef(), &TransformCntrl::OnButtonClickHandler);
		}
		if (auto lock = m_topleftCntrl.lock()) {
				lock->onButtonDrag.BindAction(GetWeakRef(), &TransformCntrl::onScaleHandler);
				lock->onButtonDown.BindAction(GetWeakRef(), &TransformCntrl::OnDragStartHandler);
				lock->onButtonUp.BindAction(GetWeakRef(), &TransformCntrl::OnDragEndHandler);
				lock->onButtonOut.BindAction(GetWeakRef(), &TransformCntrl::OnDragOutHandler);
		}
		if (auto lock = m_toprightCntrl.lock()) {
				lock->onButtonDrag.BindAction(GetWeakRef(), &TransformCntrl::onScaleHandler);
				lock->onButtonDown.BindAction(GetWeakRef(), &TransformCntrl::OnDragStartHandler);
				lock->onButtonUp.BindAction(GetWeakRef(), &TransformCntrl::OnDragEndHandler);
				lock->onButtonOut.BindAction(GetWeakRef(), &TransformCntrl::OnDragOutHandler);
		}
		if (auto lock = m_bottomleftCntrl.lock()) {
				lock->onButtonDrag.BindAction(GetWeakRef(), &TransformCntrl::onScaleHandler);
				lock->onButtonDown.BindAction(GetWeakRef(), &TransformCntrl::OnDragStartHandler);
				lock->onButtonUp.BindAction(GetWeakRef(), &TransformCntrl::OnDragEndHandler);
				lock->onButtonOut.BindAction(GetWeakRef(), &TransformCntrl::OnDragOutHandler);
		}
		if (auto lock = m_bottomrightCntrl.lock()) {
			lock->onButtonDrag.BindAction(GetWeakRef(), &TransformCntrl::onScaleHandler);
			lock->onButtonDown.BindAction(GetWeakRef(), &TransformCntrl::OnDragStartHandler);
			lock->onButtonUp.BindAction(GetWeakRef(), &TransformCntrl::OnDragEndHandler);
			lock->onButtonOut.BindAction(GetWeakRef(), &TransformCntrl::OnDragOutHandler);
		}

		Rectangle parentrect = {0,0, (float)GetScreenWidth(), (float)GetScreenHeight() };
		if (auto lock = m_parentObj.lock())
		{
			weak<UIElement> parent = std::dynamic_pointer_cast<UIElement>(lock);
			parentrect = parent.lock()->GetBounds();
		}

		//SetLocation({ m_targetInitState.x, m_targetInitState.y });
		Rectangle rect = GetBounds();
		Btn_Text_Properties trn_prop = {};
		trn_prop.location = { rect.x , rect.y };
		trn_prop.pivot = { 0, 0 };
		trn_prop.anchor = { 0, 0, 0, 0 };
		trn_prop.size = { rect.width, rect.height };
		trn_prop.color = { 255,255,0,0 };
		trn_prop.btncol = { 255,255,0,0 };
		trn_prop.overcol = { 255,255,0,05 };
		trn_prop.downcol = { 255,255,0,55 };
	/*	trn_prop.bordercol = { 255,255,0,255 };
		trn_prop.borderwidth = 10.f;*/
		trn_prop.dragable = true;
		trn_prop.dragableboundry = parentrect;
		trn_prop.component = NO_LAYOUT;

		if (auto lock = m_translateCntrl.lock()) {
			lock->SetButtonProperties(trn_prop);
			//lock->MaintainAspectRatio(true);
			lock->SetVisible(true);
			lock->Init();
		}



		//======================================
		UI_Properties lnui = {};
		lnui.location = { rect.x, rect.y };
		lnui.size = { rect.width, rect.height };
		lnui.pivot = { 0, 0 };
		lnui.anchor = { 0, 0, 0, 0 };
		lnui.color = { 255,0,255,0 };
		lnui.shapetype = SHAPE_TYPE::LINE;
		lnui.linewidth = 5;
		lnui.bordercol = BLUE;
		lnui.component = NO_LAYOUT;
		m_outline.lock()->SetUIProperties(lnui);
		m_outline.lock()->SetVisible(true);
		//m_outline.lock()->MaintainAspectRatio(true);
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
		cntrlui.dragable = true;
		cntrlui.dragableboundry = parentrect;
		cntrlui.shapetype = SHAPE_TYPE::RECTANGLE;
		//====================================== 
		// top left cntrl;
		m_topleftCntrl.lock()->SetButtonProperties(cntrlui);
		m_topleftCntrl.lock()->MaintainAspectRatio(true);
		m_topleftCntrl.lock()->SetVisible(true);
		m_topleftCntrl.lock()->Init();
		//====================================== 

		// top right cntrl;
		cntrlui.location = { rect.x + rect.width , rect.y };
		m_toprightCntrl.lock()->SetButtonProperties(cntrlui);
		m_toprightCntrl.lock()->MaintainAspectRatio(true);
		m_toprightCntrl.lock()->SetVisible(true);
		m_toprightCntrl.lock()->Init();

		//==============================================

		// bottom left cntrl;
		cntrlui.location = { rect.x, rect.y + rect.height };
		m_bottomleftCntrl.lock()->SetButtonProperties(cntrlui);
		m_bottomleftCntrl.lock()->MaintainAspectRatio(true);
		m_bottomleftCntrl.lock()->SetVisible(true);
		m_bottomleftCntrl.lock()->Init();

		//==============================================

		// bottom right cntrl;
		cntrlui.location = { rect.x + rect.width , rect.y + rect.height };
		m_bottomrightCntrl.lock()->SetButtonProperties(cntrlui);
		m_bottomrightCntrl.lock()->MaintainAspectRatio(true);
		m_bottomrightCntrl.lock()->SetVisible(true);
		m_bottomrightCntrl.lock()->Init();
		//=========================================================
		cntrlui = {};		
		UpdateUIControls();
		Actor::Start();

	}

#pragma endregion
#pragma region  LOOP


	//void TransformCntrl::Update(float _deltatime)
	//{
	//	if (!m_active || !m_visible)return;

	//	std::vector<Transform_Controls> rects = {
	//		{"tl", { m_location.x - (cntrlsize * 0.5f), m_location.y - (cntrlsize * 0.5f),cntrlsize, cntrlsize }},
	//		{"tr", { m_location.x + m_width - (cntrlsize * 0.5f), m_location.y - (cntrlsize * 0.5f),cntrlsize, cntrlsize }},
	//		{"bl", { m_location.x - (cntrlsize * 0.5f), m_location.y + m_height - (cntrlsize * 0.5f),cntrlsize, cntrlsize }},
	//		{"br", { m_location.x + m_width - (cntrlsize * 0.5f), m_location.y + m_height - (cntrlsize * 0.5f),cntrlsize, cntrlsize }},
	//		{"mo", { m_location.x , m_location.y ,m_width, m_height}}
	//	};
	//	Vector2 pos = GetMousePosition();
	//	for(auto iter = rects.begin(); iter != rects.end(); ++iter) 
	//	{
	//		
	//		if (CheckCollisionPointRec(pos, iter->rect)) {
	//			
	//			break;
	//		}
	//	}
	//	Rectangle rect = {m_location.x, m_location.y, m_width, m_height};
	//	
	//}

	void TransformCntrl::Draw(float _deltatime)
	{
	//	Rectangle rect = GetBounds();
		//DrawRectangle(rect.x, rect.y, rect.width, rect.height, { 0,255,255, 255 });
		UIElement::Draw(_deltatime);
	}
#pragma endregion
	
#pragma region Event Handler
	void TransformCntrl::onScaleHandler(weak<Object> btn, Vector2 pos)
	{	
		SetScale(pos);
	}
	void TransformCntrl::OnDragStartHandler(weak<Object> btn, Vector2 pos)
	{
		if (m_isScaling)return;
		m_isScaling = true;	
		curDragCntrl = btn.lock()->GetId();
	}
	void TransformCntrl::OnDragEndHandler(weak<Object> btn, Vector2 pos)
	{
		m_isScaling = false;
		curDragCntrl = "";
		onButtonUp.Broadcast(pos);
		//Logger::Get()->Trace(std::format(" TransformCntrl::onDragOut() m_center x {} | y {}  \n", m_center.x, m_center.y));
	}
	void TransformCntrl::OnDragOutHandler(weak<Object> btn)
	{
		m_isScaling = false;
		curDragCntrl = "";
		onStop.Broadcast();
	}
	void TransformCntrl::OnTranslateStartHandler(weak<Object>, Vector2 pos) {
		if (m_isScaling)return;
	//	m_tempTargetLoc = pos;
		m_isTranslating = true;		
	}
	void TransformCntrl::OnTranslateContinueHandler(weak<Object>, Vector2 pos) {

		if (m_isScaling)return;

		SetLocation(pos);
		UpdateUIControls();
		onMoved.Broadcast(m_location);

	}
	void TransformCntrl::OnTranslateEndHandler(weak<Object>) {
		m_isTranslating = false;
		onStop.Broadcast();
	}
	void TransformCntrl::OnButtonClickHandler(weak<Object> btn, Vector2 pos)
	{
		onButtonUp.Broadcast(pos);
	}
#pragma endregion
#pragma region  Helper
	void TransformCntrl::SetScale(Vector2 pos) {
		float scrScale = 1.f;// World::UI_CANVAS.get()->Scale();

		Rectangle curRect = GetBounds();

		Vector2 center = { curRect.x + curRect.width * 0.5f , curRect.y + curRect.height * 0.5f };
		Vector2 dir = Direction(center, pos);
		float halfW = m_MinMaxRect.x * 0.5f;
		float halfH = m_MinMaxRect.y * 0.5f;
		float minLen = std::hypot(halfW, halfH);

		halfW = m_MinMaxRect.width * 0.5f;
		halfH = m_MinMaxRect.height * 0.5f;
		float maxLen = std::hypot(halfW, halfH);

		double len = std::max(minLen, GetRawVectorLength(dir));
		len = std::min(maxLen, (float)len);
	//	Logger::Get()->Trace(std::format("TransformCntrl::SetScale() len {}, min len {} max len {} ", len, minLen, maxLen));
		if (len < 0)return;
		float tlxpos = center.x + cos(DegreesToRadians(135.f)) * len;
		float tlypos = center.y - sin(DegreesToRadians(135.f)) * len;

		float trxpos = center.x + cos(DegreesToRadians(45.f)) * len;
		float trypos = center.y - sin(DegreesToRadians(45.f)) * len;


		float blxpos = center.x + cos(DegreesToRadians(225.f)) * len;
		float blypos = center.y - sin(DegreesToRadians(225.f)) * len;


		float brxpos = center.x + cos(DegreesToRadians(315.f)) * len;
		float brypos = center.y - sin(DegreesToRadians(315.f)) * len;
		//	Logger::Get()->Trace(std::format("TransformCntrl::onScaleHandler() bounds x {} | y {} |  w {} | h {} ", center.x, center.y, curRect.width, curRect.height));
		//	Logger::Get()->Trace(std::format("TransformCntrl::onScaleHandler() pos x {} | y {} | center x {} | y {} ", pos.x, pos.y, center.x, center.y));
		//Logger::Get()->Trace(std::format("TransformCntrl::onScaleHandler() len {} | tlx {} | tly {} | trx {} | try {}  ", len, tlxpos, tlypos, trxpos, trxpos));

		int width = GetVectorLength(Direction({ trxpos, trypos }, { tlxpos, tlypos }));
		int height = GetVectorLength(Direction({ blxpos, blypos }, { tlxpos, tlypos }));
		
		if (width < m_MinMaxRect.x)width = m_MinMaxRect.x;
		if (width > m_MinMaxRect.width)width = m_MinMaxRect.width;
		if (height < m_MinMaxRect.y)height = m_MinMaxRect.y;
		if (height > m_MinMaxRect.height)height = m_MinMaxRect.height;

		SetLocation({ tlxpos , tlypos });
		SetSize({ (float)width, (float)height });
		UpdateRawSize({ (float)width, (float)height });
	//	Logger::Get()->Trace(std::format("TransformCntrl::onScaleHandler() x {} | y {} | w {} | h{}  ", tlxpos, tlypos, width, height));
		UpdateUIControls();
		onScaled.Broadcast({ tlxpos , tlypos, (float)width, (float)height });
	}
	void TransformCntrl::UpdateUIControls() {

		Rectangle rect = GetBounds();
		//Rectangle rect = { m_location.x, m_location.y,   m_width,  m_height };
		if (auto lock = m_outline.lock())
		{		
			lock->SetLocation({ rect.x  , rect.y });
			lock->SetSize({ rect.width , rect.height});
			lock->UpdateRawSize({ rect.width, rect.height });
			//Logger::Get()->Trace(std::format("TransformCntrl::UpdateUIControls() rec x {} | y {} | w {} | h {} ", rect.x, rect.y, rect.width, rect.height));
		}
		if (auto lock = m_translateCntrl.lock())
		{
			if (!lock->IsDragging()) {
				lock->SetLocation({ rect.x  , rect.y });
				Rectangle targetRect = lock->GetBounds();
				if (targetRect.width != rect.width || targetRect.height != rect.height) {
					lock->SetSize({ rect.width , rect.height });
					lock->UpdateRawSize({ rect.width, rect.height});
				}
			}
		}
		if (auto lock = m_topleftCntrl.lock())
		{
			lock->SetLocation({ rect.x, rect.y });
		
		}

		if (auto lock = m_toprightCntrl.lock())
		{
			lock->SetLocation({ rect.x + rect.width, rect.y });
		
		}
		if (auto lock = m_bottomleftCntrl.lock())
		{
			lock->SetLocation({ rect.x, rect.y + rect.height });
		
		}
		if (auto lock = m_bottomrightCntrl.lock())
		{
			lock->SetLocation({ rect.x + rect.width, rect.y + rect.height });
		
		}
		
	}
	bool TransformCntrl::IsActiveCtrl(std::string _cntrl) {
		return curDragCntrl  == _cntrl;
	}
	void TransformCntrl::Reset(){
		//m_center = { m_targetInitState.x + m_targetInitState.width / 2.f, m_targetInitState.y + m_targetInitState.height / 2.f };
		UpdateUIControls();//	onScaleHandler(m_topleftCntrl, m_rawlocation);
		//m_tempTargetLoc = m_rawlocation;
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


		onScaled.Destroy();
		onRotated.Destroy();
		onMoved.Destroy();
		onButtonUp.Destroy();
		onStop.Destroy();
		
	}
	TransformCntrl::~TransformCntrl()
	{
		std::string log_str = "Transfrom Control Destroyed";
	}
#pragma endregion
}