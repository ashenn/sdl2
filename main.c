#include<SDL2\SDL.h>
#include<SDL2\SDL_Image.h>
#include "common.h"
#include "base/math.h"

#include "core/asset/asset.h"

#include "core/object/object.h"
#include "core/object/SpriteObj/spriteObj.h"

#include "core/event/event.h"
#include "core/project/project.h"

#include "core/view/view.h"
#include "core/asset/asset.h"
#include "core/view/render/render.h"

#include "core/timer/timer.h"
#include "core/animation/animation.h"
#include "core/animation/sprite/spriteAnim.h"

Object* testObj = NULL;

bool leftPress(KeyEvent* evt) {
    logger->err(LOG_SPRITE, "LEFT Pressed !!!");
    testObj->flip = FLIP_H;

    spriteAnim((SpriteObject*) testObj, 2, 0);

    return false;
}

bool release(KeyEvent* evt) {
    logger->err(LOG_SPRITE, "BTN Realeased !!!");
    spriteRemoveObject(testObj);
    spriteAnim((SpriteObject*) testObj, 1, 0);

    return false;
}

bool rightPress(KeyEvent* evt) {
    logger->err(LOG_SPRITE, "Right Pressed !!!");
    testObj->flip = FLIP_N;

    spriteAnim((SpriteObject*) testObj, 2, 0);

    return false;
}

bool downPress(KeyEvent* evt) {
    logger->err(LOG_SPRITE, "Down Pressed !!!");
    testObj->flip = FLIP_N;

    spriteAnim((SpriteObject*) testObj, 3, 0);

    return false;
}


int main(int arc, char* argv[]) {
    logger = initLogger(arc, argv);

    Project* pro = initProject(arc, argv);

    logger->dbg(LOG_MAIN, "-- Init SDL");
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS);

    logger->dbg(LOG_MAIN, "-- Init Window");
    getWindow();

    loadJson("animation/adventurer");
    return 0;


    SDL_Rect pos = {10, 10, 250, 150};
    //SDL_Surface* img = IMG_Load("asset/lg-button-green.png");

    AssetMgr* ast = getAssets();
    //SDL_Surface* img = ast->getImg("lg-button-green");
    SDL_Surface* img = ast->getImg("adventurer/adventurer");

    if (img == NULL) {
        logger->err(LOG_MAIN, "Fail To Load Image");
        logger->err(LOG_MAIN, "%s", SDL_GetError());
        return 1;
    }

    testObj = (Object*) newSpriteObject("testSprite", img, &pos, 2);


    logger->err(LOG_MAIN, "BIND EVENT RIGHT");
    KeyEvent* evt = bindKeyEvent("RIGHT", SDLK_RIGHT, NULL);
    evt->pressed = rightPress;
    evt->released = release;

    logger->err(LOG_MAIN, "BIND EVENT LEFT");
    evt = bindKeyEvent("LEFT", SDLK_LEFT, NULL);
    evt->pressed = leftPress;
    evt->released = release;

    logger->err(LOG_MAIN, "BIND EVENT Down");
    evt = bindKeyEvent("DOWN", SDLK_DOWN, NULL);
    evt->pressed = downPress;
    evt->released = release;



    logger->inf(LOG_MAIN, "#### CREATE RENDER TRHEAD");
    pthread_create(&pro->renderThread, NULL, renderThread, (void*)NULL);

    logger->err(LOG_MAIN, "EVENT LOOP");
    while (pro->status != PRO_CLOSE) {
        handleEvents();
    }

    logger->inf(LOG_MAIN, "#### JOIN RENDER TRHEAD");
    pthread_join(pro->renderThread, NULL);
    logger->inf(LOG_MAIN, "#### TRHEAD CLOSED");


    closeProject();
    logger->inf(LOG_MAIN, "#### PROJECT CLOSED");

    logger->inf(LOG_MAIN, "#### CLEAN SDL");
    SDL_DestroyWindow(getWindow());
    SDL_Quit();

    logger->close();
    return 0;
}
