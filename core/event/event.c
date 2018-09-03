#include "event.h"
#include "../project/project.h"


KeyEvent* bindKeyEvent(char* label, SDL_Keycode key, bool (*fnc)(Event* evt)) {
    logger->inf(LOG_EVENT, "BINDING EVENT: %s", label);
    EventMgr* mgr = getEventMgr();

    Node* n = getNode(mgr->keyEvts, (int)key);
    if (n == NULL) {
        char name[150];
        memset(name, 0, 150);
        snprintf(name, 150, "Key:%s", SDL_GetKeyName(key));
        
        logger->inf(LOG_EVENT, "CREATING EVENT LIST: %s", name);
        n = addNodeV(mgr->keyEvts, name, initListMgr(), 1);
        n->id = key;
    }

    ListManager* keyEvents = (ListManager*) n->value;
    KeyEvent* evt = new(KeyEvent);
    evt->fnc = fnc;
    evt->enabled = true;
    evt->name = Str(label);
    evt->pressed = NULL;
    evt->released = NULL;

    logger->inf(LOG_EVENT, "ADD EVENT TO LIST");

    n = addNodeV(keyEvents, label, evt, 1);
    evt->id = n->id;

    return evt;
}

EventMgr* getEventMgr() {
    static EventMgr* mgr = NULL;

    if (mgr != NULL) {
        return mgr;
    }

    mgr = new(EventMgr);
    mgr->keyEvts = initListMgr();
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

void eventKey(int key, bool up) {
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

    Node* n = NULL;
    while ((n = listIterate(events, n)) != NULL) {
        if (n->value == NULL) {
            logger->war(LOG_EVENT, "EVENT IS NULL !!!!");
            continue;
        }

        bool doBreak = false;

        KeyEvent* evt = (KeyEvent*) n->value;
        if (!evt->enabled) {
            continue;
        }

        logger->dbg(LOG_EVENT, "++ Call EVENT: %s", n->name);

        if (!up && evt->pressed != NULL) {
            logger->dbg(LOG_EVENT, "Call Pressed");
            doBreak = !evt->pressed(evt);
        }
        else if(up && evt->released != NULL) {
            logger->dbg(LOG_EVENT, "Call Pressed");
            doBreak = !evt->released(evt);
        }

        if (evt->fnc != NULL) {
            logger->dbg(LOG_EVENT, "Call FNC");
            doBreak = !evt->fnc((Event*) evt) || !doBreak;
        }

        if (doBreak) {
            break;
        }
    }

    logger->inf(LOG_EVENT, "==== EVENT DONE ====");
}

void handleEvents() {
    SDL_Event event;
    int key = -1;

    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
                changeStatus(PRO_CLOSE);
				break;

            case SDL_KEYUP:
                key = event.key.keysym.sym;
                eventKey(key, true);
                break;

            case SDL_KEYDOWN:
				key = event.key.keysym.sym;
				eventKey(key, false);
				break;
        }

        //SDL_Delay(25);
    }
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
