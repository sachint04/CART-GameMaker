#include "component/UIList.h"
#include "raylib.h"
#include "Application.h"
#include "World.h"
#include "UIButton.h"
#include "ImageButton.h"
#include "AssetManager.h"
namespace cart {

#pragma region  Constructor  & Initialization

	UIList::UIList(World* _owningworld, const std::string& _id, const std::vector<ListItemData>& data)
		:UIElement{ _owningworld, _id }, m_listdata{ data }, m_selectedindex{ -1 }, m_listitem{}
	{
	}

	void UIList::Init()
	{
		UIElement::Init();
		uint16_t scrW = GetScreenWidth();
		uint16_t scrH = GetScreenHeight();
		std::string resourcepath = m_owningworld->GetApplication()->GetAssetsPath();
		std::string staticassetpath = m_owningworld->GetApplication()->GetStaticAssetsPath();
		m_listitem.clear();
		int counter = 0;
		auto& c = m_owningworld->GetApplication()->GetGameConfig()["cart"];
		std::string fntid = c["systemfont"].GetString(0);
		int fontsize = c["systemfont"].GetInt(1);
		std::string strfont = c["fonts"][fntid].GetString(0);
		if (!fnt)
		{
			fnt = AssetManager::Get().LoadFontAsset(strfont, fontsize);
		}
													
		// Mask to items
		maskimg = GenImageColor(scrW, scrH, { 0 });
		ImageFormat(&maskimg, 7);
		for (size_t x = 0; x < scrW; x++)
		{
			for (size_t y = 0; y < scrH; y++)
			{
				if (x < m_location.x || x >m_location.x + m_width ||
					y < m_location.y || y >m_location.y + m_height)
				{
					ImageDrawPixel(&maskimg, x, y, { 0 });
				}
				else {
					ImageDrawPixel(&maskimg, x, y, { 255 });
				}
			}
		}
		Vector2 margin{ 10.f,10.f };
		Vector2 btnpadding{ 4.f, 16.f };
		for (auto iter = m_listdata.begin();iter != m_listdata.end(); ++iter)
		{
			counter++;

			weak<ImageButton> item = m_owningworld->SpawnActor<ImageButton>(iter->id);
			weak<UIElement> icon = m_owningworld->SpawnActor<UIElement>(iter->id + std::string{"icon"});
			// Set Item properties
			Vector2 tsize = MeasureTextEx(*fnt, iter->label.c_str(), fontsize, 2.f);
			Btn_Text_Properties btnprop = {};
			btnprop.text = iter->label;
			btnprop.font = std::string{ staticassetpath + strfont };
			btnprop.fontsize = fontsize;
			btnprop.textcolor = BLACK;
			btnprop.texthoverolor = ORANGE;
			btnprop.color = WHITE;

			btnprop.btncol = ORANGE;
			btnprop.overcol = WHITE;
			btnprop.downcol = WHITE;
			btnprop.align = RIGHT;
			btnprop.fontspace = 2.f;
			btnprop.borderwidth = 1;
			btnprop.bordercol = BLACK;
			btnprop.size = { m_width - margin.x , tsize.y + btnpadding.y };
			btnprop.location =  { m_location.x + margin.x / 2.f, m_location.y + (margin.y / 2.f) + (tsize.y * (counter - 1)) + (btnpadding.y * (counter - 1)) };
			item.lock()->SetButtonProperties(btnprop);			
			item.lock()-> Init();
			item.lock()-> SetVisible(true);
			
		

			item.lock()->SetScreenMask(maskimg);
			AddChild(item);


			// Set Icon properties
			UI_Properties iconprop = {};
			iconprop.texturetype = TEXTURE_PART;
			iconprop.texture = iter->stricon;
			iconprop.texturesource = iter->iconsource;			
			iconprop.size = { iter->iconsource.width, iter->iconsource.y };
			iconprop.location =  { m_location.x + margin.x , 
				m_location.y + (margin.y / 2.f) + ((tsize.y + btnpadding.y) * (counter - 1)) + ((tsize.y + btnpadding.y) - iter->iconsource.y )/2.f};
			icon.lock()->SetUIProperties(iconprop);
			icon.lock()->Init();
			icon.lock()->SetVisible(true);
			item.lock()->AddChild(icon);
			item.lock()->onButtonUp.BindAction(GetWeakRef(), &UIList::SelectHandler);
		}
	}

#pragma endregion

#pragma region  LOOP

	void UIList::Update(float _deltaTime)
	{
		UIElement::Update(_deltaTime);
	}

	void UIList::Draw(float _deltaTime)
	{
		UIElement::Draw(_deltaTime);
		
	}


#pragma endregion
#pragma region Helpers
	

	void UIList::Open()
	{
		onOpen.Broadcast();
	}

	void UIList::Close()
	{
		onClose.Broadcast();
	}

	int UIList::GetSelectedIndex()
	{
		return m_selectedindex;
	}

#pragma endregion

#pragma region Event Handler

	void UIList::SelectHandler(weak<Object> btn, Vector2 pos)
	{
		for (auto iter = m_listdata.begin(); iter != m_listdata.end(); ++iter)
		{
			if (iter->id.compare(btn.lock()->GetId()) == 0) {

				onSelect.Broadcast(iter->index);
				break;
			}
		}
	}
#pragma endregion



#pragma region  CleanUp

	UIList::~UIList()
	{
		UnloadImage(maskimg);
		m_listdata.clear();
		m_listitem.clear();
		fnt.reset();
	}

	void UIList::Destroy()
	{
		if (m_isPendingDestroy)return;
		UIElement::Destroy();
	}
#pragma endregion


	
}