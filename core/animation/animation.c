#include "animation.h"
#include <stdarg.h>

AnimParam* animAddObject(Object* obj, AnimParam* param) {
	logger->inf(LOG_ANIM, "==== Adding Anim Object: %s ====", obj->name);

	Animator* anim = getAnimator();

	logger->dbg(LOG_ANIM, "FINDING NODE");
	Node* objNode = getNodeByName(anim->moves, obj->name);

	if (objNode == NULL) {
		logger->dbg(LOG_ANIM, "-- Animation: Adding New Object: %s", obj->name);
		objNode = addNodeV(anim->moves, obj->name, initListMgr(), 0);

		logger->dbg(LOG_ANIM, "-- nodeID: %d", objNode->id);
	}
	else{
		logger->dbg(LOG_ANIM, "NODE FOUND !!!!");
	}

	Node* paramNode = addNodeV(objNode->value, obj->name, param, 1);

	param->id = paramNode->id;

	logger->dbg(LOG_ANIM, "==== Animation: Added %s #%d ====", param->obj->name, paramNode->id);

	return param;
}


void initAnimParam(AnimParam* param, Object* obj, float time, float delay, void* fnc) {
	logger->inf(LOG_ANIM, "=== INIT ANIM PARAM ===");

	//AnimParam* param = new(AnimParam);
	logger->inf(LOG_ANIM, "=== Instance OK");

	param->isInit = true;
	param->obj = obj;
	param->fnc = fnc;

	param->stepFnc = NULL;
	param->callback = NULL;

	param->loop = false;
	param->breakAnim = false;
	param->deleteObject = false;

	param->delay = delay * FPS;
	param->initialFrames = param->frames = time * FPS;

	logger->inf(LOG_ANIM, "=== PARAM READY ===");
}


void animSetPosition(Object* obj, int x, int y) {
	LOCK(obj);
	logger->inf(LOG_ANIM, "=== Set Object Position %s => x: %d | u: %d", obj->name, x, y);

	obj->pos.x = x;
	obj->pos.y = y;

	UNLOCK(obj);
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

	animDist.perFrame = perFrames + (rest / frames);
	animDist.rest = rest % frames;

	logger->dbg(LOG_ANIM, "-- dist per frame: %d\n--rest: %d", animDist.perFrame, animDist.rest);
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

	int xMove = param->move[0].perFrame;
	int yMove = param->move[1].perFrame;

	int xRest = param->move[0].rest;
	int yRest = param->move[1].rest;

	if (xRest != 0) {
		logger->dbg(LOG_ANIM, "-- Moving X Rest = %d", xRest);
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

	if (yRest != 0) {
		logger->dbg(LOG_ANIM, "-- Moving Y Rest = %d", yRest);
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

	logger->dbg(LOG_ANIM, "-- Moving Coords: X = %d | Y = %d", xMove, yMove);

	animSetPosition(
		obj,
		obj->pos.x + xMove,
		obj->pos.y + yMove
	);

	if (obj->childs != NULL) {
		Node* childNode = NULL;
		logger->dbg(LOG_ANIM, "-- Moving Childs: %d", obj->childs->nodeCount);

		LOCK(obj->childs);
		while((childNode = listIterate(obj->childs, childNode)) != NULL) {
			LOCK(childNode);

			Object* child = (Object*) childNode->value;

			logger->dbg(LOG_ANIM, "-- Child: %s\nvisible: %d", child->name, child->visible);

			if (child->visible) {
				animSetPosition(
					child,
					child->pos.x + xMove,
					child->pos.y + yMove
				);
			}


			UNLOCK(childNode);
		}
		UNLOCK(obj->childs);
	}
}

AnimMoveParam* newMoveParam(Object* obj, float time, float delay, void* fnc) {
	AnimMoveParam* param = new(AnimMoveParam);
	initAnimParam((AnimParam*) param, obj, time, delay, fnc);

	param->duration = time;

	return param;
}

AnimParam* moveTo(Object* obj, int x, int y, float time, float delay) {
	logger->err(LOG_ANIM, "==== Animation: Moving: %s to %d | %d (%fs) ====", obj->name, x, y, time);

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
	logger->dbg(LOG_ANIM, "-- xTotDist: %d", param->move[0]);

	param->move[1] = animDistanceByFrame(vec.y, time);
	logger->dbg(LOG_ANIM, "-- yTotDist: %d", param->move[1]);

	param->fnc = animMoveTo;

    logger->dbg(LOG_ANIM, "==== ADDING OBJ TO ANIMATOR !!!!");

    animAddObject(obj, (AnimParam*) param);
    logger->dbg(LOG_ANIM, "==== Animation Added ====");
    return (AnimParam*) param;
}

short animateObject(int index, Node* n, short* delete, void* data, va_list* args) {
	ListManager* moves = (ListManager*) n->value;
	if (!moves->nodeCount) {
		return true;
	}

	AnimParam* param = (AnimParam*) moves->first->value;

	logger->inf(LOG_ANIM, "ANIMATING: %s", param->obj->name);

	if (param->delay > 0) {
		logger->dbg(LOG_ANIM, "-- delayed: %d", param->delay);
		param->delay--;
		return true;
	}

	if (param->fnc != NULL) {
		logger->dbg(LOG_ANIM, "-- Calling Anim Function: %s", param->obj->name);
		param->fnc(param);
	}

	if (param->stepFnc != NULL) {
		logger->dbg(LOG_ANIM, "-- Calling Custom Step Function");
		param->stepFnc(param);
	}

	param->frames--;
	logger->dbg(LOG_ANIM, "-- Frames Left: %d", param->frames);

	if (param->frames > 0 && !param->breakAnim) {
		return true;
	}

	logger->dbg(LOG_ANIM, "-- Animation Ended");

	if (param->callback != NULL) {
		logger->inf(LOG_ANIM, "-- Calling CallBack");
		param->callback(param);
	}

	if (param->loop) {
		logger->inf(LOG_ANIM, "-- Looping Animation");
		param->frames = param->initialFrames;
	}
	else{
		*delete = 1;
	}

	return true;
}


void animate() {
	Animator* anim = getAnimator();

	if (!anim->moves->nodeCount) {
		logger->inf(LOG_ANIM, "No Objects To Animate");
		return;
	}

	logger->inf(LOG_ANIM, "Objects To Animate: #%d", anim->moves->nodeCount);
	listIterateFnc(anim->moves, animateObject, NULL, NULL);
}

void animRemoveObject(Object* obj) {
	if (obj == NULL) {
		return;
	}

	logger->inf(LOG_ANIM, "==== Removing Animation For Object: %s ====", obj->name);
	Animator* anim = getAnimator();

	Node* n = getNodeByName(anim->moves, obj->name);

	if (n == NULL) {
		return;
	}

	logger->dbg(LOG_ANIM, "-- Clearing Anim Que");

	//pthread_mutex_lock(anim->moves->mutex);
	//logger->dbg(LOG_ANIM, "-- Node Ask-Locked");
	//pthread_mutex_lock(n->mutex);
	//logger->dbg(LOG_ANIM, "-- Node Locked");

	logger->dbg(LOG_ANIM, "-- Deleting List");
	deleteList(n->value);

	logger->dbg(LOG_ANIM, "-- Remove Node");
	removeAndFreeNode(anim->moves, n);

	logger->dbg(LOG_ANIM, "-- Unlock Node");
	//pthread_mutex_unlock(n->mutex);
	//pthread_mutex_unlock(anim->moves->mutex);

	logger->dbg(LOG_ANIM, "==== Removing Anim %s DONE ====", obj->name);
}
