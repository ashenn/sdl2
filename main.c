#include<SDL2\SDL.h>
#include<SDL2\SDL_Image.h>

#include "common.h"
#include "base/math.h"

#include "core/object/object.h"
#include "core/object/SpriteObj/spriteObj.h"
#include "core/object/SpriteObj/character/charObj.h"

#include "core/event/event.h"
#include "core/event/control/control.h"
#include "core/project/project.h"

#include "core/view/view.h"
#include "core/asset/asset.h"
#include "core/view/render/render.h"

#include "core/timer/timer.h"
#include "core/animation/animation.h"
#include "core/animation/sprite/spriteAnim.h"

#include "core/character/character.h"
#include "core/character/charAttr.h"

Object* testObj = NULL;

void* leftPress(void* e) {
	logger->err(LOG_SPRITE, "LEFT Pressed !!!");
	testObj->flip = FLIP_H;

	KeyEvent* evt = e;

	CharObj* obj = (CharObj*) evt->target;
	if (obj == NULL) {
		logger->war(LOG_MAIN, "EVENT OBJ IS NULL");
		return NULL;
	}

	if (obj->ch == NULL) {
		logger->err(LOG_SPRITE, "Char Object Character Is Null!!!", &obj->ch->attr);
		return NULL;
	}

	Character* ch = obj->ch;
	obj->ch->attr.crouch = false;
	obj->ch->attr.moving = true;

	return false;
}

void* release(void* e) {
	logger->err(LOG_SPRITE, "BTN Realeased !!!");

	KeyEvent* evt = e;
	CharObj* obj = (CharObj*) evt->target;
	if (obj == NULL) {
		logger->war(LOG_MAIN, "EVENT OBJ IS NULL");
		return NULL;
	}

	if (obj->ch == NULL) {
		logger->err(LOG_SPRITE, "Char Object Character Is Null!!!", &obj->ch->attr);
		return NULL;
	}

	Character* ch = obj->ch;
	obj->ch->attr.crouch = false;
	obj->ch->attr.moving = false;
	
	return false;
}

void* rightPress(void* e) {
	logger->err(LOG_SPRITE, "Right Pressed !!!");

	KeyEvent* evt = (KeyEvent*) e;
	if (evt->key == SDLK_RIGHT) {
		testObj->flip = FLIP_N;
	}
	else {
		testObj->flip = FLIP_H;
	}

	CharObj* obj = (CharObj*) evt->target;
	obj->ch->attr.crouch = false;
	obj->ch->attr.moving = true;

	return false;
}

void* downPress(void* e) {
	logger->err(LOG_SPRITE, "Down Pressed !!!");
	KeyEvent* evt = e;

	CharObj* obj = (CharObj*) evt->target;
	if (obj == NULL) {
		logger->war(LOG_MAIN, "EVENT OBJ IS NULL");
		return NULL;
	}

	if (obj->ch == NULL) {
		logger->err(LOG_SPRITE, "Char Object Character Is Null!!!", &obj->ch->attr);
		return NULL;
	}

	Character* ch = obj->ch;
	if (!obj->ch->attr.inAir) {
		logger->war(LOG_MAIN, "-- NOT IN AIR !!!!");
		obj->ch->attr.crouch = true;
	}

	return NULL;
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

void* topPress(void* e) {
	logger->err(LOG_SPRITE, "Top Pressed !!!");
	KeyEvent* evt = e;

	CharObj* obj = (CharObj*) evt->target;
	if (obj == NULL) {
		logger->war(LOG_MAIN, "EVENT OBJ IS NULL");
		return NULL;
	}

	if (obj->ch == NULL) {
		logger->err(LOG_SPRITE, "Char Object Character Is Null!!!", &obj->ch->attr);
		return NULL;
	}

	Character* ch = obj->ch;
	if (obj->ch->attr.inAir) {
		logger->war(LOG_MAIN, "-- IN AIR");
		SpriteAnimParam* anim = spriteAnimByName((SpriteObject*) obj, "DoubleJump", 0);
	}
	else {
		logger->war(LOG_MAIN, "-- NOT IN AIR !!!!");
		obj->ch->attr.inAir = true;
		obj->ch->attr.crouch = false;
	}

	return NULL;
}

int main(int arc, char* argv[]) {
	logger = initLogger(arc, argv);

	logger->dbg(LOG_MAIN, "-- Init Project");
	Project* pro = initProject(arc, argv);

	logger->dbg(LOG_MAIN, "-- Init SDL");
	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS);

	logger->dbg(LOG_MAIN, "-- Init Window");
	getWindow();

	SDL_Rect pos = {10, 10, 250, 150};
	Character* ch = initCharacter(CHAR_PLAYER, "adventurer", &pos, 2);
	testObj = ch->obj;

	addControl("stop", release);
	addControl("jump", topPress);
	addControl("run", rightPress);
	addControl("down", downPress);

	loadControl("SideScroll", "player-1", testObj);

	/*logger->err(LOG_MAIN, "BIND EVENT Top");
	KeyEvent* evt = bindKeyEvent("TOP", SDLK_UP, NULL);
	evt->holdMin = 0.0f;
	evt->holdMax = 0.75f;
	evt->callHoldOnMax = true;
	evt->hold = topPress;*/


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
