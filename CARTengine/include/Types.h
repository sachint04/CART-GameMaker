#pragma once
#include <raylib.h>
#include <string>
#include <functional>
#include <vector>
#include "easing.h"
#include "Core.h"
namespace cart {



#pragma region  ENUMNS
    enum Layout_Component_Type { NO_LAYOUT, LAYOUT, V_LAYOUT, H_LAYOUT };


    enum STYLE_POSITION {
        STYLE_POSITION_RELATIVE,
        STYLE_POSITION_ABSOLUTE,
        STYLE_POSITION_FIXED,
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
        RIGHT,
        JUSTIFIED
    };
    enum V_ALIGN {
        TOP,
        MIDDLE,
        BOTTOM
    };


    enum PARTICLE_SCALING_MODE {
        LOCAL,
        GLOBAL
    };

    enum PARTILCE_EMITTER_SHAPE {
        P_CONE,
        P_CIRCLE,
        P_BOX,
        P_LINE,
        P_CIRCLE_OUTLINE,
        P_PLANE2D
    };
    enum SHAPE_TYPE {
        NONE,
        CIRCLE,
        RECTANGLE,
        LINE,
        CIRCLE_LINE,
        ROUNDED_RECTANGLE
    };
    enum FILL_TYPE {
        NO_FILL,
        SOLID_FILL,
        LINEAR_GRADIENT_FILL_V,
        LINEAR_GRADIENT_FILL_H,
        RADIAL_GRADIENT_FILL
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

    enum KeyboardStatus
    {
        Visible = 0,
        Done = 1
    };

    enum KeyboardType
    {
        Default = 0,
        NumbersAndPunctuation = 2,
        URL = 3,
        NumberPad = 4,
        PhonePad = 5,
        EmailAddress = 7
    };

    enum Popup_Button_Type {
        Alert_Ok,
        Alert_Cancel,
        Modal_Close
    };

    enum Asset_Type {
        ASSET_IMAGE,
        ASSET_MODEL,
        ASSET_TEXT
    };

#pragma endregion

#pragma region STRUCTS
    struct UI_Style {
        STYLE_POSITION style_Position = STYLE_POSITION_ABSOLUTE;


    };
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


    typedef  struct {
        std::string url;
        std::string virtualpath;
        Asset_Type type;
        TEXTURE_DATA_STATUS texture_status;
    }External_Asset;

    struct Preload_Data {
    public:
        std::string uid;
        int count;
        std::function<bool()> callback;
        std::vector<External_Asset> list;
        std::string loadmessage;
    };

    struct UI_Layout_Properties
    {
    public:
        ALIGN align = LEFT;
        V_ALIGN valign = TOP;
        float margin = 0.f;
        float padding = 0.f;
        float gap = 0.f;
    };

    struct UI_Properties {
    public:
        TEXTURE_DATA_STATUS texturestatus = TEXTURE_DATA_STATUS::UNLOCKED;
        TEXTURE_TYPE texturetype = TEXTURE_TYPE::TEXTURE_FULL;
        Layout_Component_Type component;
        SHAPE_TYPE shapetype = SHAPE_TYPE::RECTANGLE;
        FILL_TYPE filltype = SOLID_FILL;
        bool blockscale;
        int linewidth = 0;
        int roundnessSegments = 36;
        int borderwidth;
        float scale{ 1.f };
        float rotation;
        float roundness = 0.5f;
        std::string texture;
        Color color = WHITE;
        Color textureColor = WHITE;
        Vector2 size;
        Vector2 defaultSize = { -1, -1 };// Required incase of "Layout Component" with anchor x <> anchor width or anchor  y <> anchor height
        Vector2 location;
        Vector2 pivot;
        Vector2 padding = { 0,0 };
        Rectangle texturesource;
        Rectangle anchor = { 0,0,0,0 };
        Color bordercol;
        UI_Layout_Properties layout_props;
        std::vector<Color> gradientcols;
    };

    struct Text_Properties : UI_Properties {
    public:
        bool multiline = false;
        ALIGN align = ALIGN::LEFT;
        V_ALIGN valign = V_ALIGN::MIDDLE;
        float fontsize;
        float minfontsize = 0;
        float maxfontsize = 0;
        float fontspacing = 2.f;
        float minfontspacing = 1;
        float linespace = 1.f;
        std::string font;
        std::string text;
        Color textbackground;
        Color textcolor;

    };

    struct Texture_Source_Rect {
    public:
        Rectangle defaultrect;
        Rectangle hoverrect;
        Rectangle downrect;
        Rectangle disabledrect;
    };
    struct Btn_Properties :UI_Properties {
    public:
        ALIGN align = ALIGN::LEFT;
        bool selectable;
        bool dragable;
        bool maintainoffset = true;
        int cursorstyle = 4;
        Color btncol;
        Color overcol;
        Color downcol;
        Color disablecol = DARKGRAY;
        Rectangle texturesourceover;
        Rectangle texturesourcedown;
        Rectangle texturesourcedisable;
        Rectangle dragableboundry;
        // only for ImageToggleButton
        Texture_Source_Rect primarytogglerect;
        Texture_Source_Rect secondarytogglerect;
        std::string primarytexture;
        std::string secondarytexture;
        // End
    };

    struct Btn_Text_Properties : Btn_Properties {
    public:
        ALIGN textAlign = ALIGN::LEFT;
        std::string font;
        std::string text;
        Color textcolor = BLACK;
        Color texthoverolor = BLACK;
        float fontsize;
        float minfontsize;
        float fontspacing = 2.f;
        float minfontspace = 1.f;

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
        bool colorovertime;
        bool scaleovertime;
        bool burstonstart;
        int blendmode = BLEND_ADDITIVE;
        std::string id;
        float lifetime;
        std::string texturepath;
        int burstcount;
        float speed;
        float acceleration;
        float startscale;
        float endscale;
        float startrotation;
        float endrotation;
        float gravity = 0;
        Vector2 size;
        Vector2 startlocation;
        Vector2 velocity;
        Vector3 angluarvelocity;
        Color color;
        Color startcolor;
        Color endcolor;
        easing_functions easing;
        PARTILCE_EMITTER_SHAPE emittershape;
        Vector2 emitterlocation;
        Rectangle texturerect;
    };

    struct Particle_System_Propterties {
        PARTILCE_EMITTER_SHAPE shape;
        bool randomizeSize;
        bool randomizeColor;
        bool looping;
        bool burstmode;
        bool colorovertime;
        bool scaleovertime;
        bool burstonstart;
        bool playonstart = true;
        bool usecolorpool;
        bool randomizeSpeed;
        int maxParticles;
        int burstcount;
        int blendmode = BLEND_ADDITIVE;
        float delay;
        float size = 1.f;
        float startscale;
        float endscale;
        float speed;
        float acceleration;
        float minspeed;
        float maxspeed;
        float minlifetime;
        float maxlifetime;
        float maxrotation;
        float minrotation;
        Vector3 angularvelocity;
        float gravity = 0;
        float emittersize = 1.f;
        Rectangle locationoffset;
        Vector2 minsize;
        Vector2 maxsize;
        Color color;
        Color startcolor;
        Color endcolor;
        easing_functions easing;
        std::vector<Color> colorspoolstart;
        std::vector<Color> colorspoolend;
        std::vector<std::string> texturepath;
        bool istexturesprite;
        std::vector<Rectangle> texturecells;
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

    struct Vec2_short { short int x; short int y; };

    struct UI_ANCHOR { float MinX = 0.f; float MaxX = 1.f; float MinY = 0.f; float MaxY = 1.f; };



#pragma endregion

#pragma region HUD themes

    struct AlertTheme {
        Color background = { 0, 165, 173,255};
        Color button = { 244,244,244, 255 };// pestal green
        Color hover = { 0, 165, 173, 255 }; // blue green
        Color disable = { 247, 247, 242,255 };// sea green
        Color down = { 244,244,244,255 }; // pink
        Color border = { 0,0,0,255 }; // blue green
        Color textcol = { 0,0,0,255 };
        Color blocker = { 0,0,0,0 }; // bg blocker 
    };
   
    



}
