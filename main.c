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

Object* testObj = NULL;

bool moveTest(Event* evt) {
    logger->err(LOG_ANIM, "####### MOVE INPUT");

    moveTo(testObj, 350, 350, 5, 0);
    return true;
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

    bindKeyEvent("moveTest", SDLK_LEFT, moveTest);

    testObj = addSimpleObject("test", img, &pos, 1);

    //pos.x += 100;
    //SDL_Surface* img2 = IMG_Load("asset/lg-button-red.png");
    //addSimpleObject("test-2", img2, &pos, 1);

    logger->inf(LOG_MAIN, "#### CREATE TRHEAD");
    pthread_create(&pro->renderThread, NULL, renderThread, (void*)NULL);

    while (pro->status != PRO_CLOSE) {
        handleEvents();
        usleep(500);
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
