#ifndef _ASSET_H__
#define _ASSET_H__

#include "../../common.h"
#include "../../base/libList.h"

#define ASSET_BODY  \
	CLASS_BODY			\
	ListManager* imgs;	\
	ListManager* fonts;	\
	\
	void* (*getImg) (char* name);				\
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

#endif
