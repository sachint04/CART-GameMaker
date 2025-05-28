#pragma once
#include "UIElement.h"
#include "Core.h"
#include "Delegate.h"
namespace cart
{
	struct ListItemData {
		std::string id;
		int index;
		std::string label;
		std::string stricon;
		std::string extra;
		Rectangle iconsource;
	};

	class World;
	class UIList :public UIElement
	{
	public:
		UIList(World* _owningworld, const std::string& _id, const std::vector<ListItemData>& data);
		void Init()override;
		void Update(float _deltaTime)override;
		void Draw(float _deltaTime)override;

		void Open();
		void Close();
		int GetSelectedIndex();
		void SelectHandler(weak<Object> btn, Vector2 pos);
		~UIList();
		void Destroy()override;

		Delegate<int> onSelect;
		Delegate<>onOpen;
		Delegate<>onClose;
	private:
		std::vector<ListItemData> m_listdata;
		int m_selectedindex;
		std::vector<weak<UIButton>> m_listitem;
		shared<Font> fnt;
		Image maskimg;
	};

}