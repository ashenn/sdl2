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

    spriteAnimByName((SpriteObject*) testObj, "Run", 0);

    return false;
}

bool release(KeyEvent* evt) {
    logger->err(LOG_SPRITE, "BTN Realeased !!!");
    spriteAnimByName((SpriteObject*) testObj, "Idle", 0);

    return false;
}

bool rightPress(KeyEvent* evt) {
    logger->err(LOG_SPRITE, "Right Pressed !!!");
    testObj->flip = FLIP_N;

    spriteAnimByName((SpriteObject*) testObj, "Run", 0);

    return false;
}

bool downPress(KeyEvent* evt) {
    logger->err(LOG_SPRITE, "Down Pressed !!!");
    spriteAnimByName((SpriteObject*) testObj, "Down", 0);

    return false;
}

void land(AnimParam* anim) {
    logger->err(LOG_SPRITE, "-- CALLING LAND");
    anim = (AnimParam*) spriteAnimByName((SpriteObject*) testObj, "Land", 0);
    anim->callback = release;
    anim->deleteOnDone = false;
    logger->err(LOG_SPRITE, "-- LAND CALLED");
}

void inAir(AnimParam* anim) {
    static int i = 50;
    if (--i == 0) {
        i = 25;
        anim->breakAnim = true;
    }
    else{
        logger->err(LOG_SPRITE, "-- In Air: %d", i);
    }
}

void jumpEnd(AnimParam* anim) {
    logger->err(LOG_SPRITE, "==== JUMP ENDED ====");
    anim = (AnimParam*) spriteAnimByName((SpriteObject*) testObj, "Fall", 0);
    anim->stepFnc = inAir;
    anim->callback = land;
}

bool topPress(KeyEvent* evt) {
    logger->err(LOG_SPRITE, "Top Pressed !!!");
    SpriteAnimParam* anim = spriteAnimByName((SpriteObject*) testObj, "Jump", 0);
    anim->callback = jumpEnd;

    return false;
}

int main(int arc, char* argv[]) {
    logger = initLogger(arc, argv);

    Project* pro = initProject(arc, argv);

    logger->dbg(LOG_MAIN, "-- Init SDL");
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS);

    logger->dbg(LOG_MAIN, "-- Init Window");
    getWindow();


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

    logger->err(LOG_MAIN, "BIND EVENT Top");
    evt = bindKeyEvent("TOP", SDLK_UP, NULL);
    evt->pressed = topPress;
    //evt->released = release;



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
