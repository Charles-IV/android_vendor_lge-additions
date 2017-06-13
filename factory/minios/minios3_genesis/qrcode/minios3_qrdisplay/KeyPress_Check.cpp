#include <stdio.h>
#include <stdlib.h>

#include "MWEvent.h"
#include "KeyPress_Check.h"
#include "AatConfig.h"
#include "AatCommon.h"

#define LOG_NDEBUG 0
#define LOG_TAG "AAT"
#define LOG_TITLE "minios_KeyPressTest"

using namespace MW;

typedef enum{
    OPTIONAL = 0,
    UP,
    DOWN,
    END,
    BACK,
    HOME,
    MENU,
    FOURTH,
    HOOK,
    KEY_ARR_ROW,
} key_array_order;

static int keyPressed[KEY_ARR_ROW] = {0,};    // the number of times a pressed event made on each keys
static int keyReleased[KEY_ARR_ROW] = {0,};   // the number of times a released event made on each keys

bool allKeyPressed_gt_twice()
{
#if HAS_KEY_VOLUME
    if (keyPressed[END] >= 2 && keyReleased[END] >= 2 && keyPressed[END] == keyReleased[END]
        && keyPressed[UP] >= 2 && keyReleased[UP] >= 2 && keyPressed[UP] == keyReleased[UP]
        && keyPressed[DOWN] >= 2 && keyReleased[DOWN] >= 2 && keyPressed[DOWN] == keyReleased[DOWN]) {
        return true;
    }
    return false;
#else
    return true;
#endif
}

int key_match_func(void* data) {

    kbd_data_t * k = (kbd_data_t *) data;
    return key_match_func(k->ksym);
}

int key_match_func(int key) {

    switch(key) {
        case SDLK_HOME:
            return HOME;
        case SDLK_ESCAPE:
            return BACK;
        case SDLK_MENU:
            return MENU;
        case SDLK_RECENT: // recent app key
            return FOURTH;
        case SDLK_DSIM: // dual sim switching key
            return FOURTH;
        case SDLK_POWER:
            return END;
        case SDLK_VOLUMEUP:
            return UP;
        case SDLK_VOLUMEDOWN:
            return DOWN;
#if HAS_KEY_OPTIONAL
        case SDLK_QMEMO: // quick memo
        case SDLK_HOT:
            return OPTIONAL;
#endif
        case SDLK_HOOK: // earjack hook key
            return HOOK;
        default:
            ALOGD("[%s]PRESS UNKNOWN KEY",LOG_TITLE);
            break;
    }
    return -1;
}

int get_num_button_clicked(int button)
{
    if(key_match_func(button) >= OPTIONAL && key_match_func(button) <= KEY_ARR_ROW) {
        return keyReleased[key_match_func(button)];
    } else {
        ALOGE("Button %d is invalid", key_match_func(button));
        return -1;
    }
}

void inc_keyPressed(void* data)
{
    int key = key_match_func(data);
    switch(key) {
        case HOME:
            keyPressed[HOME]++;
            break;
        case BACK:
            keyPressed[BACK]++;
            break;
        case MENU:
            keyPressed[MENU]++;
            break;
        case FOURTH: // recent app key
            keyPressed[FOURTH]++;
            break;
        case END:
            keyPressed[END]++;
            break;
        case UP:
            keyPressed[UP]++;
            break;
        case DOWN:
            keyPressed[DOWN]++;
            break;
#if HAS_KEY_OPTIONAL
        case OPTIONAL: // quick memo
            keyPressed[OPTIONAL]++;
            break;
#endif
        case HOOK: // earjack hook key
            keyPressed[HOOK]++;
            break;
    }

    ALOGD("Key %d Pressed %d", key, keyPressed[key]);
}

void inc_keyReleased(void* data)
{
    int key = key_match_func(data);
    switch(key) {
        case HOME:
            keyReleased[HOME]++;
            break;
        case BACK:
            keyReleased[BACK]++;
            break;
        case MENU:
            keyReleased[MENU]++;
            break;
        case FOURTH: // recent app key
            keyReleased[FOURTH]++;
            break;
        case END:
            keyReleased[END]++;
            break;
        case UP:
            keyReleased[UP]++;
            break;
        case DOWN:
            keyReleased[DOWN]++;
            break;
#if HAS_KEY_OPTIONAL
        case OPTIONAL: // quick memo
            keyReleased[OPTIONAL]++;
            break;
#endif
        case HOOK: // earjack hook key
            keyReleased[HOOK]++;
            break;
    }
    ALOGD("Key %d Released, %d", key, keyReleased[key]);
}
