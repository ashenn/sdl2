#ifndef _ASSET_H__
#define _ASSET_H__

#include <SDL2/SDL.h>

#include "../../common.h"
#include "../../base/libList.h"
#include "../../base/json.h"

#define ASSET_BODY  \
	CLASS_BODY			\
	ListManager* imgs;	\
	ListManager* fonts;	\
	ListManager* jsons;	\
	ListManager* confs;	\
	\
	void* (*getImg) (char* name);				\
	Json* (*getJson) (char* path);				\
	Json* (*getConf) (char* path);				\
	void* (*getFont) (char* name, short size);	\
	void* (*getTexture) (char* name, short size);	\
	\
	void (*clear) ();		\
	void (*clearImgs) ();	\
	void (*clearFonts) ();	\
	void (*clearTextures) ();	\
	\
	void* (*isCached) (char* name, ListManager* cont);			\
	void* (*cache) (char* name, ListManager* cont, void* data);	\
	\
	void (*destroy) ();

typedef struct AssetMgr {
    ASSET_BODY
} AssetMgr;

AssetMgr* getAssets();
SDL_Surface* scaleImg(SDL_Surface* surf, float scaleX, float scaleY);

Json* loadJson(char* path);

#endif
