#include "event.h"
#include "../project/project.h"
#include "../../base/basic.h"


KeyEvent* bindKeyEvent(char* label, SDL_Keycode key, bool (*fnc)(Event* evt)) {
    logger->inf(LOG_EVENT, "BINDING EVENT: %s", label);
    EventMgr* mgr = getEventMgr();
    logger->inf(LOG_EVENT, "testing");

    logger->inf(LOG_EVENT, "Getting Node: %s", label);
    Node* n = getNode(mgr->keyEvts, (int)key);
    logger->inf(LOG_EVENT, "CHECKING Node");
    if (n == NULL) {
        char name[150];
        memset(name, 0, 150);
        snprintf(name, 150, "Key:%s", SDL_GetKeyName(key));

        logger->inf(LOG_EVENT, "CREATING EVENT LIST: %s", name);
        n = addNodeV(mgr->keyEvts, name, initListMgr(), 1);
        n->id = key;
    }

    logger->inf(LOG_EVENT, "-- Init Event");
    ListManager* keyEvents = (ListManager*) n->value;
    KeyEvent* evt = new(KeyEvent);
    evt->fnc = fnc;
    evt->enabled = true;
    evt->pressed = NULL;
    evt->released = NULL;
    evt->name = Str(label);
    evt->allowRepeat = false;

    logger->inf(LOG_EVENT, "ADD EVENT TO LIST");

    n = addNodeV(keyEvents, label, evt, 1);
    evt->id = n->id;

    return evt;
}

EventMgr* getEventMgr() {
    logger->inf(LOG_EVENT, "==== GETTTING EVNT MANAGER ====");
    static EventMgr* mgr = NULL;

    if (mgr != NULL) {
        logger->inf(LOG_EVENT, "Already Init");
        return mgr;
    }

    logger->inf(LOG_EVENT, "-- Initinialize !");
    mgr = new(EventMgr);
    logger->inf(LOG_EVENT, "-- Key list !");
    mgr->keyEvts = initListMgr();
    logger->inf(LOG_EVENT, "-- input list !");
    mgr->inputEvts = initListMgr();

    return mgr;
}

ListManager* getEventsByKey(int key) {
    EventMgr* mgr = getEventMgr();
    Node* n = getNode(mgr->keyEvts, key);

    if (n == NULL) {
        return NULL;
    }

    return (ListManager*) n->value;
}

short callEventQue(int i, Node* n, short* delete, void* param, va_list* args) {
    logger->inf(LOG_EVENT, "==== CALL EVENT QUEU ====");
    if (n->value == NULL) {
        logger->war(LOG_EVENT, "EVENT IS NULL !!!!");
        return true;
    }

    logger->inf(LOG_EVENT, "-- GET EVENT");
    KeyEvent* evt = (KeyEvent*) n->value;
    if (!evt->enabled) {
        logger->dbg(LOG_EVENT, "-- Event Disabled");
        return true;
    }


    bool doBreak = false;
    bool up = *((bool*) param);

    SDL_Event* sdl_evt = (SDL_Event*) va_arg(*args, SDL_Event*);
    if (sdl_evt->key.repeat && !evt->allowRepeat) {
        logger->dbg(LOG_EVENT, "-- Skipping Reapeat");
        return true;
    }

    logger->inf(LOG_EVENT, "-- TEST PARAM: %d", up);

    logger->dbg(LOG_EVENT, "-- Call EVENT: %s", n->name);

    if (!up && evt->pressed != NULL) {
        logger->dbg(LOG_EVENT, "-- Call Pressed");
        doBreak = !evt->pressed(evt);
    }
    else if(up && evt->released != NULL) {
        logger->dbg(LOG_EVENT, "-- Call Released");
        doBreak = !evt->released(evt);
    }

    if (evt->fnc != NULL) {
        logger->dbg(LOG_EVENT, "-- Call FNC");
        doBreak = !evt->fnc((Event*) evt) || !doBreak;
    }

    return !doBreak;
}

void eventKey(int key, bool up, SDL_Event* evt) {
    logger->inf(LOG_EVENT, "==== Key EVENT ====");

    if (key == SDLK_ESCAPE) {
        logger->dbg(LOG_EVENT, "-- Event: Quit");
        logger->err(LOG_EVENT, "############# Event: Quit ##################");

        changeStatus(PRO_CLOSE);
        return;
    }

    ListManager* events = getEventsByKey(key);
    if (events == NULL) {
        logger->inf(LOG_EVENT, "no Bound");
        return;
    }

    logger->inf(LOG_EVENT, "==== CALL ITERATE ====");
    listIterateFnc(events, callEventQue, NULL, &up, evt);
    logger->inf(LOG_EVENT, "==== ITERATE DONE ====");

    logger->inf(LOG_EVENT, "==== EVENT DONE ====");
}

void handleEvents() {
    logger->inf(LOG_EVENT, "==== HANDLE EVENTS ====");
    int key = -1;
    SDL_Event event;

    if (SDL_PollEvent(&event)) {
        logger->inf(LOG_EVENT, "-- LOOP");
        logger->inf(LOG_EVENT, "-- Type: %d", event.type);
        switch (event.type) {
            case SDL_QUIT:
                logger->inf(LOG_EVENT, "Evt Quit");
                changeStatus(PRO_CLOSE);
                break;

            case SDL_KEYUP:
                logger->inf(LOG_EVENT, "Evt Key Up");
                key = event.key.keysym.sym;
                eventKey(key, true, &event);
                break;

            case SDL_KEYDOWN:
                logger->inf(LOG_EVENT, "Evt Key Down");
                key = event.key.keysym.sym;
                eventKey(key, false, &event);
                break;

            default:
                logger->inf(LOG_EVENT, "Unknown Event Type");
                break;

        }

        logger->inf(LOG_EVENT, "-- End Loop");
    }

    logger->inf(LOG_EVENT, "-- Loop Done");
}

void removeKeyEvent(Event* evt) {
    EventMgr* mgr = getEventMgr();
    if (!mgr->keyEvts->nodeCount) {
        return;
    }

    deleteNode(mgr->keyEvts, evt->id);
}

void removeinputEvent(Event* evt) {
    EventMgr* mgr = getEventMgr();
    if (!mgr->inputEvts->nodeCount) {
        return;
    }

    deleteNode(mgr->inputEvts, evt->id);
}
