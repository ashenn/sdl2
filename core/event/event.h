#ifndef EVENT
#define EVENT

#include <SDL2/SDL.h>
#include "../../common.h"

#define EVENT_MGR_BODY \
    CLASS_BODY  \
    ListManager* keyEvts;   \
    ListManager* inputEvts;


typedef struct EventMgr
{
    EVENT_MGR_BODY
} EventMgr;


typedef struct Event Event;

#define EVENT_BODY \
    CLASS_BODY \
    bool enabled; \
    bool (*fnc)(Event* evt); \
    void (*remove)(Event* evt);


struct Event {
    EVENT_BODY
};

typedef struct KeyEvent KeyEvent;
#define KEY_EVT_BODY \
    EVENT_BODY  \
    bool allowRepeat;  \
    bool (*pressed)(KeyEvent* evt);  \
    bool (*released)(KeyEvent* evt);

struct KeyEvent {
    KEY_EVT_BODY
};

void handleEvents();
EventMgr* getEventMgr();
KeyEvent* bindKeyEvent(char* label, SDL_Keycode key, bool (*fnc)(Event* evt));


#endif
