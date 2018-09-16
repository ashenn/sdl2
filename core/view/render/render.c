#include "render.h"
#include "../../project/project.h"
#include "../../animation/animation.h"
#include <stdarg.h>

void renderObject(Object* obj) {
	logger->inf(LOG_RENDER, "PRINT OBJECT: #%s", obj->name);
    printObject(obj);

	if (obj->childs != NULL && obj->childs->nodeCount > 0) {
		Node* childNode = NULL;

		while((childNode = listIterate(obj->childs, childNode)) != NULL) {
			Object* child = (Object*) childNode->value;

			if (child->visible) {
				renderObject(child);
			}
		}
	}
}

short iterateObjects(int i, Node* n, short* delete, void* param, va_list* args) {
	logger->inf(LOG_RENDER, "OBJECT: #%d", n->id);
	if (n->value == NULL) {
		logger->inf(LOG_RENDER, "SURFACE IS NULL");
		return 1;
	}

	Object* obj = (Object*) n->value;
	logger->inf(LOG_RENDER, "RENDER Obj: %s", obj->name);
	renderObject(obj);

	return 1;
}

short iterateLayers(int i, Node* n, short* delete, void* param, va_list* args) {
	if (n->value == NULL) {
		return 1;
	}

	logger->inf(LOG_RENDER, "LAYER: #%d", n->id);
	ListManager* layer = (ListManager*) n->value;
	listIterateFnc(layer, iterateObjects, NULL, 0);

	return 1;
}

void* renderThread(void* arg) {
	logger->inf(LOG_RENDER, "##### INIT RENDER THERAD #####");
	Project* pro = getProject();
	SDL_Renderer* rend = getRenderer();
	ListManager* layerList = getLayers();

	int nextTick = SDL_GetTicks();

	pthread_t animThread;
	//pthread_create(&animThread, NULL, renderThread, (void*)NULL);
	while (pro->status != PRO_CLOSE) {
		UNLOCK(pro);
        nextTick += (1000 / FPS);


		SDL_RenderClear(rend);
        animate();

        if (!layerList->nodeCount) {
        	logger->war(LOG_RENDER, "NO LAYERS TO RENDER !!!");
        }

		listIterateFnc(layerList, iterateLayers, NULL, NULL);
		SDL_RenderPresent(rend);

		int waited = tickWait(nextTick);
		logger->dbg(LOG_RENDER, "Frame Wait: %d ms", waited);

		nextTick = SDL_GetTicks();
		LOCK(pro);
	}

	UNLOCK(pro);
	return NULL;
}
