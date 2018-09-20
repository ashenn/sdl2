#include "view.h"

SDL_Window* getWindow() {
	static SDL_Window* window = NULL;
	if (window != NULL) {
		return window;
	}

	window = SDL_CreateWindow("GAME", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_W, SCREEN_H, 0);
	return window;
}

SDL_Surface* getScreen() {
	return SDL_GetWindowSurface(getWindow());
}

SDL_Renderer* getRenderer() {
	static SDL_Renderer* renderer = NULL;

	if (renderer != NULL) {
		return renderer;
	}

	renderer = SDL_CreateRenderer(getWindow(), -1,SDL_RENDERER_ACCELERATED);

	if (renderer == NULL) {
        logger->err(LOG_VIEW, "Fail To RENDER OBJECT");
        logger->err(LOG_VIEW, "%s", SDL_GetError());
        assert(0);
	}

	return renderer;
}

void printObject(Object* obj) {
	logger->inf(LOG_RENDER, "==== Printing Object: %s ====", obj->name);
	logger->inf(LOG_RENDER, "-- x=%d | y=%d", obj->pos.x, obj->pos.y);

	SDL_Surface* screen = getScreen();
	SDL_Renderer* renderer = getRenderer();

	int tmpx = obj->pos.x;
	int tmpy = obj->pos.y;

    if (obj->texture != NULL) {

		logger->inf(LOG_RENDER, "-- Blitting Surface");
		logger->inf(LOG_RENDER, "-- Pos: x: %d, y: %d, w: %d, h: %d", obj->pos.x, obj->pos.y, obj->pos.w, obj->pos.h);

		/*if (obj->clip != NULL) {
			logger->inf(LOG_RENDER, "-- Clip: x: %d, y: %d, w: %d, h: %d", obj->clip->x, obj->clip->y, obj->clip->w, obj->clip->h);
		}*/

		if (SDL_RenderCopyEx(renderer, obj->texture, obj->clip, &obj->pos, obj->rotation, NULL, obj->flip)) {
            logger->inf(LOG_RENDER, "Fail To RENDER OBJECT");
            logger->inf(LOG_RENDER, "%s", SDL_GetError());
            assert(0);
		}

	    obj->pos.x = tmpx;
	    obj->pos.y = tmpy;
    }
    else if(obj->color) {
    	logger->inf(LOG_RENDER, "FILL COLOR: ");
    	SDL_FillRect(screen, &obj->pos, obj->color);
    }
    else if(obj->childs == NULL || !obj->childs->nodeCount){
		//logger->war(LOG_RENDER, "-- Surface Is NULL for object: %s", obj->name);
    }
}
/*
void renderObject(Object* obj) {
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
*/
/*void renderObjects() {
    logger->inf(LOG_VIEW, "=== Rendering ===");

	ListManager*  layers = getLayers();

	if (!layers->nodeCount) {
		return;
	}

	Node* objNode = NULL;
	Node* layerNode = NULL;

	SDL_Renderer* rend = getRenderer();
	SDL_RenderClear(rend);

	while((layerNode = listIterate(layers, layerNode)) != NULL) {
		ListManager* objects = (ListManager*) layerNode->value;

		if (!objects->nodeCount) {
			continue;
		}
		else if (layerNode->id > 1) {
			//logger->err("===== Sorting List %s =====", layerNode->name);
			//sortList(objects, &layerSort);
		}

        objNode = NULL;
        while((objNode = listIterate(objects, objNode)) != NULL) {
            Object* obj = (Object*) objNode->value;
            if (!obj->visible) {
                continue;
            }

            renderObject(obj);
        }
	}

	SDL_RenderPresent(rend);
}
*/
