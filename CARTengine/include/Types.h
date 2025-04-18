#pragma once
#include <string>
#include <vector>
#include <raylib.h>
#include "easing.h"
    
namespace cart {

      

#pragma region  ENUMNS
   

    enum UITYPE {
        BUTTON,
        TEXT,
        LABEL
    };

    enum ALIGN {
        LEFT,
        CENTER,
        RIGHT
    };

    enum PARTICLE_SCALING_MODE {
        LOCAL,
        GLOBAL
    };

    enum PARTILCE_EMITTER_SHAPE {
        P_CONE,
        P_CIRCLE,
        P_BOX,
        P_LINE
    };
    enum SHAPE_TYPE {
        NONE,
        CIRCLE,
        RECTANGLE,
        LINE,
        CIRCLE_LINE,
    };

    enum APP_STATE {
        TITLE,
        LEVEL_SELECTION,
        GAME
    };

    enum GAME_STATE {
        STAGE1,
        STAGE2,

    };

#pragma endregion

#pragma region STRUCTS
    struct UI_Properties {
    public:
        Vector2 size;
        Vector2 location;
        Vector2 pivot;
        Color color;
        float scale{ 1.f };
        float rotation;
        std::string texture;
    };

	struct Text_Properties : UI_Properties{
    public:
		std::string font;
		std::string text;
		float fontsize;
		ALIGN align = ALIGN::LEFT;	
        Color textbackground;
	};

    struct Btn_Properties :UI_Properties {
    public:
        Color defaultcol;
        Color overcol;
        Color downcol;
        Color textcolor;
        ALIGN align = ALIGN::LEFT;
        bool isSelectable;
    };
    struct Btn_Text_Properties : Btn_Properties {
    public:
        std::string font;
        std::string text;        
        float fontsize;
        ALIGN align = ALIGN::LEFT;
        Color color;
        Color hovercolor;

    };
   

    struct TransformAnimData {
        Vector2 offset;
        float scale;
        float rotate;
        Color color = WHITE;
        bool offsetActive = true;
        bool rotateActive = true;
        bool scaleActive = true;
        bool colorActive = true;
    };

   struct Particle_Properties {
        std::string id;
        float lifetime;
        std::string texturepath;
        float angluarvelocity;
        float speed;
        float startscale;
        float endscale;
        float startrotation;
        float endrotation;
        Vector2 size;
        Vector2 startlocation;
        Vector2 velocity;
        Color color;
        Color startcolor;
        Color endcolor;
        easing_functions easing;
        float gravity = 0;
        int burstcount;
        bool colorovertime;
        bool scaleovertime;
        bool burstonstart;
    };

    struct Particle_System_Propterties {
        std::vector<std::string> texturepath;
        int maxParticles;
        float delay;
        Particle_Properties particleprop;
        PARTILCE_EMITTER_SHAPE shape;     
        Rectangle locationoffset;
        Vector2 minsize;
        Vector2 maxsize;
        float startscale;
        float endscale;
        Vector2 minvelocity;
        Vector2 maxvelocity;
        float minlifetime;
        float maxlifetime;
        Color color;
        Color startcolor;
        Color endcolor;
        float maxangularvelocity;
        float minangularvelocity;
        easing_functions easing;
        float gravity = 0;
        std::vector<Color> colorspool;
        int burstcount;
        bool randomizeSpeed;
        bool randomizeSize;
        bool randomizeColor;
        bool looping;
        bool burstmode;
        bool colorovertime;
        bool scaleovertime;
        bool burstonstart;

    };

    struct PARTICLE_EMITTER_BOX {
        Rectangle rect;        
    };

    struct PARTICLE_EMITTER_CIRCLE {
        float radius;
        float arc;
        float thickness;
    };

    struct PARTICLE_EMITTER_LINE {
        float length;
        float angle;
    };

    struct DebugData {
        Vector2 start;
        Vector2 end;
        PARTILCE_EMITTER_SHAPE shape;
        float radius;
        float linewidth;
        Color color;

    };

    struct ButtonRect {
        float scale;
        Vector2 loc;
        int width;
        int height;
        std::string texture;
        Color texturecolor;
        std::string text;
        Color textcolor;
        Vector2 textsize;
        float fontsize;
    };
#pragma endregion




}
