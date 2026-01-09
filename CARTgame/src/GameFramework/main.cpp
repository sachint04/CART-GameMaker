#include "Application.h"
#include "CARTjson.h"




#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

enum SCREEN_SCALE_MODE {
    MATCH_HEIGHT = 0,
    MATCH_WIDTH,
    MATCH_SCREEN
};
int DEFAULT_CANVAS_WIDTH = 800;
int DEFAULT_CANVAS_HEIGHT = 600;
int SCREEN_WIDTH = 800;
int SCREEN_HEIGHT = 600;

int CANVAS_WIDTH;
int CANVAS_HEIGHT;
int CANVAS_X = 0;
int CANVAS_Y = 0;
float CANVAS_SCALE_X = 1.f;
float CANVAS_SCALE_Y = 1.f;
float CANVAS_STRECH_X = 1;
float CANVAS_STRECH_Y = 1;
SCREEN_SCALE_MODE SCALAR_MODE = MATCH_HEIGHT;
float MAX_STREACH = 2.f;

namespace cart
{
    class Application;

}
extern cart::Application* GetApplication();

void AppResize(int w, int h)
{
    SCREEN_WIDTH = w;
    SCREEN_HEIGHT = h;
    float r1 = 1, r2 = 1, d = 0, s = 0;
    switch (SCALAR_MODE) {
    case SCREEN_SCALE_MODE::MATCH_HEIGHT:
        r1 = (float)DEFAULT_CANVAS_WIDTH / (float)DEFAULT_CANVAS_HEIGHT;
        r2 = (float)w / (float)h;
        CANVAS_WIDTH = (float)SCREEN_HEIGHT * r2;

        CANVAS_SCALE_X = (float)SCREEN_WIDTH / (float)DEFAULT_CANVAS_WIDTH;
        CANVAS_SCALE_Y = 1.f;
        CANVAS_HEIGHT = h;
        CANVAS_X = ((float)SCREEN_WIDTH - (float)CANVAS_WIDTH) / 2;
        CANVAS_Y = 0;
        break;
    case SCREEN_SCALE_MODE::MATCH_WIDTH:

        break;
    case SCREEN_SCALE_MODE::MATCH_SCREEN:
        SCREEN_HEIGHT = h;
        break;

    };
}

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __EMSCRIPTEN__
    EMSCRIPTEN_KEEPALIVE
#endif
        int GetHTTPCallback(char* uid, char* response, char* data)
    {
        std::cout << "From Application | HTTP callback  call back | id " << uid << " | response " << response << " | data " << data << std::endl;
        GetApplication()->SetHTTPCallback(uid, response, data);
        return 1;
    }

#ifdef __EMSCRIPTEN__
    EMSCRIPTEN_KEEPALIVE
#endif
        int PostHTTPCallback(char* uid, char* response, char* data)
    {
        GetApplication()->SetHTTPCallback(uid, response, data);
        return 1;
    }
#ifdef __EMSCRIPTEN__
    EMSCRIPTEN_KEEPALIVE
#endif
        void ProcessByteArray(char* id, char* filename, const unsigned char* data, int size) {

        unsigned char* buffer = (unsigned char*)malloc(size * sizeof(unsigned char));

        if (buffer == NULL) {
            unsigned char* d = nullptr;
            GetApplication()->LoadAssetCallback(id, filename, d, size);
        }
        else {
            memcpy(buffer, data, size);
            GetApplication()->LoadAssetCallback(id, filename, buffer, size);
        }
        // Copy the data

    }

#ifdef __EMSCRIPTEN__
    EMSCRIPTEN_KEEPALIVE
#endif
        void beforeMain(char* _info)
    {
        json& settings = GetApplication()->SetEnviornmentSettings(_info);
        int w = settings["width"];
        int h = settings["height"];
        AppResize(w, h);
    }

#ifdef __EMSCRIPTEN__
    EMSCRIPTEN_KEEPALIVE
#endif // __EMSCRIPTEN__
        void OnStageResized(int w, int h)
    {
        AppResize(w, h);
    };

#ifdef __EMSCRIPTEN__
    EMSCRIPTEN_KEEPALIVE
#endif
        void ProcessMobileInput(char* input)
    {
        GetApplication()->NotifyMobileInput(input);
    }
#ifdef __cplusplus
}
#endif


int main()
{
    cart::Application* app = GetApplication();
#ifdef _WIN32
    //AppResize(SCREEN_WIDTH, SCREEN_HEIGHT);
    std::string envsettings = { "{\"width\": " + std::to_string(SCREEN_WIDTH) + ", \"height\" : " + std::to_string(SCREEN_HEIGHT) + ", \"useragent\" : -1}" };
    beforeMain(envsettings.data());
#endif // _WIN32
    app->Init();   
    delete app;
}