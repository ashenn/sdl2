#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

#include "asset.h"
#include "../../base/basic.h"
#include "../../lib/jsmn/jsmn.h"
#include "../../base/file.h"

SDL_Surface* getImg(char* name) {
	logger->inf(LOG_ASSET, "=== GETTING IMG: %s ===", name);
	AssetMgr* ast = getAssets();
	SDL_Surface* img = (SDL_Surface*) ast->isCached(name, ast->imgs);

	if (img == NULL){
		char imgPath[100];
		snprintf(imgPath, 100, "asset/%s.png", name);
		validatePath(imgPath);

		logger->dbg(LOG_ASSET, "-- Fething File: %s", imgPath);

		img = IMG_Load(imgPath);
		if (img == NULL){

			logger->err(LOG_ASSET, "-- Fail to get Image: %s", imgPath);
			logger->dbg(LOG_ASSET, "==== GETTING IMG FAILD ====");
			return NULL;
		}

		ast->cache(name, ast->imgs, img);
	}
	else{
		logger->dbg(LOG_ASSET, "-- isCached");
	}

	logger->dbg(LOG_ASSET, "=== GETTING IMG DONE ===", name);
	return img;
}

TTF_Font* getFont(char* name, short size) {
	logger->inf(LOG_ASSET, "=== GETTING FONT: %s ===", name);
	AssetMgr* ast = getAssets();

	char fontName[100];
	snprintf(fontName, 88, "%s-%d", name, size);

	TTF_Font* font = (TTF_Font*) ast->isCached(fontName, ast->fonts);

	if (font == NULL){
		char fontPath[100];
		snprintf(fontPath, 100, "font/%s.ttf", name);

		logger->dbg(LOG_ASSET, "-- Fething File: %s", fontPath);

		font = TTF_OpenFont(fontPath, size);
		if (font == NULL){
			logger->err(LOG_ASSET, "-- Fail to get FONT: %s", fontPath);
			logger->dbg(LOG_ASSET, "==== GETTING FONT FAILD ====");
			return NULL;
		}

		ast->cache(fontName, ast->fonts, font);
	}
	else{
		logger->dbg(LOG_ASSET, "-- isCached");
	}

	logger->dbg(LOG_ASSET, "=== GETTING FONT DONE ===");
	return font;
}

void clearAssets() {
	logger->inf(LOG_ASSET, "=== CLEAR ASSETS ===");

	AssetMgr* ast = getAssets();
	ast->clearImgs();
	ast->clearFonts();

	logger->dbg(LOG_ASSET, "=== CLEAR ASSETS DONE ===");
}


void clearImgs() {
	logger->inf(LOG_ASSET, "=== CLEAR IMAGES ===");
	AssetMgr* ast = getAssets();

	if (ast->imgs == NULL) {
		logger->dbg(LOG_ASSET, "=== CLEAR IMAGES DONE ===");
		return;
	}

	Node* n = NULL;
	while((n = listIterate(ast->imgs, n)) != NULL) {
		logger->dbg(LOG_ASSET, "-- Clearing: %s", n->name);

		logger->dbg(LOG_ASSET, "-- Free Surface");
	    SDL_FreeSurface(n->value);

		logger->dbg(LOG_ASSET, "-- Delete Node");
	    deleteNode(ast->imgs, n->id);

	    n = NULL;
	}

	logger->dbg(LOG_ASSET, "=== CLEAR IMAGES DONE ===");
}

void clearFonts() {
	logger->inf(LOG_ASSET, "=== CLEAR FONTS ===");
	AssetMgr* ast = getAssets();

	if (ast->fonts == NULL) {
		logger->dbg(LOG_ASSET, "=== CLEAR FONTS DONE ===");
		return;
	}

	Node* n = NULL;
	while((n = listIterate(ast->fonts, n)) != NULL) {
		logger->dbg(LOG_ASSET, "-- Clearing: %s", n->name);

		logger->dbg(LOG_ASSET, "-- Free Font");
	    TTF_CloseFont(n->value);

		logger->dbg(LOG_ASSET, "-- DELETE NODE");
	    deleteNode(ast->fonts, n->id);

	    n = NULL;
	}

	logger->dbg(LOG_ASSET, "=== CLEAR FONTS DONE ===");
}

void* assetIsCached(char* name, ListManager* cont) {
	logger->dbg(LOG_ASSET, "-- Search Cache: %s", name);
	Node* n = getNodeByName(cont, name);

	if (n != NULL) {
		logger->dbg(LOG_ASSET, "-- Cache Found: %d", n->id);
		return n->value;
	}

	return NULL;
}

void* cacheAsset(char* name, ListManager* cont, void* data) {
	logger->dbg(LOG_ASSET, "-- Caching: %s", name);
	Node* n =  addNodeV(cont, name, data, 0);

	logger->dbg(LOG_ASSET, "-- Cache Node: %p", n);
	logger->dbg(LOG_ASSET, "-- id: %d", n->id);
	logger->dbg(LOG_ASSET, "-- name: %s", n->name);

	return n;
}

void destroyAssets() {
	logger->inf(LOG_ASSET, "=== DESTROY ASSESTS ===");
	AssetMgr* ast = getAssets();

	ast->clear();
	deleteList(ast->imgs);
	deleteList(ast->fonts);
	logger->dbg(LOG_ASSET, "=== DESTROY ASSESTS ===");
}

SDL_Surface* scaleImg(SDL_Surface* surf, float scaleX, float scaleY) {
	if (scaleX < 0) {
		logger->war(LOG_ASSET, "Trying to sacle Surface with negative scaleX: %f", scaleX);
		scaleX *= -1;
	}
	if (scaleY < 0) {
		logger->war(LOG_ASSET, "Trying to sacle Surface with negative scaleY: %f", scaleY);
		scaleY *= -1;
	}

	SDL_Rect dimension;
	dimension.w = surf->w * scaleX;
	dimension.h = surf->h * scaleY;

	SDL_Surface* tmp = SDL_CreateRGBSurface(0, surf->w, surf->h, 32, 0, 0, 0, 0);

    if(tmp == NULL) {
        logger->err(LOG_ASSET, "Fail To Create RGB SURFACE: \n%s", SDL_GetError());
        return NULL;
    }

	if (SDL_BlitSurface(surf, NULL, tmp, NULL) < 0) {
		logger->err(LOG_ASSET, "Fail To blit surface for scale\n%s", SDL_GetError());
		return NULL;
    }

    SDL_Surface* res = SDL_CreateRGBSurface(0, dimension.w, dimension.h, tmp->format->BitsPerPixel, 0, 0, 0, 1);

    if(res == NULL) {
        logger->err(LOG_ASSET, "Fail To Create RESULT RGB SURFACE: \n%s", SDL_GetError());
        return NULL;
    }


    SDL_FillRect(res, &dimension, SDL_MapRGBA(res->format, 255, 0, 0, 255));

 	if (SDL_BlitScaled(tmp, NULL, res, NULL) < 0) {
 		logger->err(LOG_ASSET, "Fail To Scale Surface: %s", SDL_GetError());
 		SDL_FreeSurface(tmp);
 		SDL_FreeSurface(res);
 		return NULL;
 	}

 	SDL_FreeSurface(tmp);
 	return res;
}


Json* loadJson(char* path) {
    AssetMgr* ast = getAssets();
	Json* json = (Json*) ast->isCached(path, ast->jsons);

	if (json == NULL){
		char jsonPath[350];
		memset(jsonPath, 0, 350);
		snprintf(jsonPath, 350, "asset/%s.json", path);

		logger->dbg(LOG_ASSET, "-- Fething File: %s", jsonPath);
		json = loadJsonFile(jsonPath);

		if (json == NULL){
			logger->err(LOG_ASSET, "-- Fail to get JSON: %s", jsonPath);
			logger->dbg(LOG_ASSET, "==== GETTING JSON FAILD ====");
			return NULL;
		}

		ast->cache(path, ast->jsons, json);
	}
	else{
		logger->dbg(LOG_ASSET, "-- isCached");
	}

	return json;
}

Json* loadDefaultConf(char* src) {
	logger->inf(LOG_CONTROL, "=== Loading Default Conf: %s", src);

	char srcPath[150];
	memset(srcPath, 0, 150);
	snprintf(srcPath, 150, "%s.default", src);

	AssetMgr* ast = getAssets();
	Json* json = ast->getConf(srcPath);

	char* txt = json2Str(json, false, false);

	char confPath[150];
	memset(confPath, 0, 150);
	snprintf(confPath, 150, "config/%s.json", src);
	
	filePutContent(confPath, txt, false);

	free(txt);

	return json;
}

Json* loadConf(char* path) {
    AssetMgr* ast = getAssets();
	Json* json = (Json*) ast->isCached(path, ast->confs);

	if (json == NULL){
		char jsonPath[350];
		memset(jsonPath, 0, 350);
		snprintf(jsonPath, 350, "config/%s.json", path);

		logger->dbg(LOG_ASSET, "-- Fething File: %s", jsonPath);
		json = loadJsonFile(jsonPath);

		if (json == NULL && strstr(path, ".default.json") == NULL){
			logger->war(LOG_ASSET, "-- Fail to get Config: %s", jsonPath);
			logger->war(LOG_ASSET, "-- Getting Default Config: %s", jsonPath);

			json = loadDefaultConf(path);
		}

		if (json == NULL){
			logger->err(LOG_ASSET, "-- Fail to get Config: %s", jsonPath);
			logger->dbg(LOG_ASSET, "==== GETTING JSON FAILD ====");
			return NULL;
		}

		ast->cache(path, ast->confs, json);
	}
	else{
		logger->dbg(LOG_ASSET, "-- isCached");
	}

	return json;
}

AssetMgr* getAssets() {
	static AssetMgr* ast = NULL;

	if (ast != NULL) {
		return ast;
	}

	logger->inf(LOG_ASSET, "=== INIT ASSEST MANAGER ===");
	ast = malloc(sizeof(AssetMgr));
	ast->imgs = initListMgr();
	ast->fonts = initListMgr();
	ast->jsons = initListMgr();
	ast->confs = initListMgr();

	ast->getImg = (void*) getImg;
	ast->getFont = (void*) getFont;
	
	ast->getJson = loadJson;
	ast->getConf = loadConf;

	ast->clear = clearAssets;
	ast->clearImgs = clearImgs;
	ast->clearFonts = clearFonts;

	ast->isCached = assetIsCached;
	ast->cache = cacheAsset;

	ast->destroy = destroyAssets;
	logger->dbg(LOG_ASSET, "=== INIT ASSEST DONE ===");

	return ast;
}
