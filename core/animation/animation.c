#include "animation.h"

AnimParam* animAddObject(Object* obj, AnimParam* param) {
	logger->inf(LOG_ANIM, "==== Adding Anim Object: %s ====", obj->name);

	Animator* animator = getAnimator();

	Node* objNode = getNodeByName(animator->anims, obj->name);
	if (objNode == NULL) {
		logger->dbg(LOG_ANIM, "-- Animation: Adding New Object: %s", obj->name);
		objNode = addNodeV(animator->anims, obj->name, initListMgr(), 0);
		
		logger->dbg(LOG_ANIM, "-- nodeID: %d", objNode->id);
	}

	Node* paramNode = addNodeV(objNode->value, obj->name, param, 1);
	
	param->id = paramNode->id;
	
	logger->dbg(LOG_ANIM, "==== Animation: Added %s #%d ====", param->obj->name, paramNode->id);

	return param;
}


AnimParam* initAnimParam(Object* obj, float time, float delay, void* fnc) {
	logger->inf(LOG_ANIM, "=== INIT ANIM PARAM ===");

	AnimParam* param = new(AnimParam);

	param->obj = obj;
	param->fnc = fnc;

	param->callBack = NULL;
	param->duration = time;

	param->breakAnim = false;
	param->deleteObject = false;

	param->delay = delay * FPS;
	param->frames = time * FPS;

	return param;
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
	anim->anims = initListMgr();

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

void animMoveTo(AnimParam* param) {
	logger->inf(LOG_ANIM, "==== ANIM Move: %s ====", param->obj->name);
	Object* obj = param->obj;

	logger->dbg(
		LOG_ANIM,
		"Animation: Moving ID: #%d Obj: %s\nX = %d\nY = %d",
		param->id,
		obj->name,
		obj->pos.x,
		obj->pos.y
	);

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

AnimParam* moveTo(Object* obj, int x, int y, float time, float delay) {
	logger->inf(LOG_ANIM, "==== Animation: Moving: %s to %d | %d (%fs) ====", obj->name, x, y, time);

	SDL_Rect targetPos;
	targetPos.x = x;
	targetPos.y = y;

	logger->dbg(LOG_ANIM, "-- Move To: %d | %d", x, y);
	vector vec = getVector(obj->pos, targetPos);
	logger->dbg(LOG_ANIM, "-- Move Vector: %lf | %lf", vec.x, vec.y);

	AnimParam* param = initAnimParam(obj, time, delay, animMoveTo);

	logger->dbg(LOG_ANIM, "-- Time: %f", time);
	logger->dbg(LOG_ANIM, "-- Frames: %d", param->frames);

	param->move[0] = animDistanceByFrame(vec.x, time);
	logger->dbg(LOG_ANIM, "-- xTotDist: %d", param->move[0]);

	param->move[1] = animDistanceByFrame(vec.y, time);
	logger->dbg(LOG_ANIM, "-- yTotDist: %d", param->move[1]);

	param->fnc = animMoveTo;

    animAddObject(obj, param);
    logger->dbg(LOG_ANIM, "==== Animation Added ====");
    return param;
}

bool animateObject(Node* n) {
	AnimParam* param = (AnimParam*) n->value;
	Object* obj = param->obj;

	logger->inf(LOG_ANIM, "ANIMATING: %s", obj->name);
	
}


void animate() {
	Animator* animator = getAnimator();
	listIterateFnc(animator->anims, animateObject, NULL);
}