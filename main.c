#include<SDL2\SDL.h>
#include<SDL2\SDL_Image.h>

#include "common.h"
#include "base/math.h"

#include "core/object/object.h"
#include "core/collision/collision.h"
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

#include "core/controller/player/playerCtrl.h"

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
		logger->err(LOG_SPRITE, "Char Object Character Is Null !!!", &obj->ch->attr);
		return NULL;
	}

	obj->ch->attr.crouch = false;
	obj->ch->attr.moving = true;

	return false;
}

void* release(void* e) {
	logger->err(LOG_SPRITE, "BTN Realeased !!!");

	KeyEvent* evt = e;
	CharObj* obj = (CharObj*) evt->target;
	stopMovement(obj->ctrl);

	return false;
}



void* movePress(void* e) {
	logger->err(LOG_SPRITE, "Right Pressed !!!");

	KeyEvent* evt = (KeyEvent*) e;

	DirectionEnum dir;
	if (evt->key == SDLK_RIGHT) {
		dir = DIR_RIGHT;
		testObj->flip = FLIP_N;
	}
	else {
		dir = DIR_LEFT;
		testObj->flip = FLIP_H;
	}

	CharObj* obj = (CharObj*) evt->target;
	logger->err(LOG_SPRITE, "CALL MOVE DIR Pressed !!!");
	moveDir(obj->ctrl, dir);
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

	if (!obj->ch->attr.inAir) {
		logger->war(LOG_MAIN, "-- NOT IN AIR !!!!");
		obj->ch->attr.crouch = true;
	}

	return NULL;
}

void land(AnimParam* anim) {
	logger->err(LOG_SPRITE, "-- CALLING LAND");
	//anim = (AnimParam*) spriteAnimByName((SpriteObject*) testObj, "Land", 0);
	//anim->callback = release;
	//anim->deleteOnDone = false;

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
	//	anim = (AnimParam*) spriteAnimByName((SpriteObject*) testObj, "Fall", 0);
	//	anim->stepFnc = inAir;
	//	anim->callback = land;
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

	if (obj->ch->attr.inAir) {
		logger->war(LOG_MAIN, "-- IN AIR");
		if (obj->ch->attr.canDoubleJump && !obj->ch->attr.doubleJump) {
			logger->war(LOG_MAIN, "-- Set Double Jump");
			obj->ch->attr.doubleJump = true;
		}
	}
	else {
		logger->war(LOG_MAIN, "-- NOT IN AIR !!!!");
		obj->ch->attr.inAir = true;
		obj->ch->attr.crouch = false;
	}

	vector v = {0, -30};
	addVelocity((Object*) obj, v);

	return NULL;
}


void* spacePressed(void* evt) {
	return false;
}

int main(int arc, char* argv[]) {
	logger = initLogger(arc, argv);

	logger->dbg(LOG_MAIN, "-- Init Project");
	Project* pro = initProject(arc, argv);

	collision_init();
	logger->dbg(LOG_MAIN, "-- Init SDL");
	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS);

	logger->dbg(LOG_MAIN, "-- Init Window");
	getWindow();


	SDL_Rect pos = {0, 350, 250, 150};
	PlayerCtrl* player = spawnPlayer(1, "Player-1", "adventurer", &pos, 2);
	Character* ch = player->character;


	pos.x = 350;
	PlayerCtrl* player2 = spawnPlayer(2, "Player-2", "adventurer", &pos, 2);

	testObj = ch->obj;
	pos.x = 15;
	pos.y = 30;
	pos.w = 20;
	pos.h = 8;

	Collision* col = collision_add(testObj, "TestCol", COL_OVERLAP, COL_PLAYER, pos, true);
	collision_add(player2->obj, "XD_COL", COL_OVERLAP, COL_PLAYER, pos, true);

	logger->war(LOG_MAIN, "==== COLLISION READY: %s =====", col->name);

	collision_handle();

	return 0;

	addControl("stop", release);
	addControl("jump", topPress);
	addControl("run", movePress);
	addControl("down", downPress);

	loadControl("SideScroll", "player-1", testObj);

	logger->err(LOG_MAIN, "BIND EVENT Space");
	KeyEvent* evt = bindKeyEvent("Space", SDLK_SPACE, NULL);
	evt->pressed = spacePressed;


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
