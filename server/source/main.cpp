#include <drogon/drogon.h>

int main()
{
    drogon::app().loadConfigFile("resource/config.json");
    drogon::app().run();
}
