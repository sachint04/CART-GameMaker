#include "Actor3D.h"
#include "World.h"
#include "AssetManager.h"


namespace cart
{
#pragma region Cunstructor & Init

	Actor3D::Actor3D(World* _owningworld, const std::string& _id)
		:Actor{ _owningworld, _id }, 
		m_model{ }, 
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
		m_fontSpace{2.f},
		m_animations{nullptr},
		m_bPlayAnimReverse{false},
		m_loopAnim{false}
	{
	}
	void Actor3D::Init()
	{
		Actor::Init();
	}

	void Actor3D::Start()
	{
		Actor::Start();
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
		if (m_bPlayAnim)
		{
			ModelAnimation anim = m_animations[m_currentAnimation];
			if (m_currentFrame < 0 || m_currentFrame > anim.frameCount - 1)
			{
				//m_currentFrame = anim.frameCount - 1;
				if (!m_loopAnim)
				{
					m_bPlayAnim = false;
					onAnimFinish.Broadcast(GetWeakRef(), m_currentAnimation);
				}
				else {
					PlayAnimation(m_currentAnimation, m_bPlayAnimReverse, m_loopAnim);
				}

			}
			else {
				UpdateModelAnimation(m_model, anim, m_currentFrame);
				m_currentFrame = !m_bPlayAnimReverse ? m_currentFrame + 1 : m_currentFrame - 1;
			}
		}
		if (!m_visible)return;
		
		m_ray = GetScreenToWorldRay(GetMousePosition(), camera);
	
		
		m_collision = GetRayCollisionBox(m_ray,
		{
			{m_location3.x - m_width / 2, m_location3.y - m_height / 2, m_location3.z - m_zSize / 2},
			{m_location3.x + m_width / 2, m_location3.y + m_height / 2, m_location3.z + m_zSize / 2}
		});
	


		if (!m_owningworld->GetHUD().lock()->IsMouseOverUI(GetMousePosition()) && m_active) {
			if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
			{
				if (m_collision.hit)
				{
					onTouch.Broadcast(GetWeakRef(), GetMousePosition());
					//m_currentFrame = 0;
				}
			}
			if ( !m_isHover) {
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
		}
		
	}
	void Actor3D::Draw(float _deltaTime)
	{
		if (!m_visible)return;
		Actor::Draw(_deltaTime);
		if (m_bShowLabel)
		{
			Vector2 pos = GetWorldToScreen(m_location3, m_owningworld->GetApplication()->GetCamera());
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
	void Actor3D::Draw3D(float _deltaTime, const Camera& camera)
	{
		if (!m_visible || m_isPendingDestroy)return;
			DrawModelEx(m_model, m_location3, { m_rotation3.x, m_rotation3.y, m_rotation3.z }, m_rotation3.w, {(float)m_width, (float)m_height, (float)m_zSize}, WHITE);
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
	void Actor3D::PlayAnimation(int index, bool reverse, bool loop)
	{
		
		if (index >= 0) {
			m_bPlayAnim = true;
			m_currentAnimation = index;
		}
		else {
			PlayDefaultAnim();
			m_bPlayAnim = false;
		}
		m_bPlayAnimReverse = reverse;	
		m_loopAnim = loop;
		
		int framecount = m_animations[m_currentAnimation].frameCount;
		m_currentFrame = !m_bPlayAnimReverse? 0 : framecount - 1;
	}
	bool Actor3D::UpdateTexture(const std::string& path, int matIndex) {

		shared<Texture2D> tex = AssetManager::Get().LoadTextureAsset(path, LOCKED);
		SetTextureFilter(*tex, TEXTURE_FILTER_BILINEAR);
		if (!tex)
		{
			return false;
		}
		int count = m_model.materialCount;
		SetMaterialTexture(&m_model.materials[matIndex], MATERIAL_MAP_DIFFUSE, *tex);
		return true;
	}

	bool Actor3D::IsAnimPlaying()
	{
		return m_bPlayAnim;
	}

#pragma endregion

#pragma region CleanUp
	void Actor3D::Destroy()
	{
		if (m_animations && IsModelAnimationValid(m_model, *m_animations)) {

			UnloadModelAnimations(m_animations, m_animcount);
		}

		if (m_model.meshes != NULL && IsModelValid(m_model)  )
		{
			UnloadModel(m_model);
			m_model.meshes = NULL;

		}
	
		
		SetVisible(false);
		Actor::Destroy();
	}
	Actor3D::~Actor3D()
	{		
	}
#pragma endregion
}