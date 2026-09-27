#include "Actor3D.h"
#include "World.h"
#include "AssetManager.h"
#include "MathUtility.h"
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
		m_bShowLabel{false},
		m_bPlayAnim{false},
		m_animations{nullptr},
		m_bPlayAnimReverse{false},
		m_loopAnim{false},
		m_bInstanced{false},
		m_meshBoundingBox{},
		m_scale3{1.f,1.f ,1.f }
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

	}
	void Actor3D::Update3D(float _deltaTime)
	{
		if (!m_visible)return;
		bool animfinished = false;
		if (m_bPlayAnim)		
		{
			ModelAnimation anim = m_animations[m_currentAnimation];
			if (m_currentFrame < 0 || m_currentFrame > anim.frameCount - 1)// Loo
			{				
				m_currentFrame = m_bPlayAnimReverse ? 0 :anim.frameCount - 1;
				animfinished = true;// animation playing forward hence mark finished
			}
			else {
				UpdateModelAnimation(m_model, anim, m_currentFrame);				
				m_currentFrame = !m_bPlayAnimReverse ? m_currentFrame + 1 : m_currentFrame - 1;				
			}

			if (animfinished) {

				// Animation finished
				if (!m_loopAnim) // Not loopiung
				{
					m_bPlayAnim = false;
					onAnimFinish.Broadcast(GetWeakRef(), m_currentAnimation);
					// Callback
					auto find = std::find_if(m_AnimCallbacks.begin(), m_AnimCallbacks.end(), [&](const auto& p)
					{
						return p.first == m_currentAnimation;
					});
					if (find != m_AnimCallbacks.end()) {

						find->second(m_currentAnimation, GetWeakRef());
						find->first = -1;
						//m_AnimCallbacks.erase(find);//TBD : Delete completed callbacks on cleanup
					}
				}
				else {
					// Looping Play again
					PlayAnimation(m_currentAnimation, m_bPlayAnimReverse, m_loopAnim);
				}
			}
		}
		
		

		
		//{
		//	{m_location3.x - m_width / 2, m_location3.y - m_height / 2, m_location3.z - m_zSize / 2},
		//	{m_location3.x + m_width / 2, m_location3.y + m_height / 2, m_location3.z + m_zSize / 2}
		//});
	
		if (!m_owningworld->GetHUD().lock()->IsMouseOverUI(GetMousePosition()) && m_active) {
			
			if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
			{
				m_collision = GetRaycastHit(GetMousePosition(), m_model);				
				if (m_collision.hit )
				{
					onTouch.Broadcast(GetWeakRef(), GetMousePosition());
					//m_currentFrame = 0;
				}
			}
			if ( !m_isHover) {
				m_collision = GetRaycastHit(GetMousePosition(), m_model);
				if (m_collision.hit) {
					onHover.Broadcast(GetWeakRef(), GetMousePosition());					
					m_isHover = true;
				}
			}
			else {
				m_collision = GetRaycastHit(GetMousePosition(), m_model);
				if (m_isHover && !m_collision.hit) {
					onOut.Broadcast(GetWeakRef(), GetMousePosition());
					m_isHover = false;
				}
			}
		}
		
	}
	void Actor3D::Draw(float _deltaTime)
	{
			
	}
	void Actor3D::Draw3D(float _deltaTime)
	{
		if (!m_visible || m_isPendingDestroy)return;
		Shader shader = m_model.materials[0].shader;
		m_collision = GetRaycastHit(GetMousePosition(), m_model);
		
	//	DrawBoundingBox(GetMeshBoundingBox(m_model.meshes[0]), RED);

		DrawModelEx(m_model, m_location3, { m_rotation3.x, m_rotation3.y, m_rotation3.z }, m_rotation3.w, m_scale3, WHITE);
		
	}

#pragma endregion
	
#pragma region  Helper


	void Actor3D::SetModel(Model& model)
	{
		m_model = model;
		m_meshBoundingBox = GetMeshBoundingBox(m_model.meshes[0]);
	}
	void Actor3D::SetScale3(Vector3 _scale)
	{
		m_scale3 = _scale;
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
	}
	void Actor3D::SetInstanced(bool flag)
	{
		m_bInstanced = true;
	}

	void Actor3D::PlayAnimation(int index, bool reverse, bool loop, AnimationCallback callback)
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
		if (callback) {
			m_AnimCallbacks.push_back({ m_currentAnimation, callback });
		}
	}
	bool Actor3D::UpdateTexture(const std::string& path, int matIndex) {
		Logger::Get()->Trace(std::format("Actor3D::UpdateTexture() path {} ", path));
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

	RayCollision Actor3D::GetRaycastHit(Vector2 _pos, Model & model)
	{
		Ray ray = GetScreenToWorldRay(_pos, Application::CAMERA);
		BoundingBox  localbox = GetMeshBoundingBox(model.meshes[0]);

		BoundingBox worldBox;
		worldBox.min = Vector3Add(localbox.min, m_location3);
		worldBox.max = Vector3Add(localbox.max, m_location3);

		//m_collision = GetRayCollisionBox(m_ray, worldBox);
		return GetRayCollisionBox(ray, worldBox);
	}

#pragma endregion

#pragma region CleanUp
	void Actor3D::Destroy()
	{
		if (IsPendingDestroy())return;

		if (m_animations && IsModelAnimationValid(m_model, *m_animations)) {

			UnloadModelAnimations(m_animations, m_animcount);
		}
		if (IsModelValid(m_model))
		{
			UnloadModel(m_model);
		}
		m_AnimCallbacks.clear();

		onHover.Destroy();
		onOut.Destroy();
		onTouch.Destroy();
		onAnimFinish.Destroy();

		SetVisible(false);
		Actor::Destroy();
	}
	
	Actor3D::~Actor3D()
	{		
	}
#pragma endregion
}