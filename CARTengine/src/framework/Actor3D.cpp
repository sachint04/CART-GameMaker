#include "Actor3D.h"
#include "AssetManager.h"
namespace cart
{
#pragma region Cunstructor & Init

	Actor3D::Actor3D(World* _owningworld, const std::string& _id)
		:Actor{ _owningworld, _id }, 
		m_model{ }, 
		m_rotation{0},
		m_ray{ 0 }, 
		m_collision{ 0 },  
		m_isTouch{0},
		m_isHover{0},
		m_currentFrame{0},
		m_currentAnimation{0},
		m_label{},
		m_bShowLabel{false},
		m_bPlayAnim{false},
		m_strfont{},
		m_fontSize{18},
		m_fontSpace{2.f}
	{
	}
	void Actor3D::Init()
	{
		
	}
#pragma endregion
	
#pragma region Loop

	void Actor3D::Update(float _deltaTime)
	{
		if (!m_visible)return;
		Actor::Update(_deltaTime);
	}
	void Actor3D::Update3D(float _deltaTime, const Camera& camera)
	{
		if (!m_visible)return;
		
		m_ray = GetScreenToWorldRay(GetMousePosition(), camera);
	
		
		m_collision = GetRayCollisionBox(m_ray,
		{
			{m_location3.x - m_width / 2, m_location3.y - m_height / 2, m_location3.z - m_zSize / 2},
			{m_location3.x + m_width / 2, m_location3.y + m_height / 2, m_location3.z + m_zSize / 2}
		});
		if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
		{			
			if (m_collision.hit)
			{
				onTouch.Broadcast(GetWeakRef(), GetMousePosition());
				m_currentFrame = 0;
			}	
		}

//		if (m_collision.hit) {
//			m_isHover = m_collision.hit;
//			
//		//	onHover.Broadcast(GetWeakRef(), GetMousePosition());
//		//	PlayHoverAnim();
////			std::cout << "mouse over card" << std::endl;
//		}
//		else {
//			m_isHover = false;
//		}
	
		if (!m_isHover) {
			if (m_collision.hit) {
				onHover.Broadcast(GetWeakRef(), GetMousePosition());					
				m_isHover = true;
			}
		}
		else {
			if (!m_collision.hit) {
				onOut.Broadcast(GetWeakRef(), GetMousePosition());
				m_isHover = false;
			}
		}
		if (m_bPlayAnim)
		{
			ModelAnimation anim = m_animations[m_currentAnimation];
			m_currentFrame++;
			if (m_currentFrame > anim.frameCount - 1)m_currentFrame = anim.frameCount - 1;
			UpdateModelAnimation(m_model, anim, m_currentFrame);
		}
	}
	void Actor3D::Draw(float _deltaTime)
	{
		if (!m_visible)return;
		Actor::Draw(_deltaTime);
	}
	void Actor3D::Draw3D(float _deltaTime, const Camera& camera)
	{
		if (!m_visible)return;
		BeginMode3D(camera);
			DrawModelEx(m_model, m_location3, { m_rotation.x, m_rotation.y, m_rotation.z }, m_rotation.w, {(float)m_width, (float)m_height, (float)m_zSize}, WHITE);
		EndMode3D();
		if (m_bShowLabel)
		{
			Vector2 pos = GetWorldToScreen(m_location3, camera);
			float boxWidth = 160;
			float boxHeight = 40;
			Rectangle rect{ pos.x - boxWidth/2.f, pos.y + 90.f, boxWidth, boxHeight};
			DrawRectangleRounded(rect, 0.8f, 2, { 255,255, 255, 150 });
			shared<Font> fnt = AssetManager::Get().LoadFontAsset(m_strfont, m_fontSize);
			if (fnt) {

				Vector2 fsize = MeasureTextEx(*fnt,m_label.c_str(), m_fontSize, m_fontSpace);
				DrawTextEx(*fnt, m_label.c_str(), { rect.x + (rect.width / 2.f - (fsize.x / 2.f)), rect.y + (rect.height / 2.f - fsize.y /2.f) }, m_fontSize, m_fontSpace, BLACK);
				fnt.reset();
			}
		}
		
	}
#pragma endregion
	
#pragma region  Helper


	void Actor3D::SetModel(Model model)
	{
		m_model = model;
	}
	void Actor3D::SetAnimations(ModelAnimation * animations, const int& count)
	{
		m_animations = animations;		
		m_animcount = count;
		m_currentAnimation = 0;
	/*	ModelAnimation anim = m_animations[0];
		UpdateModelAnimation(m_model, anim, m_currentFrame);*/
	}
	void Actor3D::PlayHoverAnim()
	{
		m_currentFrame = 0;
		m_currentAnimation = 1;
	}
	void Actor3D::PlayDefaultAnim()
	{
		m_currentFrame = 0;
		m_currentAnimation = 0;
		ModelAnimation anim = m_animations[0];
		UpdateModelAnimation(m_model, anim, m_currentFrame);
	}
	void Actor3D::SetLabel(const std::string& _label, const std::string& font, float size, float spacing)
	{
		m_label = _label;
		m_strfont = font;
		m_fontSize = size;
		m_fontSpace = spacing;
	}

	void Actor3D::ShowLabel(bool _flag)
	{
		m_bShowLabel = _flag;
	}

	void Actor3D::PlayAnimation(int index)
	{
		if (index >= 0) {
			m_bPlayAnim = true;
			m_currentAnimation = index;
		}
		else {
			PlayDefaultAnim();
			m_bPlayAnim = false;
		}
			m_currentFrame = 0;
	}

	

#pragma endregion

#pragma region CleanUp
	void Actor3D::Destroy()
	{
		SetVisible(false);
		Actor::Destroy();
	}
	Actor3D::~Actor3D()
	{		
		if (m_animations)delete m_animations;

		UnloadModel(m_model);
	}
#pragma endregion
}