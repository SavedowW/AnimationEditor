#include "Application.h"
#include <iostream>
#include <vector>
#include "Vector2.h"
#include "TimelineProperty.h"
#include "AnimViewLevel.h"

int main(int argc, char* args[])
{
    Application app;
    std::cout << app.getBasePath() << std::endl;

    app.setLevel(gamedata::stages::stage1::stageID, new AnimViewLevel(&app, {gamedata::stages::stageWidth, gamedata::stages::stageHeight}, 1));
    app.run();

    return 0;
}