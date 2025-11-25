#pragma once
#include <string>
#include <functional>
#include <vector>
#include <raylib.h>
#include "easing.h"
#include "Core.h"
namespace cart {

      

#pragma region  ENUMNS
    enum COMPONENT_TYPE {
        LAYOUT_COMPONENT
   };
    enum VEC_TYPE {
        Vec2D,
        Vec3D,
        Vec4D
    };

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
        ROUNDED_RECTANGLE
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

    enum TEXTURE_TYPE {
        TEXTURE_FULL,
        TEXTURE_PART,
    };

    enum TEXTURE_DATA_STATUS {
        UNLOCKED,
        LOCKED
    };
    enum ASYNC_CALLBACK_STATUS {
        OK,
        PROGRESS,
        FAILED
    };
    enum PROGRESS_VIEW_TYPE {
        P_BAR,
        P_DISC,
        P_CLOCK
    };

    enum LOG_TYPE {
        LOG_INFO,
        LOG_WARNING,
        LOG_ERROR,
    };

    enum CANVAS_SCALE_MODE {
        CONSTANT_PIXEL, // no change in define window size
        MATCH_HEIGHT,   // maintain aspect raio according to height
        MATCH_WIDTH,    // maintain aspect raio according to width
        SCREEN_SIZE     // no aspect ratio - match screen/display size
    };
#pragma endregion

#pragma region STRUCTS

    typedef struct Async_Call_Header
    {
        std::string id;
        std::string location;
        TEXTURE_DATA_STATUS texture_status;

    }Async_Call_Header;

    typedef struct Async_Call_Response 
    {
        Async_Call_Header request;
        ASYNC_CALLBACK_STATUS callbackstatus;
        const char* data;
        int progress;
        int totalBytes;
    }Async_Call_Response;
       
    typedef struct Async_Call_Data {
        std::string id;
        std::string location;
        TEXTURE_DATA_STATUS texture_status;
        std::function<bool(Async_Call_Response)> callback;

    }Async_Call_Data;

    typedef struct {
        shared<Texture2D> texture;
        TEXTURE_DATA_STATUS status;
    }TextureData;

    struct Preload_Data {
    public:
        std::string uid;
        int count;
        std::function<bool()> callback;
        std::vector<std::string> list;
        std::string loadmessage;
    };

    struct UI_Properties {
    public:
        TEXTURE_DATA_STATUS texturestatus = TEXTURE_DATA_STATUS::UNLOCKED;
        TEXTURE_TYPE texturetype = TEXTURE_TYPE::TEXTURE_FULL;
        SHAPE_TYPE shapetype = SHAPE_TYPE::RECTANGLE;
        int linewidth = 0;
        std::string texture;
        float scale{ 1.f };
        float rotation;
        Color color = WHITE;
        Color textureColor =  WHITE ;
        Vector2 size;
        Vector2 location;
        Vector2 pivot;
        Rectangle texturesource;
        Rectangle anchor = {0,0,1.f,1.f};

    };

	struct Text_Properties : UI_Properties{
    public:
		ALIGN align = ALIGN::LEFT;	
		std::string font;
		std::string text;
        Color textbackground;
        Color textcolor;
		float fontsize;
        float fontspacing = 2.f;
	};

    struct Btn_Properties :UI_Properties {
    public:
        ALIGN align = ALIGN::LEFT;
        bool isSelectable;
        int borderwidth = 0;
        Color btncol;
        Color overcol;
        Color downcol;
        Color disablecol;
        Color bordercol = GRAY;
        Rectangle texturesourceover;
        Rectangle texturesourcedown;
        Rectangle texturesourcedisable;
       
    };
    
    struct Btn_Text_Properties : Btn_Properties {
    public:
        ALIGN textAlign = ALIGN::LEFT;
        std::string font;
        std::string text;        
        Color textcolor = BLACK;
        Color texthoverolor = BLACK;
        float fontsize;
        float fontspace = 2.f;

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

    struct ProgressVeiw_Properties : UI_Properties {
    public:
        Rectangle barRect;
        Color barColor;
        Color barFill;
        Color msgCol;
    };

    typedef struct Log_Data {
        LOG_TYPE type;
        std::string log;
    }LogData;
   
    struct Vec2_short {short int x; short int y;};

    struct UI_ANCHOR {float MinX = 0.f;float MaxX = 1.f;float MinY = 0.f;float MaxY = 1.f;} ;
#pragma endregion




}
