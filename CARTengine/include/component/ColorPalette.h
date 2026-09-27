#pragma once
#include "UIButton.h"
#include <raylib.h>
#include "Delegate.h"
namespace cart
{


	class World;
	class ColorPalette : public UIButton
	{

	public:
		ColorPalette(World* _owningworld, const std::string& _id, bool isExcludedFromParentAutoControl, float palatteiconsize);
		void Init()override;
		void Start()override;
		void Update(float _deltaTime)override;
		void Draw(float _deltaTime)override;
		void SetVisible(bool _flag)override;
		void ButtonUp(Vector2 pos)override;
		//Colors
		Color PastelCoral = { 255, 100, 100, 255 };
		Color SweetSalmon = { 255, 113, 100, 255 };
		Color SoftPeach = { 255, 126, 100, 255 };
		Color LightApricot = { 255, 140, 100, 255 };
		Color CantaloupeCream = { 255, 153, 100, 255 };
		Color MelonBlush = { 255, 166, 100, 255 };
		Color Creamsicle = { 255, 179, 102, 255 };

		Color WarmButter = { 255, 191, 106, 255 };
		Color MutedHoney = { 255, 204, 110, 255 };
		Color PaleSunflower = { 255, 216, 114, 255 };
		Color SoftLemon = { 255, 228, 118, 255 };
		Color LightMimosa = { 243, 234, 120, 255 };
		Color PaleChartreuse = { 219, 233, 122, 255 };
		Color CeladonZing = { 195, 232, 124, 255 };

		Color KeyLimeCream = { 171, 231, 126, 255 };
		Color SoftPistachio = { 147, 230, 128, 255 };
		Color LightSage = { 129, 230, 134 , 255 };
		Color PaleMint = { 127, 230, 153 , 255 };
		Color SweetSpearmint = { 125, 230, 172 , 255 };
		Color SeafoamGlow = { 123, 230, 191, 255 };
		Color LightTurquoise = { 121, 230, 210, 255 };

		Color SoftCyan = { 120, 230, 230 , 255 };
		Color PaleIce = { 120, 216, 234, 255 };
		Color BabyBlue = { 120, 203, 239 , 255 };
		Color SkyMist = { 120, 190, 244 , 255 };
		Color SoftCornflower = { 120, 176, 249 , 255 };
		Color PeriwinkleTint = { 120, 163, 253 , 255 };
		Color LightIndigo = { 128, 155, 255 , 255 };

		Color SoftLavender = { 140, 150, 255, 255 };
		Color WisteriaMist = { 151, 144, 255 , 255 };
		Color PaleAmethyst = { 162, 138, 255, 255 };
		Color LightOrchid = { 174, 132, 255, 255 };
		Color SoftViolet = { 187, 130, 251, 255 };
		Color ThistlePetal = { 201, 130, 245 , 255 };
		Color MutedMagenta = { 215, 130, 238, 255 };

		Color HeirloomRose = { 230, 130, 231 , 255 };
		Color BubblegumCream = { 244, 130, 225 , 255 };
		Color SoftCarnation = { 255, 128, 214 , 255 };
		Color BlushPink = { 255, 122, 191 , 255 };
		Color CottonCandy = { 255, 117, 168, 255 };
		Color SweetStrawberry = { 255, 111, 145 , 255 };
		Color FlamingoPastel = { 255, 105, 122, 255 };

		Color AlabasterWhite = { 245, 245, 245, 255 };
		Color PlatinumMist = { 220, 220, 220 , 255 };
		Color SilverSatin = { 190, 190, 190 , 255 };
		Color LightSlate = { 160, 160, 160, 255 };
		Color MutedPewter = { 130, 130, 130, 255 };
		Color SteelShadow = { 95, 95, 95 , 255 };
		Color DeepCharcoal = { 60, 60, 60, 255 };
		//Colors
		int cols = 7;
		int rows = 7;
		int colCount = 49;
		Color colors[49] = {
			PastelCoral, SweetSalmon, SoftPeach,  LightApricot, CantaloupeCream, MelonBlush, Creamsicle,

			WarmButter, MutedHoney, PaleSunflower, SoftLemon, LightMimosa, PaleChartreuse, CeladonZing,

			KeyLimeCream, SoftPistachio, LightSage, PaleMint, SweetSpearmint, SeafoamGlow, LightTurquoise,

			SoftCyan, PaleIce, BabyBlue, SkyMist, SoftCornflower, PeriwinkleTint, LightIndigo,

			SoftLavender, WisteriaMist, PaleAmethyst, LightOrchid, SoftViolet, ThistlePetal, MutedMagenta,

			HeirloomRose, BubblegumCream, SoftCarnation, BlushPink, CottonCandy, SweetStrawberry, FlamingoPastel,

			AlabasterWhite,  PlatinumMist,SilverSatin, LightSlate, MutedPewter, SteelShadow, DeepCharcoal
		};

		

		void EvaluateUI();
		~ColorPalette();
		Delegate<Color> onPicked;
	private:
		float m_palatteiconsize;
		std::vector<int>colorState;
		std::vector<Rectangle> colorsRecs;
		Vector2 mousePoint = { 0.0f, 0.0f };

	};
}