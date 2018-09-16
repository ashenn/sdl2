#ifndef JSON_H
#define JSON_H value

#include "basic.h"
#include "file.h"
#include "logger.h"
#include "liblist.h"
#include "../common.h"

#define LOG_JSON 1

#define JSON_TYPES(TYPE) \
	TYPE(JSON_NULL) \
	TYPE(JSON_BOOL) \
	TYPE(JSON_INT) \
	TYPE(JSON_NUM) \
	TYPE(JSON_STRING) \
	TYPE(JSON_ARRAY) \
	TYPE(JSON_OBJECT)

#define GEN_JSON_TYPE_ENUM(ENUM) ENUM,
#define GEN_JSON_TYPE_STRING(STRING) #STRING,

typedef enum JsonDataEnum {
	JSON_TYPES(GEN_JSON_TYPE_ENUM)
} JsonDataEnum;

static const char* GEN_JSON_TYPE_STRING[] = {
    JSON_TYPES(GEN_JSON_TYPE_STRING)
};

typedef struct Json Json;

struct Json {
	int id;
	char* key;
	int integer;
	float num;
	bool boolean;
	char* string;

	Json* parent;
	int childCount;
	JsonDataEnum type;
	ListManager* childs;
};

typedef struct JsonIterator
{
	bool (*fnc)(unsigned int i, Json* json, void*);
} JsonIterator;


Json* newJson();
void deleteJson(Json* json);

Json* loadJsonFile(char* path);
Json* jsonGetData(Json* json, char* key);
void* jsonGetValue(Json* json, char* key, void* floatP);

void jsonPrint(Json* json, int tab);
char* json2Str(Json* json, bool breakLine, bool indent);
Json* jsonSetValue(Json* json, char* key, void* value, JsonDataEnum type);

void jsonIterate(Json* json, bool (*fnc)(unsigned int, Json*, void*), void* param, ...);

#endif
