#include<SDL2\SDL.h>
#include<SDL2\SDL_Image.h>
#include "common.h"
#include "base/math.h"
#include "core/view/view.h"
#include "core/object/object.h"
#include "core/event/event.h"
#include "core/project/project.h"
#include "core/view/render/render.h"
#include "core/asset/asset.h"
#include "core/animation/animation.h"
#include "core/timer/timer.h"

Object* testObj = NULL;

bool leftTest(KeyEvent* evt) {
    logger->err(LOG_ANIM, "CLICKED LEFT !!!");

    pauseDelayedFunction((DelayedFncLauncher*) evt->arg);
    //killDelayedFunction((DelayedFncLauncher*) evt->arg, true, true);
    return false;

    logger->inf(LOG_ANIM, "###### CLEAR ANIM LEFT ######");
    animRemoveObject(testObj);

    logger->inf(LOG_ANIM, "###### CLEAR ADD ANIM LEFT ######");
    moveTo(
       testObj,
       10,
       150,
       5, 0
    );

    logger->inf(LOG_ANIM, "###### ANIM LEFT ADDED ######");
    return false;
}

bool topTest(KeyEvent* evt) {
    logger->err(LOG_ANIM, "CLICKED TOP !!!");
    resumeDelayedFunction((DelayedFncLauncher*) evt->arg);
    return false;

    logger->inf(LOG_ANIM, "###### CLEAR ANIM TOP ######");
    animRemoveObject(testObj);

    logger->inf(LOG_ANIM, "###### CLEAR ADD ANIM TOP ######");
    moveTo(
        testObj,
        150,
        10,
        5, 0
    );


    logger->inf(LOG_ANIM, "###### ANIM TOP ADDED ######");

    return false;
}

bool rightTest(KeyEvent* evt) {
    logger->err(LOG_ANIM, "CLICKED RIGHT !!!");

    logger->inf(LOG_ANIM, "###### CLEAR ANIM RIGHT ######");
    animRemoveObject(testObj);

    logger->inf(LOG_ANIM, "###### CLEAR ADD ANIM RIGHT ######");
    moveTo(testObj, 790 - (testObj->pos.w / 2), (SCREEN_H / 2) - (testObj->pos.h / 2), 5, 0);

    logger->inf(LOG_ANIM, "###### ANIM RIGHT ADDED ######");

    return false;
}

bool bottomTest(KeyEvent* evt) {
    logger->err(LOG_ANIM, "CLICKED BOTTOM !!!");

    animRemoveObject(testObj);
    moveTo(testObj, (SCREEN_W / 2) - testObj->pos.w, 590 - testObj->pos.h, 5, 0);

    logger->inf(LOG_ANIM, "###### ANIM BOTTOM ADDED ######");

    return true;
}

void* testTimer(void* arg) {
    static int i = 0;
    DelayedFunction* param = (DelayedFunction*) arg;

    if (++i > 25) {
        param->doBreak = true;
    }

    logger->inf(LOG_TIMER, "==== TESTING TIMER DELAYED  %d ====", i);
    logger->inf(LOG_TIMER, "-- Do Break: %d", param->doBreak);

    return NULL;
}

void* testTimer2(void* arg) {
    static int i = 0;
    DelayedFunction* param = (DelayedFunction*) arg;
    param->doBreak = i++ > 7;

    logger->inf(LOG_TIMER, "==== TESTING TIMER 2 DELAYED  %d ====", i);
    logger->inf(LOG_TIMER, "-- Do Break: %d", param->doBreak);

    return NULL;
}

void* callBackTimer(DelayedFunction* fncParam) {
    logger->inf(LOG_TIMER, "==== Call Back: %s ====", fncParam->name);
    return NULL;
}

int main(int arc, char* argv[]) {
    logger = initLogger(arc, argv);

    Project* pro = initProject(arc, argv);

    logger->dbg(LOG_MAIN, "-- Init SDL");
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS);

    logger->dbg(LOG_MAIN, "-- Init Window");
    getWindow();

    SDL_Rect pos = {10, 10, 250, 150};
    SDL_Surface* imgTmp = IMG_Load("asset/lg-button-green.png");

    if (imgTmp == NULL) {
        logger->err(LOG_MAIN, "Fail To Load Image");
        logger->err(LOG_MAIN, "%s", SDL_GetError());
        return 1;
    }


    SDL_Surface* img = scaleImg(imgTmp, 1.0f, 1.0f);

    if (img == NULL) {
        logger->err(LOG_MAIN, "Fail To SCALE Image");
        return 1;
    }

    KeyEvent* evt = bindKeyEvent("rightTest", SDLK_RIGHT, NULL);
    evt->pressed = rightTest;

    evt = bindKeyEvent("bottomTest", SDLK_DOWN, NULL);
    evt->pressed = bottomTest;

    testObj = addSimpleObject("Test Object", img, &pos, 1);

    //pos.x += 100;
    //SDL_Surface* img2 = IMG_Load("asset/lg-button-red.png");
    //addSimpleObject("test-2", img2, &pos, 1);


    logger->inf(LOG_MAIN, "#### CREATE TRHEAD");
    pthread_create(&pro->renderThread, NULL, renderThread, (void*)NULL);


    DelayedFncLauncher* launcher = delayed(1.0f, true, testTimer, callBackTimer);

    evt = bindKeyEvent("leftTest", SDLK_LEFT, NULL);
    evt->pressed = leftTest;
    evt->arg = (void*) launcher;

    evt = bindKeyEvent("upTest", SDLK_UP, NULL);
    evt->pressed = topTest;
    evt->arg = (void*) launcher;
    //delayed(1.0f, true, testTimer2, callBackTimer);

    while (pro->status != PRO_CLOSE) {
        handleEvents();
    }

    logger->inf(LOG_MAIN, "#### JOIN TRHEAD");

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
