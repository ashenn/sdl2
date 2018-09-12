#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

#include "asset.h"
#include "../../base/basic.h"
#include "../../lib/jsmn/jsmn.h"
#include "../../base/file.h"
#include "../../base/json.h"

SDL_Surface* getImg(char* name) {
	logger->inf(LOG_ASSET, "=== GETTING IMG: %s ===", name);
	AssetMgr* ast = getAssets();
	SDL_Surface* img = (SDL_Surface*) ast->isCached(name, ast->imgs);

	if (img == NULL){
		char imgPath[100];
		snprintf(imgPath, 100, "asset/%s.png", name);
		validatePath(imgPath);

		logger->err(LOG_ASSET, "-- Fething File: %s", imgPath);

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

AssetMgr* getAssets() {
	static AssetMgr* ast = NULL;

	if (ast != NULL) {
		return ast;
	}

	logger->inf(LOG_ASSET, "=== INIT ASSEST MANAGER ===");
	ast = malloc(sizeof(AssetMgr));
	ast->imgs = initListMgr();
	ast->fonts = initListMgr();

	ast->getImg = (void*) getImg;
	ast->getFont = (void*) getFont;
	ast->clear = clearAssets;
	ast->clearImgs = clearImgs;
	ast->clearFonts = clearFonts;

	ast->isCached = assetIsCached;
	ast->cache = cacheAsset;

	ast->destroy = destroyAssets;
	logger->dbg(LOG_ASSET, "=== INIT ASSEST DONE ===");

	return ast;
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


void loadJson(char* path) {
    logger->err(LOG_ASSET,"=== TEST JSON !! ===");
	jsmn_parser parser;


	char jsonPath[350];
	memset(jsonPath, 0, 350);
	snprintf(jsonPath, 350, "asset/%s.json", path);
	loadJsonFile(jsonPath);

	return;
    logger->err(LOG_ASSET,"-- init");
    Json* json = newJson(NULL);

    float f = 15;
    jsonSetValue(json, "KEY-1", &f, JSON_NUM);
    jsonSetValue(json, "KEY-2", "TESTING", JSON_STRING);
    jsonSetValue(json, "KEY-3", NULL, JSON_NULL);


    ListManager* test = initListMgr();
    Json* array = jsonSetValue(json, "ARRAY", test, JSON_ARRAY);

    jsonSetValue(array, "1", "NEW VALUE", JSON_STRING);

    jsonPrint(json, 0);
	return;


    logger->err(LOG_ASSET,"-- init");
	jsmn_init(&parser);

	logger->err(LOG_ASSET,"-- Parsing");

	logger->err(LOG_ASSET, "LODING: %s", jsonPath);
	char* jsonStr = fileGetContent(jsonPath);
	logger->err(LOG_ASSET, "CONTENT: %s", jsonStr);


	int tokenCnt = jsmn_parse(&parser, jsonStr, strlen(jsonStr), NULL, 1500);
    logger->err(LOG_ASSET,"-- Total Count: %d", tokenCnt);


	jsmntok_t tokens[tokenCnt];
	jsmn_init(&parser);
	int res = jsmn_parse(&parser, jsonStr, strlen(jsonStr), tokens, tokenCnt);

	if (res < 0) {
        logger->err(LOG_ASSET,"-- Error !!!!");
        switch (res) {
            case JSMN_ERROR_INVAL:
                logger->err(LOG_ASSET, "JSON CORRUPTED");
                break;

            case JSMN_ERROR_NOMEM:
                logger->err(LOG_ASSET, "Not Enought TOKENS");
                break;

            case JSMN_ERROR_PART:
                logger->err(LOG_ASSET, "JSON string is too short, expecting more JSON data");
                break;

            default:
                logger->err(LOG_ASSET,"-- UNKNOWN Error !!!!");
                break;
        }

        return;
	}

	for (int i = 1; i < res; ++i) {
		logger->err(LOG_ASSET, "==== TOKEN: #%d ====", i);

		switch (tokens[i].type) {
			case JSMN_UNDEFINED:
				logger->err(LOG_ASSET, "-- Type: Undefined");
				break;

			case JSMN_OBJECT:
				logger->err(LOG_ASSET, "-- Type: Object");
				continue;
				break;

			case JSMN_ARRAY:
				logger->err(LOG_ASSET, "-- Type: ARRAY");
				continue;
				break;

			case JSMN_STRING:
				logger->err(LOG_ASSET, "-- Type: STRING");
				break;

			case JSMN_PRIMITIVE:
				logger->err(LOG_ASSET, "-- Type: NUMBER");
				break;

	        default:
	            logger->err(LOG_ASSET,"-- Type: UNKNOWN");
	            break;
		}

		logger->err(LOG_ASSET, "-- Position: %d | %d => %d", tokens[i].start, tokens[i].end, tokens[i].size);

		char test[150];
		memset(test, 0, 150);
		memcpy(test, &jsonStr[tokens[i].start], tokens[i].end - tokens[i].start);

		logger->err(LOG_ASSET, "-- Value: %s", test);
		/* code */
	}

}
