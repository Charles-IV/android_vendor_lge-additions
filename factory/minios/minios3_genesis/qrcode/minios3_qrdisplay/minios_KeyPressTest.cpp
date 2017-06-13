#define LOG_NDEBUG 0
#define LOG_TAG "AAT"
#define LOG_TITLE "minios_KeyPressTest"

// SKIA INCLUDE
#include "MWWindow.h"
#include "MWWidgetList.h"
#include "MWEvent.h"
#include "MWScreen.h"

// AAT INCLUDE
#include "AatConfig.h"
#include "AatCommon.h"
#include <cutils/properties.h>
#include "SelfDiagnosis.h"
#include "KeyPress_Check.h"
#include "SelfDExt.h"
#include "minios_KeyPressTest.h"
#include <H2_Common.h>

android::Mutex DataRWL;
using namespace MW;

int exit_loop = 0;

void init_ButtonWigets()
{
    int type_key = 3; //default; 3 keys
    int kindOf4thKey = -1; // threre's no the forth key
    int key_x[4] = {0,};
    int cnt = 0;

    MW::Textbox  *tb        = NULL;

    // front key feature...
    type_key = NUM_KEY_FRONT;
    if(type_key == 4) {
        kindOf4thKey = TYPE_KEY_FRONT_FOURTH;
    }

//KEY LABEL
   //memo key
#if HAS_KEY_OPTIONAL
    tb = new MW::Textbox(&(ui_text_box_key_enum_t[LIST_KEY_OPTIONAL_0].r), &(ui_text_box_key_enum_t[LIST_KEY_OPTIONAL_0].t));
    ui_text_box_key_enum_t[LIST_KEY_OPTIONAL_0].index =  tb->add_MWWList();
    delete tb;
#endif

#if HAS_KEY_VOLUME
     tb = new MW::Textbox(&(ui_text_box_key_enum_t[LIST_KEY_UP_1].r), &(ui_text_box_key_enum_t[LIST_KEY_UP_1].t));
     ui_text_box_key_enum_t[LIST_KEY_UP_1].index =  tb->add_MWWList();
     delete tb;

     tb = new MW::Textbox(&(ui_text_box_key_enum_t[LIST_KEY_DOWN_1_0].r), &(ui_text_box_key_enum_t[LIST_KEY_DOWN_1_0].t));
     ui_text_box_key_enum_t[LIST_KEY_DOWN_1_0].index =  tb->add_MWWList();
     delete tb;

     tb = new MW::Textbox(&(ui_text_box_key_enum_t[LIST_KEY_DOWN_1_1].r), &(ui_text_box_key_enum_t[LIST_KEY_DOWN_1_1].t));
     ui_text_box_key_enum_t[LIST_KEY_DOWN_1_1].index =  tb->add_MWWList();
     delete tb;

     tb = new MW::Textbox(&(ui_text_box_key_enum_t[LIST_KEY_END].r), &(ui_text_box_key_enum_t[LIST_KEY_END].t));
     ui_text_box_key_enum_t[LIST_KEY_END].index =  tb->add_MWWList();
     delete tb;
#endif

#if TYPE_KEY_SOFTKEY
    for(int i = 0; i <= LIST_KEY_END; i++) {
        WidgetList::setText(ui_text_box_key_enum_t[i].index, ST_NULL, strlen(ST_NULL));
        WidgetList::setColor(ui_text_box_key_enum_t[i].index, _TDA_COLOR_BLACK, _TDA_COLOR_BLACK, 0);
    }
#if TYPE_KEY_HOME_DOME
    WidgetList::setText(ui_text_box_key_enum_t[LIST_KEY_HOME].index, ST_NULL, strlen(ST_NULL));
    WidgetList::setColor(ui_text_box_key_enum_t[LIST_KEY_HOME].index, _TDA_COLOR_BLACK, _TDA_COLOR_BLACK, 0);
#endif
    // if has only h/w home dome key ( ex. madai )
    if( type_key == 1 ) {
#if TYPE_KEY_HOME_DOME
        ui_text_box_key_enum_t[LIST_KEY_HOME].r.x = SCREEN_WIDTH*50/100-_KEY_W_*50/100;
        tb = new MW::Textbox(&(ui_text_box_key_enum_t[LIST_KEY_HOME].r), &(ui_text_box_key_enum_t[LIST_KEY_HOME].t));
        ui_text_box_key_enum_t[LIST_KEY_HOME].index =  tb->add_MWWList();
        delete tb;
#endif
    }
#else
    if(type_key == 3) {
        ALOGD("IF TYPE KEY 3!!!!!!");
        key_x[0] = 0; //{0, SCREEN_WIDTH*50/100-key_w*50/100, SCREEN_WIDTH-key_w };
        key_x[1] = SCREEN_WIDTH*50/100-_KEY_W_*50/100;
        key_x[2] = SCREEN_WIDTH-_KEY_W_;
        ALOGD("Key x(0): %d, Key x(1) : %d, Key x(2) : %d", key_x[0], key_x[1], key_x[2]);
    } else {
        ALOGD("IF TYPE KEY 4 !!!!!!");
        int space = (SCREEN_WIDTH-_KEY_W_*4)/3;
        key_x[0] = 0; //{0, SCREEN_WIDTH*0.5-key_w*0.5, SCREEN_WIDTH-key_w };
        key_x[1] = _KEY_W_+space;
        key_x[2] = _KEY_W_+space+_KEY_W_+space;
        key_x[3] = SCREEN_WIDTH-_KEY_W_;
    }

    for(int i=LIST_KEY_BACK; i< LIST_KEY_FOURTH; i++) {
        ui_text_box_key_enum_t[i].r.x = key_x[cnt++];
        tb = new MW::Textbox(&(ui_text_box_key_enum_t[i].r), &(ui_text_box_key_enum_t[i].t));
        ui_text_box_key_enum_t[i].index =  tb->add_MWWList();
        ALOGD("Lower Key Cnt: %d", cnt);
        delete tb;
    }

    switch(kindOf4thKey) {
        case 0:
            ALOGD("[%s] The type of 4thKey %d", LOG_TITLE, kindOf4thKey);
            ui_text_box_key_enum_t[LIST_KEY_FOURTH].r.x = key_x[3];
            //ui_text_box_key_enum_t[LIST_KEY_FOURTH].t.
            tb = new MW::Textbox(&(ui_text_box_key_enum_t[LIST_KEY_FOURTH].r), &(ui_text_box_key_enum_t[LIST_KEY_FOURTH].t));
            ui_text_box_key_enum_t[LIST_KEY_FOURTH].index =  tb->add_MWWList();
                // RECENT KEY
#if defined(AAT_y25c) || defined(AAT_y25)
            WidgetList::setText(ui_text_box_key_enum_t[LIST_KEY_FOURTH].index, "MENU", strlen("MENU"));
#else
            WidgetList::setText(ui_text_box_key_enum_t[LIST_KEY_FOURTH].index, "RECENT", strlen("RECENT"));
#endif
            delete tb;
            break;
        case 1:
            ALOGD("[%s] The type of 4thKey %d", LOG_TITLE, kindOf4thKey);
            ui_text_box_key_enum_t[LIST_KEY_FOURTH].r.x = key_x[3];
            tb = new MW::Textbox(&(ui_text_box_key_enum_t[LIST_KEY_FOURTH].r), &(ui_text_box_key_enum_t[LIST_KEY_FOURTH].t));
            ui_text_box_key_enum_t[LIST_KEY_FOURTH].index =  tb->add_MWWList();
            delete tb;
            break;
        default:
            ALOGD("[%s] no has 4thKey %d", LOG_TITLE, kindOf4thKey);
            break;
    }
#endif
}

void change_KeyButton_Color() {
#if HAS_KEY_VOLUME
    /* Power */
    if (get_num_button_clicked(SDLK_POWER) == 0) {
        WidgetList::setColor(ui_text_box_key_enum_t[LIST_KEY_END].index,_TDA_COLOR_BLACK,_TDA_COLOR_WHITE, 0);
    } else if (get_num_button_clicked(SDLK_POWER) == 1)  {
        WidgetList::setColor(ui_text_box_key_enum_t[LIST_KEY_END].index,_TDA_COLOR_BLACK,_TDA_COLOR_YELLOW, 0);
    } else {
        WidgetList::setColor(ui_text_box_key_enum_t[LIST_KEY_END].index,_TDA_COLOR_BLACK,_TDA_COLOR_GREEN, 0);
    }

    /* Volume Up */
    if (get_num_button_clicked(SDLK_VOLUMEUP) == 0) {
        WidgetList::setColor(ui_text_box_key_enum_t[LIST_KEY_UP_1].index,_TDA_COLOR_BLACK,_TDA_COLOR_WHITE, 0);
    } else if (get_num_button_clicked(SDLK_VOLUMEUP) == 1) {
        WidgetList::setColor(ui_text_box_key_enum_t[LIST_KEY_UP_1].index,_TDA_COLOR_BLACK,_TDA_COLOR_YELLOW, 0);
    } else {
        WidgetList::setColor(ui_text_box_key_enum_t[LIST_KEY_UP_1].index,_TDA_COLOR_BLACK,_TDA_COLOR_GREEN, 0);
    }

    /* Volume Down */
    if (get_num_button_clicked(SDLK_VOLUMEDOWN) == 0) {
        WidgetList::setColor(ui_text_box_key_enum_t[LIST_KEY_DOWN_1_0].index,_TDA_COLOR_BLACK,_TDA_COLOR_WHITE, 0);
        WidgetList::setColor(ui_text_box_key_enum_t[LIST_KEY_DOWN_1_1].index,_TDA_COLOR_BLACK,_TDA_COLOR_WHITE, 0);
    } else if (get_num_button_clicked(SDLK_VOLUMEDOWN) == 1) {
        WidgetList::setColor(ui_text_box_key_enum_t[LIST_KEY_DOWN_1_0].index,_TDA_COLOR_BLACK,_TDA_COLOR_YELLOW, 0);
        WidgetList::setColor(ui_text_box_key_enum_t[LIST_KEY_DOWN_1_1].index,_TDA_COLOR_BLACK,_TDA_COLOR_YELLOW, 0);
    } else {
        WidgetList::setColor(ui_text_box_key_enum_t[LIST_KEY_DOWN_1_0].index,_TDA_COLOR_BLACK,_TDA_COLOR_GREEN, 0);
        WidgetList::setColor(ui_text_box_key_enum_t[LIST_KEY_DOWN_1_1].index,_TDA_COLOR_BLACK,_TDA_COLOR_GREEN, 0);
    }
#endif
#if !TYPE_KEY_SOFTKEY
    if (get_num_button_clicked(SDLK_DSIM) > 0) {
        WidgetList::setColor(ui_text_box_key_enum_t[LIST_KEY_MENU].index,_TDA_COLOR_BLACK,_TDA_COLOR_GREEN, 0);
    }

    if (get_num_button_clicked(SDLK_RECENT) > 0) {
        WidgetList::setColor(ui_text_box_key_enum_t[LIST_KEY_FOURTH].index,_TDA_COLOR_BLACK,_TDA_COLOR_GREEN, 0);
    }

    if (get_num_button_clicked(SDLK_MENU) > 0) {
        WidgetList::setColor(ui_text_box_key_enum_t[LIST_KEY_MENU].index,_TDA_COLOR_BLACK,_TDA_COLOR_GREEN, 0);
    }

    if (get_num_button_clicked(SDLK_HOME) > 0) {
        WidgetList::setColor(ui_text_box_key_enum_t[LIST_KEY_HOME].index,_TDA_COLOR_BLACK,_TDA_COLOR_GREEN, 1);
    }

    if (get_num_button_clicked(SDLK_ESCAPE) > 0) {
        WidgetList::setColor(ui_text_box_key_enum_t[LIST_KEY_BACK].index,_TDA_COLOR_BLACK,_TDA_COLOR_GREEN, 0);
    }
#endif
}
