#include "animation.h"
#include "./sprite/spriteAnim.h"
#include "../collision/collision.h"
#include <stdarg.h>

AnimParam* animAddObject(Object* obj, AnimParam* param) {
	//logger->inf(LOG_ANIM, "==== Adding Anim Object: %s ====", obj->name);

	Animator* anim = getAnimator();

	//logger->dbg(LOG_ANIM, "FINDING NODE");
	//animRemoveObject(obj);
	Node* objNode = getNodeByName(anim->moves, obj->name);
	//Node* objNode = getNodeByName(anim->moves, obj->name);

	if (objNode == NULL) {
		//logger->dbg(LOG_ANIM, "-- Animation: Adding New Object: %s", obj->name);
		objNode = addNodeV(anim->moves, obj->name, initListMgr(), 0);

		//logger->dbg(LOG_ANIM, "-- nodeID: %d", objNode->id);
	}
	else{
		//logger->dbg(LOG_ANIM, "NODE FOUND !!!!");
	}

	Node* paramNode = addNodeV(objNode->value, obj->name, param, 1);

	param->id = paramNode->id;

	//logger->dbg(LOG_ANIM, "==== Animation: Added %s #%d ====", param->obj->name, paramNode->id);

	return param;
}


void initAnimParam(AnimParam* param, Object* obj, float time, float delay, void* fnc) {
	//logger->inf(LOG_ANIM, "=== INIT ANIM PARAM ===");

	//AnimParam* param = new(AnimParam);
	//logger->inf(LOG_ANIM, "=== Instance OK");

	param->isInit = true;
	param->obj = obj;
	param->fnc = fnc;

	param->stepFnc = NULL;
	param->callback = NULL;

	param->done = false;
	param->loop = false;
	param->breakAnim = false;
	param->deleteOnDone = false;
	param->deleteObject = false;

	param->delay = delay * FPS;
	param->initialFrames = param->frames = time * FPS;

	//logger->inf(LOG_ANIM, "=== PARAM READY ===");
}

bool animSetPosition(Object* obj, int x, int y) {
	//logger->err(LOG_ANIM, "Lock Anim Set Pos");
	bool b = LOCK(obj, "Anim SET POS-0");
	logger->err(LOG_ANIM, "=== Set Object Position %s => X: %d | Y: %d", obj->name, x, y);
	logger->err(LOG_ANIM, "=== FROM X: %d | Y: %d", obj->pos.x, obj->pos.y);


	bool res = true;
	logger->err(LOG_ANIM, "-- Call Can Move");
	vector move = canMoveTo(obj, x, y);

	if (move.x != x || move.y != y) {
		logger->err(LOG_ANIM, "#### CHANGE POSITION ####");
		logger->err(LOG_ANIM, "-- X: %lf", move.x);
		logger->err(LOG_ANIM, "-- Y: %lf", move.y);

		res = false;
	}

	//logger->err(LOG_ANIM, "-- Can Move Result: X: %d | %d", (int) move.x, (int) move.y);
	obj->pos.x = (int) move.x;
	obj->pos.y = (int) move.y;

	UNLOCK(obj, "Anim SET POS-1", b);
	return res;
}

Animator* getAnimator() {
	static Animator* anim = NULL;

	if (anim != NULL) {
		return anim;
	}

	anim = new(Animator);
	anim->moves = initListMgr();
	anim->sprites = initListMgr();

	return anim;
}

AnimDistance animDistanceByFrame(int dist, float time) {
	logger->inf(LOG_ANIM, "==== Calculation Animation Distance Per Frame ====");
	logger->dbg(LOG_ANIM, "-- dist: %dpx\n--time: %fs", dist, time);

	int rest;
	int frames;
	int perFrames;
	AnimDistance animDist;

	frames = FPS * time;
	if (frames < 1) {
		frames = 1;
	}

	logger->dbg(LOG_ANIM, "-- Total Frames: %d", frames);

	rest = dist % frames;
	perFrames = dist / frames;

	animDist.rest = rest % frames;
	animDist.perFrame = perFrames + (rest / frames);

	logger->dbg(LOG_ANIM, "-- dist per frame: %d\n--rest: %d", animDist.perFrame, animDist.rest);


	animDist.curDispatch = 0;
	if (rest) {
		logger->dbg(LOG_ANIM, "-- Calc Dispatch: %d / %d", frames, rest);
		animDist.dispatch = frames / rest;
	}
	else {
		animDist.dispatch = 0;
	}


	logger->dbg(LOG_ANIM, "-- Dispatch: %d", animDist.dispatch);
	logger->dbg(LOG_ANIM, "==== Calculation DONE ====");

	return animDist;
}

void animMoveTo(AnimMoveParam* param) {
	logger->inf(LOG_ANIM, "==== ANIM Move: %s ====", param->obj->name);
	Object* obj = param->obj;

	/*logger->dbg(
		LOG_ANIM,
		"Animation: Moving ID: #%d Obj: %s\nX = %d\nY = %d",
		param->id,
		obj->name,
		obj->pos.x,
		obj->pos.y
	);*/

	int xRest = param->move[0].rest;
	int xMove = param->move[0].perFrame;
	int xDispatch = param->move[0].dispatch;
	int xCurDispatch = param->move[0].curDispatch;


	int yRest = param->move[1].rest;
	int yMove = param->move[1].perFrame;
	int yDispatch = param->move[1].dispatch;
	int yCurDispatch = param->move[1].curDispatch;

	if (xDispatch && xCurDispatch < xDispatch) {
		param->move[0].curDispatch++;
		logger->dbg(LOG_ANIM, "-- Dispatching X: %d / %d", param->move[0].curDispatch, param->move[0].dispatch);
	}
	else{
		logger->dbg(LOG_ANIM, "-- Apply X Move");
		param->move[0].curDispatch = 0;
		if (xRest != 0) {
			//logger->dbg(LOG_ANIM, "-- Moving X Rest = %d", xRest);
			if (xRest > 0) {
				xMove++;
				xRest--;
			}
			else{
				xMove--;
				xRest--;
			}

			param->move[0].rest = xRest;
		}
	}

	if (yDispatch && yCurDispatch < yDispatch) {
		param->move[1].curDispatch++;
		logger->dbg(LOG_ANIM, "-- Dispatching Y: %d / %d", param->move[1].curDispatch, param->move[1].dispatch);
	}
	if (yRest != 0) {
		logger->dbg(LOG_ANIM, "-- Apply Y Move");
		param->move[1].curDispatch = 0;
		//logger->dbg(LOG_ANIM, "-- Moving Y Rest = %d", yRest);
		if (yRest > 0) {
			yMove++;
			yRest--;
		}
		else{
			yMove--;
			yRest--;
		}

		param->move[1].rest = yRest;
	}

	//logger->dbg(LOG_ANIM, "-- Moving Coords: X = %d | Y = %d", xMove, yMove);

	bool breakAnim = !animSetPosition(
		obj,
		obj->pos.x + xMove,
		obj->pos.y + yMove
	);

	if (obj->childs != NULL) {
		Node* childNode = NULL;
		logger->dbg(LOG_ANIM, "-- Moving Childs: %d", obj->childs->nodeCount);

		while((childNode = listIterate(obj->childs, childNode)) != NULL) {
			lockNode(childNode);

			Object* child = (Object*) childNode->value;
			bool b = LOCK(child, "Anim MOVE-0");

			logger->dbg(LOG_ANIM, "-- Child: %s\nvisible: %d", child->name, child->visible);

			if (child->visible) {
				animSetPosition(
					child,
					child->pos.x + xMove,
					child->pos.y + yMove
				);
			}


			UNLOCK(child, "Anim MOVE-1", b);
			unlockNode(childNode);
		}

		logger->dbg(LOG_ANIM, "-- Moving Childs DONE");
	}

	if (param->breakOnReach) {
		logger->dbg(LOG_ANIM, "-- Breaking Anim");
		bool xDone = !param->move[0].rest && !param->move[0].perFrame;
		bool yDone = !param->move[1].rest && !param->move[1].perFrame;

		if (xDone && yDone) {
			logger->inf(LOG_ANIM, "ANIMATION DONE");
			param->breakAnim = true;
			logger->inf(LOG_ANIM, "Frames Left: %d", param->frames);
		}
	}

	if (breakAnim) {
		param->breakAnim = true;
	}

	logger->inf(LOG_ANIM, "==== ANIM Move DONE ====");
}

AnimMoveParam* newMoveParam(Object* obj, float time, float delay, void* fnc) {
	AnimMoveParam* param = new(AnimMoveParam);
	initAnimParam((AnimParam*) param, obj, time, delay, fnc);

	param->duration = time;
	param->breakOnReach = false;

	return param;
}

AnimParam* moveTo(Object* obj, int x, int y, float time, float delay) {
	bool b = LOCK(obj, "MoveTo-0");
	logger->inf(LOG_ANIM, "==== Animation: Moving: %s to %d | %d (%fs) ====", obj->name, x, y, time);

	SDL_Rect targetPos;
	targetPos.x = x;
	targetPos.y = y;

	logger->dbg(LOG_ANIM, "-- Move To: %d | %d", x, y);
	vector vec = getVector(obj->pos, targetPos);
	logger->dbg(LOG_ANIM, "-- Move Vector: %lf | %lf", vec.x, vec.y);


	AnimMoveParam* param = newMoveParam(obj, time, delay, animMoveTo);

	logger->dbg(LOG_ANIM, "-- Time: %f", time);
	logger->dbg(LOG_ANIM, "-- Frames: %d", param->frames);

	param->move[0] = animDistanceByFrame(vec.x, time);
	logger->dbg(LOG_ANIM, "-- xTotDist: %d", param->move[0].perFrame + param->move[0].rest);

	param->move[1] = animDistanceByFrame(vec.y, time);
	logger->dbg(LOG_ANIM, "-- yTotDist: %d", param->move[1].perFrame + param->move[1].rest);

	param->fnc = animMoveTo;

    logger->dbg(LOG_ANIM, "==== ADDING OBJ TO ANIMATOR !!!!");

    animAddObject(obj, (AnimParam*) param);

    logger->inf(LOG_ANIM, "==== Animation Added ====");

	UNLOCK(obj, "MoveTo-1", b);
    return (AnimParam*) param;
}

short animateObject(int index, Node* n, short* delete, void* data, va_list* args) {
	ListManager* moves = (ListManager*) n->value;
	if (!moves->nodeCount) {
		*delete = 1;
		return true;
	}

	n = NULL;
	while ((n = listIterate(moves, n)) != NULL) {
		AnimParam* param = (AnimParam*) n->value;
		if (param == NULL) {
			Node* tmp = n->prev;

			//logger->err(LOG_ANIM, "-- Removing NULL Anim");
			removeAndFreeNode(moves, n);
			//logger->err(LOG_ANIM, "-- Anim NULL Removed");

			n = tmp;
			continue;
		}

		//logger->err(LOG_ANIM, "ANIMATING: %s", param->obj->name);

		if (param->delay > 0) {
			//logger->dbg(LOG_ANIM, "-- delayed: %d", param->delay);
			param->delay--;
			break;
		}

		Object* obj = param->obj;
		if (param->fnc != NULL) {
			//logger->dbg(LOG_ANIM, "-- Calling Anim Function: %s", param->obj->name);
			bool b = LOCK(obj, "ANIMMATE-0");
			param->fnc(param);
			UNLOCK(obj, "ANIMMATE-1", b);
		}

		if (param->stepFnc != NULL) {
			//logger->dbg(LOG_ANIM, "-- Calling Custom Step Function");
			bool b = LOCK(obj, "ANIMMATE-2");
			param->stepFnc(param);
			UNLOCK(obj, "ANIMMATE-3", b);
		}

		param->frames--;
		//logger->dbg(LOG_ANIM, "-- Frames Left: %d", param->frames);

		if (param->frames > 0 && !param->breakAnim) {
			break;
		}

		//logger->dbg(LOG_ANIM, "-- Animation Ended");

		if (param->callback != NULL) {
			//logger->err(LOG_ANIM, "-- Calling CallBack: %p", param);
			bool b = LOCK(obj, "ANIMMATE-4");
			param->callback(param);
			UNLOCK(obj, "ANIMMATE-5", b);
		}

		if (param->loop) {
			//logger->inf(LOG_ANIM, "-- Looping Animation");
			param->frames = param->initialFrames;
			break;
		}
		else{
			Node* tmp = n->prev;
			//logger->err(LOG_ANIM, "-- Removing Anim");
			removeAndFreeNode(moves, n);
			//logger->err(LOG_ANIM, "-- Anim Removed");

			n = tmp;
		}
	}

	if (!moves->nodeCount) {
		//logger->err(LOG_ANIM, "-- Deleting Move List Anim");
		*delete = 1;
	}

	return true;
}

short checkAnimLink(int i, Node* n, short* delete, void* param, va_list* arg) {
	AnimLink* link = (AnimLink*) n->value;
	//logger->inf(LOG_SPRITE, "-- Checking: %s", n->name);

	SpriteAnimParam* animParam = (SpriteAnimParam*) param;
	if (link->waitEnd && !animParam->done) {
		//logger->inf(LOG_SPRITE, "-- Not Ready");
		return true;
	}
	else if (link->fnc != NULL) {
		if (link->fnc(animParam)) {
			logger->inf(LOG_SPRITE, "-- Swithcing Anim: %s !!!", n->name);
			spriteAnimByName((SpriteObject*) animParam->obj, link->target, 0);
			return false;
		}
	}
	else {
		logger->dbg(LOG_SPRITE, "-- ANIM: %s No Check Function Assumed True", n->name);
		spriteAnimByName((SpriteObject*) animParam->obj, link->target, 0);
		return false;
	}

	return true;
}

void updateSpriteAnim(SpriteAnimParam* param) {
	if (param->anim->animLinks == NULL || !param->anim->animLinks->nodeCount) {
		return;
	}

	//logger->inf(LOG_SPRITE, "==== Update: %s ====", param->anim->name);

	listIterateFnc(param->anim->animLinks, checkAnimLink, NULL, (void*) param);
	//logger->inf(LOG_SPRITE, "##### UPDATE SPRITE DONE #####");
}

short animateSprite(int index, Node* n, short* delete, void* data, va_list* args) {
	SpriteAnimParam* param = (SpriteAnimParam*) n->value;

	Object* obj = (Object*) param->obj;
	bool b = LOCK(obj, "ANIMMATE-SPRITE-0");

	if (param->fnc != NULL) {
		//logger->inf(LOG_SPRITE, "== PRE WAITING #%d: %d ==", param->id, param->wait);
		param->fnc((AnimParam*) param);
		//logger->inf(LOG_SPRITE, "== AFTER WAITING #%d: %d ==", param->id, param->wait);
	}

	if (param->stepFnc != NULL) {
		param->stepFnc((AnimParam*) param);
	}
	if (param->breakAnim || (!param->loop && param->done)) {
		//logger->inf(LOG_SPRITE, "######## TEST CALL BACK NAME: %s", param->anim->name);
		if (param->callback != NULL){
			//logger->inf(LOG_SPRITE, "-- CallBack");
			param->callback((AnimParam*) param);
		}

		if (param->deleteOnDone) {
			//logger->inf(LOG_SPRITE, "-- NAME: %s", param->anim->name);
			//logger->inf(LOG_SPRITE, "-- BREAK: %d", param->breakAnim);
			//logger->inf(LOG_SPRITE, "-- DONE: %d", param->done);
			//logger->inf(LOG_SPRITE, "-- LOOP: %d", param->loop);
			*delete = true;
		}
	}

	updateSpriteAnim(param);
	//logger->inf(LOG_SPRITE, "##### Anim SPRITE DONE #####");

	UNLOCK(obj, "ANIMMATE-SPRITE-1", b);
	return true;
}


void animate() {
	Animator* anim = getAnimator();

	if (anim->moves->nodeCount) {
		logger->inf(LOG_ANIM, "Objects To Animate: #%d", anim->moves->nodeCount);
		listIterateFnc(anim->moves, animateObject, NULL, NULL);
	}


	if (anim->sprites->nodeCount) {
		//logger->inf(LOG_SPRITE, "==== ANIMATING SPRITES =====");
		listIterateFnc(anim->sprites, animateSprite, NULL, NULL);
	}
}

void animRemoveObject(Object* obj) {
	if (obj == NULL) {
		return;
	}

	logger->inf(LOG_ANIM, "==== Removing Animation For Object: %s ====", obj->name);
	Animator* anim = getAnimator();

	Node* n = getNodeByName(anim->moves, obj->name);

	if (n == NULL) {
		logger->dbg(LOG_ANIM, "-- No Anim Found");
		return;
	}

	logger->dbg(LOG_ANIM, "-- Deleting List");
	deleteList(n->value);

	logger->dbg(LOG_ANIM, "-- Remove Node");
	removeAndFreeNode(anim->moves, n);

	logger->dbg(LOG_ANIM, "==== Removing Anim %s DONE ====", obj->name);
}

void spriteRemoveObject(Object* obj) {
	if (obj == NULL) {
		return;
	}

	//logger->inf(LOG_SPRITE, "==== Removing Sprite Animation For Object: %s ====", obj->name);
	Animator* anim = getAnimator();

	//logger->inf(LOG_SPRITE, "==== Call DELETE NODE: %s ====", obj->name);
	deleteNodeByName(anim->sprites, obj->name);
}
