#ifndef PROJECT_H
#define PROJECT_H value

#include "../../common.h"
#include "../../base/logger.h"
#include "../../base/libParse.h"

#define PROJECT_STATES(STATE) \
	STATE(PRO_INIT) \
	STATE(PRO_START) \
	STATE(PRO_END) \
	STATE(PRO_CLOSE) \

#define GEN_STATE_ENUM(ENUM) ENUM,
#define GEN_STATE_STRING(STRING) #STRING,

typedef enum ProjectState {
	PROJECT_STATES(GEN_STATE_ENUM)
} ProjectState;

static const char* GEN_STATE_STRING[] = {
    PROJECT_STATES(GEN_STATE_STRING)
};

typedef struct Project
{
	CLASS_BODY
	unsigned int flags;
	unsigned int status;
	ListManager* flagList;

	pthread_t renderThread;
	pthread_t collisionThread;
} Project;

Project* getProject();
void changeStatus(ProjectState state);
Project* initProject(int argc, char* argv[]);
void closeProject();

#endif
