#include "component/controls/transformCntrl.h"
#include <memory>
#include "Types.h"
#include "UIButton.h"
#include "Shape.h"
#include "MathUtility.h"
namespace cart {
	TransformCntrl::TransformCntrl(World* _owningworld, const std::string& id, weak<UIElement> target, Vector2 minsize, Vector2 maxsize)
		: UIElement{ _owningworld, id },
		 m_topleftCntrl{},
		 m_toprightCntrl{},
		  m_bottomleftCntrl{},
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
		m_center = { m_target.lock()->GetLocation().x ,
					m_target.lock()->GetLocation().y };
	}

	void TransformCntrl::Init()
	{
		
		Btn_Text_Properties cntrlui = {};		
		cntrlui.color = GRAY;
		cntrlui.size = { cntrlsize, cntrlsize };
		cntrlui.btncol = GRAY;
		cntrlui.overcol = GRAY;
		cntrlui.downcol = ORANGE;

		
		m_targetInitState.x  = m_target.lock()->GetBounds().x;
		m_targetInitState.y  = m_target.lock()->GetBounds().y;
		m_targetInitState.width = m_target.lock()->GetDefaultSize().x;
		m_targetInitState.height = m_target.lock()->GetDefaultSize().y;

		Rectangle rect = m_target.lock()->GetBounds();// m_target.lock()->GetBounds();
		
		m_location = { rect.x, rect.y };

		std::string btnid = "translate-btn";
		Btn_Text_Properties btnprop = {};
		btnprop.location = m_location;
		btnprop.size = { rect.width, rect.height };
		btnprop.color = {0};
		btnprop.btncol = {0};
		btnprop.overcol = {0};
		btnprop.downcol = {255 ,255, 255, 50};
		btnprop.size = { rect.width, rect.height };

		m_translateCntrl = AddButton(btnid, btnprop);
		m_translateCntrl.lock()->onButtonDown.BindAction(GetWeakRef(), &TransformCntrl::onTranslateStart);
		m_translateCntrl.lock()->onButtonDrag.BindAction(GetWeakRef(), &TransformCntrl::onTranslateContinue);
		m_translateCntrl.lock()->onButtonOut.BindAction(GetWeakRef(), &TransformCntrl::onTranslateEnd);



		std::string id = "outline-cntrl";

		m_outline = m_owningworld->SpawnActor<Shape>(id, m_location, rect.width, rect.height, BLUE, SHAPE_TYPE::LINE, 2);	
		m_outline.lock()->SetVisible(true);
		m_outline.lock()->Init();
		AddChild(m_outline);


		cntrlui.location = { rect.x, rect.y };
		cntrlui.color = SKYBLUE;
		cntrlui.btncol = SKYBLUE;
		cntrlui.overcol = SKYBLUE;
		cntrlui.downcol = SKYBLUE;
		cntrlui.pivot = { cntrlhalf, cntrlhalf };
		id = "topleft-cntrl";
		m_topleftCntrl = AddButton(id, cntrlui,SHAPE_TYPE::CIRCLE);
		m_topleftCntrl.lock()->onButtonDrag.BindAction(GetWeakRef(), &TransformCntrl::onScaleHandler);
		m_topleftCntrl.lock()->onButtonDown.BindAction(GetWeakRef(), &TransformCntrl::onDragStart);
		m_topleftCntrl.lock()->onButtonUp.BindAction(GetWeakRef(), &TransformCntrl::onDragEnd);
		m_topleftCntrl.lock()->onButtonOut.BindAction(GetWeakRef(), &TransformCntrl::onDragOut);

		

		id = "topright-cntrl";
		cntrlui.location = { rect.x + rect.width , rect.y  };
		m_toprightCntrl = AddButton(id, cntrlui,SHAPE_TYPE::CIRCLE);
		m_toprightCntrl.lock()->onButtonDrag.BindAction(GetWeakRef(), &TransformCntrl::onScaleHandler);
		m_toprightCntrl.lock()->onButtonDown.BindAction(GetWeakRef(), &TransformCntrl::onDragStart);
		m_toprightCntrl.lock()->onButtonUp.BindAction(GetWeakRef(), &TransformCntrl::onDragEnd);
		m_toprightCntrl.lock()->onButtonOut.BindAction(GetWeakRef(), &TransformCntrl::onDragOut);

	
		id = "bottomleft-cntrl";
		cntrlui.location = { rect.x, rect.y + rect.height  };
		m_bottomleftCntrl = AddButton(id, cntrlui,SHAPE_TYPE::CIRCLE);
		m_bottomleftCntrl.lock()->onButtonDrag.BindAction(GetWeakRef(), &TransformCntrl::onScaleHandler);
		m_bottomleftCntrl.lock()->onButtonDown.BindAction(GetWeakRef(), &TransformCntrl::onDragStart);
		m_bottomleftCntrl.lock()->onButtonUp.BindAction(GetWeakRef(), &TransformCntrl::onDragEnd);
		m_bottomleftCntrl.lock()->onButtonOut.BindAction(GetWeakRef(), &TransformCntrl::onDragOut);

		
		id = "bottomright-cntrl";
		cntrlui.location = { rect.x + rect.width , rect.y + rect.height  };
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

		float xpos = p.x + cos(DegreesToRadians(135.f)) * len;
		float ypos = p.y - sin(DegreesToRadians(135.f)) * len;
		m_topleftCntrl.lock()->SetLocation({ xpos, ypos });

		xpos = p.x + cos(DegreesToRadians(45.f)) * len;
		ypos = p.y - sin(DegreesToRadians(45.f)) * len;	
		m_toprightCntrl.lock()->SetLocation({ xpos, ypos});


		xpos = p.x + cos(DegreesToRadians(225.f)) * len;
		ypos = p.y - sin(DegreesToRadians(225.f)) * len;
		m_bottomleftCntrl.lock()->SetLocation({ xpos, ypos });


		xpos = p.x + cos(DegreesToRadians(315.f)) * len;
		ypos = p.y - sin(DegreesToRadians(315.f)) * len;
		m_bottomrightCntrl.lock()->SetLocation({ xpos, ypos });
		
		float width = GetVectorLength(Direction(m_toprightCntrl.lock()->GetLocation() , m_topleftCntrl.lock()->GetLocation()));
		float height = GetVectorLength(Direction( m_bottomleftCntrl.lock()->GetLocation(), m_topleftCntrl.lock()->GetLocation()));

	
	//	LOG("OnDragTopLeft() width %.2f  height %.2f m_center x %.2f  y %.2f", width, height, m_center.x , m_center.y);
		m_target.lock()->SetSize({width,height});
		m_target.lock()->SetPivot({width/2.f,height/2.f});

		m_center = { m_topleftCntrl.lock()->GetLocation().x + width / 2.f,  m_topleftCntrl.lock()->GetLocation().y + height / 2.f };
		m_target.lock()->SetLocation(m_center);

		m_outline.lock()->SetLocation({ m_topleftCntrl.lock()->GetLocation().x ,  m_topleftCntrl.lock()->GetLocation().y });
		m_outline.lock()->SetSize({ width,  height});

		m_translateCntrl.lock()->SetLocation({ m_topleftCntrl.lock()->GetLocation().x ,  m_topleftCntrl.lock()->GetLocation().y });
		m_translateCntrl.lock()->SetSize({ width,  height });	
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
		SetLocation({m_location.x + offset.x, m_location.y + offset.y});

		float width = GetVectorLength(Direction(m_toprightCntrl.lock()->GetLocation(), m_topleftCntrl.lock()->GetLocation()));
		float height = GetVectorLength(Direction(m_bottomleftCntrl.lock()->GetLocation(), m_topleftCntrl.lock()->GetLocation()));
		m_center = { m_topleftCntrl.lock()->GetLocation().x + width / 2.f,  m_topleftCntrl.lock()->GetLocation().y + height / 2.f };
	//	LOG("onTranslateContinue() | offset x %.2f y %.2f  | width %.2f  height %.2f | center x %.2f  y %.2f", offset.x, offset.y, width, height, m_center.x, m_center.y);
		m_target.lock()->SetLocation(m_center);
		m_outline.lock()->SetLocation({ m_topleftCntrl.lock()->GetLocation().x ,  m_topleftCntrl.lock()->GetLocation().y });
		m_translateCntrl.lock()->SetLocation({ m_topleftCntrl.lock()->GetLocation().x ,  m_topleftCntrl.lock()->GetLocation().y });
		m_tempTargetLoc = pos;
	}
	void TransformCntrl::onTranslateEnd(weak<Object>) {
	//	LOG("Translate Control End");
	}
	void TransformCntrl::TransformTarget(Rectangle rect) {

		Rectangle bound = m_target.lock()->GetBounds();
		if (m_isfixedAspectRatio) {
			if (rect.height < rect.width) {
				rect.height = rect.width / m_aspectRatio;
			}
			else {
				rect.width = rect.height * m_aspectRatio;
			}
			//rect.x = rect.x - rect.width / 2.f;
			//rect.y = rect.y - rect.height / 2.f;
		}
		Vector2 pivot = m_target.lock()->GetPivot();
		//m_target.lock()->SetLocation({ rect.x + m_tempTargetLoc 2.f ,pivot.y + rect.height /2.f});
		m_target.lock()->SetSize({ rect.width, rect.height });
		m_outline.lock()->SetLocation({ bound.x - pivot.x , bound.y  - pivot.y});
		LOG("TransformTarget x  %.2f  y %.2f width %.2f height %.2f ", rect.x, rect.y, rect.width, rect.height);
	//	m_outline.lock()->SetSize({ rect.width, rect.height });

	}

	void TransformCntrl::UpdateCntrls()
	{
		Rectangle rect = m_target.lock()->GetBounds();
		m_translateCntrl.lock()->SetLocation({ rect.x, rect.y });
		m_translateCntrl.lock()->SetSize({ rect.width, rect.height });
		m_outline.lock()->SetLocation({rect.x, rect.y});
		m_outline.lock()->SetSize({rect.width, rect.height});
		m_topleftCntrl.lock()->SetLocation({ rect.x - cntrlhalf, rect.y - cntrlhalf});
		m_toprightCntrl.lock()->SetLocation({ rect.x + rect.width - cntrlhalf, rect.y - cntrlhalf });
		m_bottomleftCntrl.lock()->SetLocation({ rect.x - cntrlhalf, rect.y + rect.height - cntrlhalf });
		m_bottomrightCntrl.lock()->SetLocation({ rect.x + rect.width - cntrlhalf, rect.y + rect.height - cntrlhalf });

	}

	bool TransformCntrl::IsActiveCtrl(std::string _cntrl) {

		return curDragCntrl  == _cntrl;
	}
	void TransformCntrl::Draw(float _deltaTime)
	{
	
	}
	void TransformCntrl::Reset(){
			
		//LOG("Reset() | init  x %.2f y %.2f ", m_targetInitState.x, m_targetInitState.y);
		SetLocation( { m_targetInitState.x , m_targetInitState.y });

		float width = GetVectorLength(Direction(m_toprightCntrl.lock()->GetLocation(), m_topleftCntrl.lock()->GetLocation()));
		float height = GetVectorLength(Direction(m_bottomleftCntrl.lock()->GetLocation(), m_topleftCntrl.lock()->GetLocation()));
		m_center = { m_topleftCntrl.lock()->GetLocation().x + width / 2.f,  m_topleftCntrl.lock()->GetLocation().y + height / 2.f };
		
		m_target.lock()->SetLocation(m_center);
		m_outline.lock()->SetLocation({ m_topleftCntrl.lock()->GetLocation().x ,  m_topleftCntrl.lock()->GetLocation().y });
		m_translateCntrl.lock()->SetLocation({ m_topleftCntrl.lock()->GetLocation().x ,  m_topleftCntrl.lock()->GetLocation().y });
		m_tempTargetLoc = { m_targetInitState.x , m_targetInitState.y };
		onScaleHandler(m_topleftCntrl, { m_targetInitState.x, m_targetInitState.y });
	
	}

	void TransformCntrl::Close()
	{
		m_target.lock()->SetVisible(false);
		m_outline.lock()->SetVisible(false);
		m_target.lock()->Destroy();
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
		
	}
}