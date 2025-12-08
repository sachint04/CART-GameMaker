#pragma once
#include "Actor.h"
#include "Delegate.h"
namespace cart
{
	class World;
	class Actor3D : public Actor{

	public:
		Actor3D(World* _owningworld, const std::string& _id);
		~Actor3D();
		void Init()override;
		void Start()override;
		void Update(float _deltaTime)override;
		void Draw(float _deltaTime)override;
		void Update3D(float _deltaTime, const Camera& camera);
		void Draw3D(float _deltaTime, const Camera& camera);
		void SetModel(Model model);
		Model& GetModel(){ return m_model; };
		void SetAnimations(ModelAnimation * animations, const int& count);
		void PlayHoverAnim();
		void PlayDefaultAnim();
		void SetLabel(const std::string& _label, const std::string& font, float size, float spacing);
		void ShowLabel(bool _flag);
		void PlayAnimation(int index);
		bool UpdateTexture(const std::string& path, int matIndex);
		void Destroy()override;
		

		std::string& GetLabel(){ return m_label; };
		Delegate<weak<Object>, Vector2> onHover;
		Delegate<weak<Object>, Vector2> onOut;
		Delegate<weak<Object>, Vector2> onTouch;
		Delegate<weak<Object>, int> onAnimFinish;
	private:
		Model m_model;
		
		Ray m_ray;
		RayCollision m_collision;
		ModelAnimation * m_animations;
		std::string m_label;
		bool m_bShowLabel;
		bool m_isTouch;
		bool m_isHover;
		bool m_bPlayAnim;
		int m_animcount;
		int m_currentFrame;
		int m_currentAnimation;
		float m_fontSize;
		float m_fontSpace;
		std::string m_strfont;
	};

}