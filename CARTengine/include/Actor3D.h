#pragma once
#include <functional>
#include "Actor.h"
#include "Delegate.h"
namespace cart
{
	class World;
	class Actor3D : public Actor{

	public:
		using AnimationCallback = std::function<void(int animIndex, weak<Object> obj)>;

		Actor3D(World* _owningworld, const std::string& _id);
		~Actor3D();
		void Init()override;
		void Start()override;
		void Update(float _deltaTime)override;
		void Draw(float _deltaTime)override;
		void Update3D(float _deltaTime);
		void Draw3D(float _deltaTime);
		void SetModel(Model& model);
		void SetScale3(Vector3 _scale)override;
		Model& GetModel(){ return m_model; };
		void SetAnimations(ModelAnimation * animations, const int& count);
		void PlayHoverAnim();
		void PlayDefaultAnim();
		void SetInstanced(bool flag);
		bool IsInstanced() { return m_bInstanced; };		
		void PlayAnimation(int index, bool reverse = false, bool loop = false, AnimationCallback callback = nullptr);
		bool UpdateTexture(const std::string& path, int matIndex);
		bool IsAnimPlaying();
		void Destroy()override;
		RayCollision GetRaycastHit(Vector2 _pos, Model& model);

		Delegate<weak<Object>, Vector2> onHover;
		Delegate<weak<Object>, Vector2> onOut;
		Delegate<weak<Object>, Vector2> onTouch;
		Delegate<weak<Object>, int> onAnimFinish;


	private:
		bool m_bShowLabel;
		bool m_isTouch;
		bool m_isHover;
		bool m_bPlayAnim;
		bool m_bPlayAnimReverse;
		bool m_loopAnim;
		bool m_bInstanced;
		int m_animcount;
		int m_currentFrame;
		int m_currentAnimation;
		BoundingBox m_meshBoundingBox;
		Vector3 m_scale3;
		Model m_model;		
		Ray m_ray;
		RayCollision m_collision;
		ModelAnimation * m_animations;
		std::vector<std::pair<int, AnimationCallback>> m_AnimCallbacks;

	};

}