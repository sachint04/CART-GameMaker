#include "Application.h"

namespace cart
{
    class Application;

}
extern cart::Application* GetApplication();

#ifdef __cplusplus
extern "C" {
#endif
    /*  extern void UpdateHostAboutError(const char* error_message);
      extern void GetHTTP(const char* uid, const char* url);
      extern void PostHTTP(const char* uid, const char* url, const char* data);*/

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
#ifdef __cplusplus
}
#endif


int main()
{
    cart::Application* app = GetApplication();
    app->Init();
    app->BeginPlay();
    app->Run();    
    delete app;
}