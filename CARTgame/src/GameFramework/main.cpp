#include "Application.h"

namespace cart
{
    class Application;

}
extern cart::Application* GetApplication();

int main()
{
    cart::Application* app = GetApplication();
    app->Init();
    app->BeginPlay();
    app->Run();    
    delete app;
}