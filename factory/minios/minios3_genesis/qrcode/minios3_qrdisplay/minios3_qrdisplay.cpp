/*
 * Copyright(c) 2015 by LG Electronics. Confidential and Proprietary
 * All Rights Reserved.
 */

#define LOG_NDEBUG 0
#define LOG_TAG "QR"
#define LOG_TITLE "minios3_qrdisplay"

#include <string.h>
#include <errno.h>
#include <ctype.h>
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <wchar.h>
#include <unistd.h>
#include <utils/Log.h>
#include <cutils/properties.h>

#include <hardware/hardware.h>
#include <hardware/lights.h>

#include <minios3_displayinfo.h>

// SKIA INCLUDE
#include "MWWindow.h"
#include "MWWidgetList.h"
#include "MWEvent.h"

#include "minios3_qrdisplay.h"
#include <lgftm/ftmitem.h>
#include "AatList.h"
#include "AatCommon.h"
#include "android_reboot.h"

#include "unified_sysfs.h"
#include "unified_sysfs_pm.h"

#include "GenesisCommon.h"

#include "SelfDiagnosis.h"
#include "SDCardDeviceTest.h"
#if PERFORM_QR_FLASH
#include "minios3_Camera.h"
#endif

#if PERFORM_H2_PRJ
#include "SelfDExt.h"
#include "factory_item.h"
#include "minios_KeyPressTest.h"
#include <H2_Common.h>
#include "KeyPress_Check.h"
#include "minios_property.h"

int not_mainlist = 0;
int battery_color = 0;

#if IS_TABLET
#define BATTERY_PASS_LEVEL 40
#else

#if IS_WATCH_PHONE
#define BATTERY_PASS_LEVEL 60
#else

#ifdef CHECK_BATTERY_EMBEDDED_DEVICE
#define BATTERY_PASS_LEVEL 36
#else

#if  SUPPORT_Z2_BATTERY
#define BATTERY_PASS_LEVEL 60
#else

#define BATTERY_PASS_LEVEL 20
#endif
#endif
#endif
#endif

#endif
#include "../../allautotest_skia/SelfDiagnosis/include/SelfDiagnosis.h"
#define BG_SELFD_RESULT_COLOR 0xffffffff
#define PID_SIZE 168
#define PASS 0
#define FAIL 1

#define MODE_PID 0
#define MODE_RESULT 1
#if IS_TABLET
#define MODE_TABLET 2
#define AAT_FAIL_ITEM "aat.fail.item"
#if PERFORM_AIO
#define MODE_TAIO 3
#include "T_Common.h"
int hdmi_result_check = -1;
#endif
char tmp[256] = {'\0',};
#endif

#define MODE_IMEI 4
static bool h2_mode = false;
static int h2_result = -1;
static int h2_retry_cnt = 0;
static bool need_to_update_qrcode_and_qrdisplay = false;

using namespace android;
using namespace MW;
MW::Key     *k_event = NULL;
MW::Textbox *tb      = NULL;

#if IS_TABLET
MW::Line    *line_1  = NULL;
MW::Line    *line_2  = NULL;
#endif
#if IS_WATCH_PHONE
int _BORDER_= 10;
#endif
MW::StaticText *fail_item    = NULL;
#if PERFORM_QR_FLASH
minios3_Camera *mCamera = NULL;
#endif

static bool exit_loop = false;
static bool skip_init = false;
int mode = MODE_PID;
int result = -1;    // PASS;

#if (IS_TABLET || IS_WATCH_PHONE)
static int aat_byte_check = -1;
#endif

#if HAS_OFF_SCENARIO
extern "C" uint32_t minios_power_off (int init, int battery, uint32_t ticks);
static int usb_on = 0;
#endif
int QR_SIZE = 0;
int battery = 0;

char* imei[3];
int imeicnt = 0;
MW::Button *b_exit    = NULL;
int b_exit_height = 0;

#if (IS_TABLET || IS_WATCH_PHONE)
int mUsbConnect = 0;
bool doesTurnOnLCD = true;
int timeout_lcd_turn_off = 0;
int status_lcd = 1;
#if SUPPORT_64BIT
static size_t ptr_led = 0;
#else
static int ptr_led = 0;
#endif
int light = LIGHT_INDEX_BACKLIGHT;
int mColor = 0;
int mMode = LIGHT_FLASH_NONE;
int mOnMS = 0;
int mOffMS = 0;
int brightnessMode = 0;
int bootup = 1;
double t = 0;
double t0 = 0;
int time_reset = 0;

int end_ble = 0;
int aio_result = 0;

struct Devices* mDevices;
struct light_device_t* get_device(hw_module_t* module, char const* name);
int check_battery();

int check_end_ble()
{
    char check_ble[2];
    memset(check_ble, NULL, sizeof(check_ble));

    device_read(PATH_MISC, LGFTM_TAIO_QR_STATUS*BLOCK_SIZE, 1, check_ble);
    ALOGD("[%s] %s, check_ble : %s", LOG_TITLE, __FUNCTION__, check_ble);

    return atoi(&check_ble[0]);
}

int check_aio_result()
{
    char aio_result[2];
    memset(aio_result, NULL, sizeof(aio_result));

    device_read(PATH_MISC, LGFTM_TAIO_RESULT*BLOCK_SIZE, 1, aio_result);
    ALOGD("[%s] %s, aio_result : %s", LOG_TITLE, __FUNCTION__, aio_result);

    return atoi(&aio_result[0]);
}

#define TOUCH_REBASE_TRIGGER "/sys/devices/virtual/input/lge_touch/rebase"
int check_aat_finished();

void rebase_trigger_qr_code_exit()
{
    char property_value[PROPERTY_VALUE_MAX];
    size_t num_bytes;
    char state[256] = {0};
    memset(state,0,sizeof(state));

    memset(property_value, 0, sizeof(property_value));
    property_get("selfd.result", property_value, "999");

    if(check_aat_finished() == 0 && strncmp(property_value, "pass", 4) == 0)
    {
        FILE *file = fopen(TOUCH_REBASE_TRIGGER, "r");

        if (file != NULL) {
            fseek(file,0,SEEK_END);
            num_bytes = (int)ftell(file);
            fseek(file,0,SEEK_SET);
            fread((void*)state, sizeof(char), num_bytes, file);
            fclose(file);

            ALOGD("[%s] Touch rebase triggered", LOG_TITLE);
        } else {
            ALOGE("[%s] fopen fail", LOG_TITLE);
        }
    }
}

struct light_device_t* get_device(hw_module_t* module, char const* name)
{
    int err;
    hw_device_t* device;
    err = module->methods->open(module, name, &device);
    if (err == 0) {
        ALOGD("[%s] get_device! name: %s", LOG_TITLE, name);
        return (struct light_device_t*)device;
    } else {
        //ALOGE("[%s] get_device fail! name: %s", LOG_TITLE, name);
        return NULL;
    }
}

void init_light_device()
{
    int err;
    hw_module_t* module;

    mDevices = (struct Devices*)malloc(sizeof(struct Devices));

    err = hw_get_module(LIGHTS_HARDWARE_MODULE_ID, (hw_module_t const**)&module);
    if (err == 0) {
        mDevices->lights[LIGHT_INDEX_BACKLIGHT]
            = get_device(module, LIGHT_ID_BACKLIGHT);
        mDevices->lights[LIGHT_INDEX_KEYBOARD]
            = get_device(module, LIGHT_ID_KEYBOARD);
        mDevices->lights[LIGHT_INDEX_BUTTONS]
            = get_device(module, LIGHT_ID_BUTTONS); // power key led
        mDevices->lights[LIGHT_INDEX_BATTERY]
            = get_device(module, LIGHT_ID_BATTERY);
        mDevices->lights[LIGHT_INDEX_NOTIFICATIONS]
            = get_device(module, LIGHT_ID_NOTIFICATIONS);
        mDevices->lights[LIGHT_INDEX_ATTENTION]
            = get_device(module, LIGHT_ID_ATTENTION);
        mDevices->lights[LIGHT_INDEX_BLUETOOTH]
            = get_device(module, LIGHT_ID_BLUETOOTH);
        mDevices->lights[LIGHT_INDEX_WIFI]
            = get_device(module, LIGHT_ID_WIFI);
    } else {
        ALOGE("[%s] init_led! hw_get_module, error!\n", LOG_TITLE);
        memset(mDevices, 0, sizeof(struct Devices));
    }
#if SUPPORT_64BIT
    ptr_led = (size_t)mDevices;
#else
    ptr_led = (int)mDevices;
#endif
}

void setLight_device(int ptr, int light, int colorARGB, int flashMode, int onMS, int offMS, int brightnessMode)
{
    struct light_state_t state;

    if (light < 0 || light >= LIGHT_COUNT || mDevices->lights[light] == NULL) {
        ALOGE("[%s] setLight_led, invaild light type", LOG_TITLE);
        return ;
    }

    memset(&state, 0, sizeof(struct light_state_t));
    state.color = colorARGB;
    state.flashMode = flashMode;
    state.flashOnMS = onMS;
    state.flashOffMS = offMS;
    state.brightnessMode = brightnessMode;

    {
        ALOGD("[%s] setLight!, state.color: %d", LOG_TITLE, state.color);
        mDevices->lights[light]->set_light(mDevices->lights[light], &state);
    }
#if HAS_LED_FRONTKEY
    if (colorARGB == MINIOS_AAT_BACKLIGHT_OFF) {
#if PATH_SYS_LED_BUTTON
        if(device_write(PATH_BUTTON_LED1, 0, 1, (void *)"0") < 0) {
            ALOGE("[%s] fail to turn off the backlight led1", LOG_TITLE);
        }
        if(device_write(PATH_BUTTON_LED2, 0, 1, (void *)"0") < 0) {
            ALOGE("[%s] fail to turn off the backlight led2", LOG_TITLE);
        }
#else
        if(device_write(PATH_BUTTON_LED0, 0, 1, (void *)"0") < 0) {
            ALOGE("[%s] fail to turn off the backlight led0", LOG_TITLE);
        }
#endif
    } else {
#if PATH_SYS_LED_BUTTON
        if(device_write(PATH_BUTTON_LED1, 0, 3, (void *)"255") < 0) {
            ALOGE("[%s] fail to turn on the backlight led1", LOG_TITLE);
        }
        if(device_write(PATH_BUTTON_LED2, 0, 3, (void *)"255") < 0) {
            ALOGE("[%s] fail to turn on the backlight led2", LOG_TITLE);
        }
#else
        if(device_write(PATH_BUTTON_LED0, 0, 3, (void *)"255") < 0) {
            ALOGE("[%s] fail to turn on the backlight led0", LOG_TITLE);
        }
#endif
    }
#endif
}

void check_tablet_off_scene()
{
    uint32_t ticks_local = 0;

    char property_value_sleepmode[PROPERTY_VALUE_MAX] = {0, };
    mUsbConnect = check_usb_connection();
    //ALOGD("[%s] status_lcd: %d, timeout_lcd_turn_off: %d, battery : %d, t : %f, t0 : %f", LOG_TITLE, status_lcd, timeout_lcd_turn_off, battery, t, t0);

    if(mUsbConnect == 0) {
        //ALOGD("[%s] not connected with usb\n", LOG_TITLE);

        ticks_local = GetTicks();
        t = (double) ticks_local/1000.0;

        if(t - t0 > 1) {
            t0 = t;
            if(status_lcd > 0) {
                timeout_lcd_turn_off += 1;
            }
        }


        if(time_reset == 0) {
            minios_power_off (1, battery, ticks_local);
            time_reset = 1;
        }

        if(timeout_lcd_turn_off > 58*10) { // lcd_turn_off after 10 minutes approximatively. (if check value changed 58 to 60, device`s lcd turned off 10m 18s.)
            if(status_lcd > 0) {
                mColor = MINIOS_AAT_BACKLIGHT_OFF;
                setLight_device(ptr_led, light, mColor, mMode, mOnMS, mOffMS, brightnessMode);

#if HAS_OFF_SCENARIO
                usb_on = 1;
#endif
                status_lcd = 0;
            }
#if HAS_OFF_SCENARIO
            else if(status_lcd == 0 && mUsbConnect == 0 && usb_on == 1) {
                uint32_t ret = minios_power_off (0, battery, ticks_local);

                if(ret == 1)
                    android_reboot(ANDROID_RB_POWEROFF, 0, 0);
            }

            else if((status_lcd == 0 || status_lcd == 1) && mUsbConnect == 1)
                usb_on = 0;
#endif
        } else {
            if(status_lcd == 1) {
                property_get("sys.lge.sleepmode.on", property_value_sleepmode, "0");
                if (property_value_sleepmode[0] == '0') {
                    mColor = MINIOS_AAT_BACKLIGHT_MAX;
                    setLight_device(ptr_led, light, mColor, mMode, mOnMS, mOffMS, brightnessMode);

                    status_lcd = 2;
                }
            }
        }
    } else {
        //ALOGE("[%s] connected with usb", LOG_TITLE);

#if HAS_OFF_SCENARIO
        if((status_lcd == 0) || (status_lcd == 2)  || (bootup == 1)) {
#else
        if((status_lcd == 0) || (bootup == 1)) {
#endif
            bootup = 0;
#if ((IS_TABLET || IS_WATCHPHONE) && HAS_OFF_SCENARIO)
            mColor = MINIOS_AAT_BACKLIGHT_DEFAULT_TABLET;
#else
            mColor = MINIOS_AAT_BACKLIGHT_MAX;
#endif
            setLight_device(ptr_led, light, mColor, mMode, mOnMS, mOffMS, brightnessMode);
            timeout_lcd_turn_off = 0;
            time_reset = 0;
            status_lcd = 1;
        }
    }

    if(check_battery() < 0) {
        ALOGE("[%s] fail! check battery\n", LOG_TITLE);
    }

    if (battery < 40) // 40 is temporarily value.
        exit_loop = true;
}

int check_aat_finished()
{
    int i = 0;
    int offset = 0;
    int id = 0;
    int aat_not_tested = -1;

    offset = LGFTM_DEVICETEST_RESULT*BLOCK_SIZE;
    id = LGFTM_DEVICETEST_RESULT;

    char aat_test[2];
    memset(aat_test, NULL, sizeof(aat_test));

    device_read(PATH_MISC, offset, 1, aat_test);
    ALOGD("[%s] check_aat_finished, aat_test : %s", LOG_TITLE, aat_test);

    if(aat_test[0] == '1') { //aat full is not all passed.
        aat_not_tested = 1;
    } else if (aat_test[0] == '0') { //aat full is all passed.
        aat_not_tested = 0;
    } else {
        aat_not_tested = 2;
    }

    ALOGD("[%s] check_aat_tested, aat_not_tested: %d", LOG_TITLE, aat_not_tested);
    return aat_not_tested;
}
#endif

int check_battery()
{
    size_t num_bytes;
    char str[10] = {0, };
#if HAS_OFF_SCENARIO
    char buff[4] ={0,0,0,0};
#endif
    int battery_len = 0;

    FILE *sysfs;
    sysfs = fopen(PATH_FUELVAL, "rt");
    if(sysfs == NULL) {
        ALOGE("[%s] check_battery fail! file open error\n", LOG_TITLE);
        return -1;
    }
    fseek(sysfs, 0, SEEK_END);
    num_bytes = (int)ftell(sysfs);
    fseek(sysfs, 0, SEEK_SET);
    fread((void*)str, sizeof(char), num_bytes, sysfs);
    fclose(sysfs);

#if HAS_OFF_SCENARIO
    buff[0] = str[0];
    buff[1] = str[1];
    buff[2] = str[2];

    if(buff[2] == 0x30) {
        buff[3] = 0;
    }
    else {
        if((buff[1] > 0x39) || (buff[1] < 0x30)) {
            buff[1] = 0;
            buff[2] = 0;
        }
        else {
            buff[2] = 0;
            buff[3] = 0;
        }
    }
    battery = atoi(buff);
#endif
    battery_len = strlen(str);
    if((str[battery_len-1] > 0x39) || (str[battery_len-1] < 0x30))
        str[battery_len-1] = 0;

    return 0;
}

#if 0 //save for old version
#define USB_CONNECTED "1"

int check_usb_connection()
{
    char buff[16];
    int ac = 0, usb = 0;

    if( unified_sysfs_read(UNI_SYSFS_PM_CHG_AC_ONLINE, buff) == 0 ) {
        if (strncmp(buff, USB_CONNECTED, strlen(USB_CONNECTED)) == 0) {
            ac = 1;
        }
    }

    memset(buff, 0x00, sizeof(buff));
    if( unified_sysfs_read(UNI_SYSFS_PM_CHG_USB_ONLINE, buff) == 0 ) {
        if (strncmp(buff, USB_CONNECTED, strlen(USB_CONNECTED)) == 0) {
            usb = 1;
        }
    }

    if(ac == 1 || usb == 1) {
        ALOGD("[%s] CHECK_USB_CONNECTION : TRUE", LOG_TITLE);
        return 1;
    }

    ALOGD("[%s] CHECK_USB_CONNECTION : FALSE", LOG_TITLE);
    return 0;
}
#endif
int onDubbleKey(void* data) {
    kbd_data_t * k = (kbd_data_t *) data;

    switch(k->ksym) {
        case SDLK_POWER:
            if(TYPE_KEY_SOFTKEY == SKIA_TRUE)
                exit_loop = true;
            break;
    }
    return true;
}

int onTripleKey(void* data) {
    kbd_data_t * k = (kbd_data_t *) data;

    switch(k->ksym) {
        case SDLK_POWER:
            if(TYPE_KEY_SOFTKEY == SKIA_TRUE)
                exit_loop = true;
            break;
    }

    return true;
}

int key_press_event(void* data) {
    kbd_data_t * k = (kbd_data_t *) data;
    ALOGD("[%s] Key Pressed, k->ksym : %d", LOG_TITLE, k->ksym);
    if (h2_mode) {
#if PERFORM_H2_PRJ
#ifdef KEYTEST_IN_QRDISPLAY
        inc_keyPressed(data);   // Key Test
#else
        switch(k->ksym) {
            case SDLK_HOME:
                exit_loop = true;
                break;
            default:
                break;
        }
#endif
#endif
    } else {
        switch(k->ksym) {
            case SDLK_HOME:
                exit_loop = true;
                break;
            default:
                break;
        }
    }
    return true;
}

int key_release_event(void* data) {
    kbd_data_t* k = (kbd_data_t *) data;
    ALOGD("[%s] Key Released, k->ksym : %d", LOG_TITLE, k->ksym);
    switch(k->ksym) {
        case SDLK_HOME:
            exit_loop = true;
            break;
        case SDLK_POWER:
        case SDLK_VOLUMEUP:
        case SDLK_VOLUMEDOWN:
#if (IS_TABLET || IS_WATCH_PHONE)
            timeout_lcd_turn_off = 0;
            time_reset = 0;
            if (status_lcd > 0) {
            } else {
                status_lcd = 1;
            }
#endif
            break;
        default:
            break;
    }
#if PERFORM_H2_PRJ
#ifdef KEYTEST_IN_QRDISPLAY
    inc_keyReleased(data);
#endif
#endif
    return true;
}

#if (IS_TABLET || IS_WATCH_PHONE)
int touchPressEvent(void* data) {
    mouse_data_t * m = (mouse_data_t *) data;

    if(status_lcd > 0) {
        status_lcd = 1;
        timeout_lcd_turn_off = 0;
        time_reset = 0;
    } else {
        if(m->x > (SCREEN_WIDTH*50/100-SCREEN_WIDTH*5/100) && m->x < SCREEN_WIDTH*50/100+SCREEN_WIDTH*5/100
            && m->y > SCREEN_HEIGHT*50/100-SCREEN_HEIGHT*5/100 && m->y < SCREEN_HEIGHT*50/100+SCREEN_HEIGHT*5/100) {
            status_lcd = 1;
            timeout_lcd_turn_off = 0;
            time_reset = 0;
        }
    }
    ALOGD("[%s] touch Pressed [%04d,%04d]", LOG_TITLE, m->x, m->y);
    return true;
}

int touchReleaseEvent(void* data) {
    mouse_data_t * m = (mouse_data_t *) data;
    ALOGD("[%s] touch Released [%04d,%04d]", LOG_TITLE, m->x, m->y);
    return true;
}
#endif

int read_pid(char *str)
{
    char pid[LGFTM_PID_SIZE];
    memset(pid, 0x00, sizeof(pid));
#if defined(MINIOS_PLATFORM_MTK)
    device_read(PATH_MISC2, (LGFTM_PID*BLOCK_SIZE_MISC2), sizeof(pid), pid);
    if(strncmp(pid, "00000000000000000", 17) == 0)
        pid[17] = 0x00 ;
    else
        sprintf(str, "%s", pid);
#else
    device_read(PATH_MISC, (LGFTM_PID*BLOCK_SIZE), sizeof(pid), pid);
    if(strlen(pid) == 0)
        strcpy(str, "                 ");
    else
        sprintf(str, "%s", pid);
#endif
    return 0;
}

int onClicked_Exit(void* data) {
    ALOGD("[%s] onClicked_Exit()", LOG_TITLE);
    exit_loop = true;

    if(b_exit)    delete b_exit;
    return 0;
}

void draw_widget(){
    WidgetList::init();
    char property_value[PROPERTY_VALUE_MAX];
    //title & pid
    for(int loop = 0; loop < LIST_LABEL_MAX; loop++) {
        MW::Label* l = new MW::Label(&(ui_label[loop].r), &(ui_label[loop].t));
        ui_label[loop].index = l->add_MWWList();
        delete l;
    }

    if(mode == MODE_PID) //mode = 0, not used for tablet
    {
        char pid[LGFTM_PID_SIZE];
        memset(pid, 0x00, sizeof(pid));
        read_pid(pid);

        WidgetList::setText(ui_label[LIST_LABEL_TITLE].index, "PID", strlen("PID"));
        WidgetList::setText(ui_label[LIST_LABEL_TEXT].index, pid, strlen(pid));
    }

#if IS_TABLET
    else if (mode == MODE_TABLET) //mode = 2, for tablet selfd+aat
    {
        tb = new MW::Textbox(&(ui_text_box[LIST_PASS_FAIL].r), &(ui_text_box[LIST_PASS_FAIL].t));
        ui_text_box[LIST_PASS_FAIL].index = tb->add_MWWList();
        delete tb;

        tb = new MW::Textbox(&(ui_text_box[LIST_TEST_ITEM].r), &(ui_text_box[LIST_TEST_ITEM].t));
        ui_text_box[LIST_TEST_ITEM].index = tb->add_MWWList();
        delete tb;

        if(aat_byte_check == 0) //if aat_result is passed.
        {

            memset(property_value, 0, sizeof(property_value));
            property_get(SELFD_PROPERTY_RESULT, property_value, "999");
            if(strncmp(property_value, "pass", 4) == 0) {

                WidgetList::setText(ui_text_box[LIST_PASS_FAIL].index, "PASS", strlen("PASS"));
                WidgetList::setColor(ui_text_box[LIST_PASS_FAIL].index,_TDA_COLOR_BLACK,_TDA_COLOR_BLUE, 0);

                ui_text_box[LIST_TEST_ITEM].r.x = 0;
                ui_text_box[LIST_TEST_ITEM].r.y = SCREEN_HEIGHT*65/100;
                ui_text_box[LIST_TEST_ITEM].r.w = SCREEN_WIDTH*50/100;
                ui_text_box[LIST_TEST_ITEM].r.h = SCREEN_HEIGHT*20/100;

                WidgetList::setText(ui_text_box[LIST_TEST_ITEM].index, "PID", strlen("PID"));
                WidgetList::setColor(ui_text_box[LIST_TEST_ITEM].index,_TDA_COLOR_BLACK,_TDA_COLOR_WHITE, 0);
            } else {

                WidgetList::setText(ui_text_box[LIST_PASS_FAIL].index, "FAIL", strlen("FAIL"));
                WidgetList::setColor(ui_text_box[LIST_PASS_FAIL].index,_TDA_COLOR_BLACK,_TDA_COLOR_RED, 0);

                WidgetList::setText(ui_text_box[LIST_TEST_ITEM].index, "SELF_D FAIL", strlen("SELF_D FAIL"));
                WidgetList::setColor(ui_text_box[LIST_TEST_ITEM].index,_TDA_COLOR_RED,_TDA_COLOR_WHITE, 0);
            }
        }
        else
        {
            char property_aat_fail[256];
            memset(property_aat_fail, 0, sizeof(property_aat_fail));
            property_get(AAT_FAIL_ITEM, property_aat_fail, "999");

            WidgetList::setText(ui_text_box[LIST_PASS_FAIL].index, "FAIL", strlen("FAIL"));
            WidgetList::setColor(ui_text_box[LIST_PASS_FAIL].index,_TDA_COLOR_BLACK,_TDA_COLOR_RED, 0);

            memset(property_value, 0, sizeof(property_value));
            property_get(SELFD_PROPERTY_RESULT, property_value, "999");
            if(strncmp(property_value, "fail", 4) == 0) {
                WidgetList::setText(ui_text_box[LIST_TEST_ITEM].index, "SELF_D FAIL", strlen("SELF_D FAIL"));
                WidgetList::setColor(ui_text_box[LIST_TEST_ITEM].index,_TDA_COLOR_RED,_TDA_COLOR_WHITE, 0);
            } else if (strncmp(property_aat_fail, "999", 3) != 0) {
//                WidgetList::setText(ui_text_box[LIST_TEST_ITEM].index, property_aat_fail, strlen(property_aat_fail));
//                WidgetList::setColor(ui_text_box[LIST_TEST_ITEM].index,_TDA_COLOR_RED,_TDA_COLOR_WHITE, 0);
                if(strncmp(property_value, "pass", 4) == 0) {
                    fail_item->setTextColor(_TDA_COLOR_RED);
                    fail_item->show();
                }

            }
        }
    }
#endif
#if PERFORM_AIO
    else if (mode == MODE_TAIO)
    {

        memset(property_value, 0, sizeof(property_value));
        property_get(SELFD_PROPERTY_RESULT, property_value, "999");
        if(strncmp(property_value, "pass", 4) == 0) {
            fail_item->hide();
        }

        tb = new MW::Textbox(&(ui_text_box[LIST_PASS_FAIL].r), &(ui_text_box[LIST_PASS_FAIL].t));
        ui_text_box[LIST_PASS_FAIL].index = tb->add_MWWList();
        delete tb;

        tb = new MW::Textbox(&(ui_text_box[LIST_TEST_ITEM].r), &(ui_text_box[LIST_TEST_ITEM].t));
        ui_text_box[LIST_TEST_ITEM].index = tb->add_MWWList();
        delete tb;

        ui_text_box[LIST_TEST_ITEM].r.x = 0;
        ui_text_box[LIST_TEST_ITEM].r.y = SCREEN_HEIGHT*65/100;
        ui_text_box[LIST_TEST_ITEM].r.w = SCREEN_WIDTH*50/100;
        ui_text_box[LIST_TEST_ITEM].r.h = SCREEN_HEIGHT*20/100;

        WidgetList::setText(ui_text_box[LIST_TEST_ITEM].index, "PID", strlen("PID"));
        WidgetList::setColor(ui_text_box[LIST_TEST_ITEM].index,_TDA_COLOR_BLACK,_TDA_COLOR_WHITE, 0);
        if (end_ble == 1) // aio result transmit to server successfully.
        {
            if (aio_result == 0) { // pass
                WidgetList::setText(ui_text_box[LIST_PASS_FAIL].index, "PASS", strlen("PASS"));
                WidgetList::setColor(ui_text_box[LIST_PASS_FAIL].index,_TDA_COLOR_WHITE,_TDA_COLOR_BLUE, 0);
            } else { // fail
                WidgetList::setText(ui_text_box[LIST_PASS_FAIL].index, "FAIL", strlen("FAIL"));
                WidgetList::setColor(ui_text_box[LIST_PASS_FAIL].index,_TDA_COLOR_WHITE,_TDA_COLOR_RED, 0);
            }
        } else {
            char property_ble[256];
            char aio_ems_status[256];
            memset(property_ble, 0, sizeof(property_ble));
            memset(aio_ems_status, 0, sizeof(aio_ems_status));
            property_get(AIO_QR_STATUS, property_ble, "999");
            property_get(AIO_EMS_STATUS, aio_ems_status, " ");

            WidgetList::setText(ui_text_box[LIST_PASS_FAIL].index, aio_ems_status, strlen(aio_ems_status));

            if(strncmp(aio_ems_status, "BLE Detected!", strlen("BLE Detected!")) == 0) {
                aat_byte_check = check_aat_finished();
#if PERFORM_RPMB_PROVISIONING
                static int rpmb_provisioning = 0;

                if(!rpmb_provisioning) {
                    ALOGD("[%s] TAIO rpmb_provisioning : %d", LOG_TITLE, aat_byte_check);

                    if(aat_byte_check == 0) {
                        int result;
                        ALOGD("[%s] TAIO do rpmb_provisioning", LOG_TITLE);
                        result = m2a_interface(1006/*RPMB_CMD*/);
                        if(result == 0) {
                            ALOGD("[%s] TAIO rpmb_provisioning: pass", LOG_TITLE);
                        }
                        else if(result == 1) {
                            ALOGD("[%s] TAIO rpmb_provisioning: fail", LOG_TITLE);
                        }
                        else {
                            ALOGD("[%s] TAIO rpmb_provisioning: not support", LOG_TITLE);
                        }
                        rpmb_provisioning = 1;
                    }
                }
#endif
            }

            if (strncmp(property_ble, "1", 1) == 0) {
                /*
                if (strncmp(aio_ems_status, "EMS OK", 6) == 0) {
                    WidgetList::setColor(ui_text_box[LIST_PASS_FAIL].index,_TDA_COLOR_BLUE,_TDA_COLOR_WHITE, 0);
                } else if (strncmp(aio_ems_status, "EMS Fail", 8) == 0) {
                    WidgetList::setColor(ui_text_box[LIST_PASS_FAIL].index,_TDA_COLOR_RED,_TDA_COLOR_WHITE, 0);
                } else {
                    WidgetList::setColor(ui_text_box[LIST_PASS_FAIL].index,_TDA_COLOR_BLACK,_TDA_COLOR_WHITE, 0);
                }
                */
                if (aat_byte_check == 0
#if HAS_HDMI
                        && hdmi_result_check == 1
#endif
                   ) {
                    WidgetList::setColor(ui_text_box[LIST_PASS_FAIL].index,_TDA_COLOR_WHITE,_TDA_COLOR_BLUE, 0);
                } else {
                    WidgetList::setColor(ui_text_box[LIST_PASS_FAIL].index,_TDA_COLOR_WHITE,_TDA_COLOR_RED, 0);
                }
            } else {
                WidgetList::setColor(ui_text_box[LIST_PASS_FAIL].index,_TDA_COLOR_YELLOW,_TDA_COLOR_YELLOW, 0);
            }
        }
    }
#endif
    else if (mode == MODE_IMEI) //mode = 4, for imei
    {
        int w = 0;
        int h = 0;
        int w_s = 0 ;
        int width = 0;
        int height = 0;
        int margin = 0;
        int temp_y = 0;
        Window::getwindowsize(&width, &height);
        w_s = width * SKIAW_SAMPLE;

        margin = (w_s / 200) * 4;
        h = w = ((w_s - (margin * 4)) / 3) / SKIAW_SAMPLE;
        margin = margin / SKIAW_SAMPLE;

        SKIA_Rect r_exit;
        r_exit.x = margin * 3 + w * 2;
        r_exit.y = height - h - margin;
        r_exit.w = w;
        r_exit.h = h;
        r_exit.color = MW_BLACK;
        b_exit_height = r_exit.y;
        SKIA_Text t_exit = {"EXIT", 0, _TDA_COLOR_WHITE,  SKIAW_CENTER    , FSIZE_BTN};
        b_exit = new MW::Button(&r_exit, &t_exit);
        b_exit->onReleaseCallback = onClicked_Exit;
        b_exit->setActive(true);

        char pid[LGFTM_PID_SIZE];
        memset(pid, 0x00, sizeof(pid));
        read_pid(pid);

        char pid_temp[1024] = {0, };
        sprintf(pid_temp, "PID : %s", pid);

        tb = new MW::Textbox(&(ui_text_box_imei_pid[LIST_TEST_ITEM].r), &(ui_text_box_imei_pid[LIST_TEST_ITEM].t));
        ui_text_box_imei_pid[LIST_TEST_ITEM].index = tb->add_MWWList();
        delete tb;

        if (imeicnt == 1) {
            tb = new MW::Textbox(&(ui_text_box_imei_pid[LIST_IMEI1].r), &(ui_text_box_imei_pid[LIST_IMEI1].t));
            ui_text_box_imei_pid[LIST_IMEI1].index = tb->add_MWWList();
            delete tb;

            ui_text_box_imei_pid[LIST_IMEI1].r.x = SCREEN_WIDTH*10/100;
            ui_text_box_imei_pid[LIST_IMEI1].r.y = (SCREEN_HEIGHT*50/100) + (QR_SIZE*30/100) + QR_SIZE + QR_SIZE*50/100;

            char imei_temp[1024] = {0,};
#if !SUPPORT_RIL
            sprintf(imei_temp, "CSN : %s", imei[0]);
#else
            sprintf(imei_temp, "IMEI : %s", imei[0]);
#endif
            WidgetList::setText(ui_text_box_imei_pid[LIST_IMEI1].index, imei_temp, strlen(imei_temp));
            WidgetList::setColor(ui_text_box_imei_pid[LIST_IMEI1].index,_TDA_COLOR_BLACK,_TDA_COLOR_WHITE, 0);
            WidgetList::setText(ui_text_box_imei_pid[LIST_TEST_ITEM].index, pid_temp, strlen(pid_temp));
            WidgetList::setColor(ui_text_box_imei_pid[LIST_TEST_ITEM].index,_TDA_COLOR_BLACK,_TDA_COLOR_WHITE, 0);
        }
        else if (imeicnt == 2) {
            tb = new MW::Textbox(&(ui_text_box_imei_pid[LIST_IMEI1].r), &(ui_text_box_imei_pid[LIST_IMEI1].t));
            ui_text_box_imei_pid[LIST_IMEI1].index = tb->add_MWWList();
            delete tb;

            tb = new MW::Textbox(&(ui_text_box_imei_pid[LIST_IMEI2].r), &(ui_text_box_imei_pid[LIST_IMEI2].t));
            ui_text_box_imei_pid[LIST_IMEI2].index = tb->add_MWWList();
            delete tb;

            temp_y = (b_exit_height - (SCREEN_HEIGHT*50/100+ (QR_SIZE*20/100)))/2;

            ui_text_box_imei_pid[LIST_IMEI1].r.x = SCREEN_WIDTH*30/100;
            ui_text_box_imei_pid[LIST_IMEI1].r.y = (SCREEN_HEIGHT*50/100) + (QR_SIZE*20/100) + (QR_SIZE*50/100);

            ui_text_box_imei_pid[LIST_IMEI2].r.x = SCREEN_WIDTH*30/100;
            ui_text_box_imei_pid[LIST_IMEI2].r.y = (SCREEN_HEIGHT*50/100 + (QR_SIZE*20/100) + temp_y) + (QR_SIZE*50/100);

            char imei_temp[1024] = {0,};
            sprintf(imei_temp, "IMEI1 : %s", imei[0]);
            WidgetList::setText(ui_text_box_imei_pid[LIST_IMEI1].index, imei_temp, strlen(imei_temp));

            memset(imei_temp, NULL, sizeof(imei_temp));
            sprintf(imei_temp, "IMEI2 : %s", imei[1]);
            WidgetList::setText(ui_text_box_imei_pid[LIST_IMEI2].index, imei_temp, strlen(imei_temp));

            WidgetList::setColor(ui_text_box_imei_pid[LIST_IMEI1].index,_TDA_COLOR_BLACK,_TDA_COLOR_WHITE, 0);
            WidgetList::setText(ui_text_box_imei_pid[LIST_TEST_ITEM].index, pid_temp, strlen(pid_temp));
            WidgetList::setColor(ui_text_box_imei_pid[LIST_TEST_ITEM].index,_TDA_COLOR_BLACK,_TDA_COLOR_WHITE, 0);

        }
        else if (imeicnt == 3) {
            tb = new MW::Textbox(&(ui_text_box_imei_pid[LIST_IMEI1].r), &(ui_text_box_imei_pid[LIST_IMEI1].t));
            ui_text_box_imei_pid[LIST_IMEI1].index = tb->add_MWWList();
            delete tb;

            tb = new MW::Textbox(&(ui_text_box_imei_pid[LIST_IMEI2].r), &(ui_text_box_imei_pid[LIST_IMEI2].t));
            ui_text_box_imei_pid[LIST_IMEI2].index = tb->add_MWWList();
            delete tb;

            tb = new MW::Textbox(&(ui_text_box_imei_pid[LIST_IMEI3].r), &(ui_text_box_imei_pid[LIST_IMEI3].t));
            ui_text_box_imei_pid[LIST_IMEI3].index = tb->add_MWWList();
            delete tb;

            temp_y = (b_exit_height - (SCREEN_HEIGHT*50/100+ (QR_SIZE*20/100)))/3;

            ui_text_box_imei_pid[LIST_IMEI1].r.x = SCREEN_WIDTH*30/100;
            ui_text_box_imei_pid[LIST_IMEI1].r.y = (SCREEN_HEIGHT*50/100 + (QR_SIZE*20/100)*2) + (QR_SIZE*50/100);

            ui_text_box_imei_pid[LIST_IMEI2].r.x = SCREEN_WIDTH*30/100;
            ui_text_box_imei_pid[LIST_IMEI2].r.y = (SCREEN_HEIGHT*50/100 + (QR_SIZE*20/100) + temp_y) + (QR_SIZE*50/100);

            ui_text_box_imei_pid[LIST_IMEI3].r.x = SCREEN_WIDTH*30/100;
            ui_text_box_imei_pid[LIST_IMEI3].r.y = (SCREEN_HEIGHT*50/100 + (QR_SIZE*20/100) + temp_y*2) + (QR_SIZE*50/100);

            char imei_temp[1024] = {0,};
            sprintf(imei_temp, "IMEI1 : %s", imei[0]);
            WidgetList::setText(ui_text_box_imei_pid[LIST_IMEI1].index, imei_temp, strlen(imei_temp));

            memset(imei_temp, NULL, sizeof(imei_temp));
            sprintf(imei_temp, "IMEI2 : %s", imei[1]);
            WidgetList::setText(ui_text_box_imei_pid[LIST_IMEI2].index, imei_temp, strlen(imei_temp));

            memset(imei_temp, NULL, sizeof(imei_temp));
            sprintf(imei_temp, "IMEI3 : %s", imei[2]);
            WidgetList::setText(ui_text_box_imei_pid[LIST_IMEI3].index, imei_temp, strlen(imei_temp));

            WidgetList::setColor(ui_text_box_imei_pid[LIST_IMEI1].index,_TDA_COLOR_BLACK,_TDA_COLOR_WHITE, 0);
            WidgetList::setText(ui_text_box_imei_pid[LIST_TEST_ITEM].index, pid_temp, strlen(pid_temp));
            WidgetList::setColor(ui_text_box_imei_pid[LIST_TEST_ITEM].index,_TDA_COLOR_BLACK,_TDA_COLOR_WHITE, 0);

        }
    }
    else //mode = 1, for selfd.
    {
#if IS_TABLET
        tb = new MW::Textbox(&(ui_text_box[LIST_PASS_FAIL].r), &(ui_text_box[LIST_PASS_FAIL].t));
        ui_text_box[LIST_PASS_FAIL].index = tb->add_MWWList();
        delete tb;
        tb = new MW::Textbox(&(ui_text_box[LIST_TEST_ITEM].r), &(ui_text_box[LIST_TEST_ITEM].t));
        ui_text_box[LIST_TEST_ITEM].index = tb->add_MWWList();
        delete tb;
#else
        float x = 0.0;
        float y = 0.0;
        int32_t multiplier = Window::getMura(&x, &y, 15.0);
        ui_text_box.r.x = multiplier;
        ui_text_box.r.y = SCREEN_HEIGHT * 50/100;
        ui_text_box.r.w = SCREEN_WIDTH-(multiplier*2);
        ui_text_box.r.h = ui_text_box.r.y - multiplier;
#if (PERFORM_GENESIS_WIRELESS_DL && PERFORM_H2_PRJ)
        ui_text_box_desc.r.x = multiplier;
        ui_text_box_desc.r.y = ui_text_box.r.y + (ui_text_box.r.h / 2);
        ui_text_box_desc.r.w = SCREEN_WIDTH-(multiplier*2);
        ui_text_box_desc.r.h = (ui_text_box.r.h / 2);
#endif
        tb = new MW::Textbox(&(ui_text_box.r), &(ui_text_box.t));
        ui_text_box.index = tb->add_MWWList();
        delete tb;
#if (PERFORM_GENESIS_WIRELESS_DL && PERFORM_H2_PRJ)
        MW::Label* l = new MW::Label(&(ui_text_box_desc.r), &(ui_text_box_desc.t));
        ui_text_box_desc.index = l->add_MWWList();
        delete l;
#endif
#endif

#if PERFORM_H2_PRJ
#ifdef KEYTEST_IN_QRDISPLAY
        /* KeyPressTest */
        if (h2_mode) {
            init_ButtonWigets();
            change_KeyButton_Color();
        }
#endif

        if (h2_mode) {
#ifdef H2_RETRY_CNT
            ALOGE("[%s] H2.0 RETRY_CNT DISPLAY", LOG_TITLE);
            MW::Label* l = new MW::Label(&(ui_text_box_h2_retry.r), &(ui_text_box_h2_retry.t));
            ui_text_box_h2_retry.index = l->add_MWWList();
            delete l;
            char str[16] = {0,};

            if (h2_retry_cnt >= 0) {
                ALOGE("[%s] H2.0 RETRY_CNT : %d", LOG_TITLE, h2_retry_cnt);
                memset(str, 0, sizeof(str));
                sprintf(str, "%d", h2_retry_cnt);
                WidgetList::setText(ui_text_box_h2_retry.index, str, strlen(str));
                WidgetList::setColor(ui_text_box_h2_retry.index, MW_BLACK, 0, 0);
                WidgetList::show(ui_text_box_h2_retry.index);
            }
#endif

#ifdef H2_DC_JIG_NUM
            MW::Label* l = new MW::Label(&(ui_text_box_h2_dc_num.r), &(ui_text_box_h2_dc_num.t));
            ui_text_box_h2_dc_num.index = l->add_MWWList();
            delete l;

            char dc_num[PROPERTY_VALUE_MAX];
            memset(dc_num, 0x0, PROPERTY_VALUE_MAX);
            property_get("dc.jignum", dc_num, "0");

            if (dc_num[0] >= '1' && dc_num[0] <= '4') {
                ALOGE("[%s] H2.0 DC_NUM : %c", LOG_TITLE, dc_num[0]);
                memset(str, 0, sizeof(str));
                sprintf(str, "%s: %c", "DC_JIG_NUM", dc_num[0]);
                WidgetList::setText(ui_text_box_h2_dc_num.index, str, strlen(str));
                WidgetList::setColor(ui_text_box_h2_dc_num.index, MW_BLACK, 0, 0);
                WidgetList::show(ui_text_box_h2_dc_num.index);
            } else {
                ALOGE("[%s] H2.0 DC_NUM : Not Tested", LOG_TITLE);
                memset(str, 0, sizeof(str));
                sprintf(str, "%s", "DC_JIG_NUM: NT");
                WidgetList::setText(ui_text_box_h2_dc_num.index, str, strlen(str));
                WidgetList::setColor(ui_text_box_h2_dc_num.index, MW_BLACK, 0, 0);
                WidgetList::show(ui_text_box_h2_dc_num.index);
            }
#endif

            ALOGE("[%s] H2.0 RESULT DISPLAY", LOG_TITLE);
        }
#endif

        if(result == PASS)
        {
#if IS_TABLET
            WidgetList::setText(ui_text_box[LIST_PASS_FAIL].index, "PASS", strlen("PASS"));
            WidgetList::setColor(ui_text_box[LIST_PASS_FAIL].index,_TDA_COLOR_BLACK,_TDA_COLOR_BLUE, 0);
#else
#if PERFORM_H2_PRJ
            if(h2_mode) {
                WidgetList::setText(ui_text_box.index, "PASS", strlen("PASS"));
                WidgetList::setColor(ui_text_box.index,_TDA_COLOR_BLACK,_TDA_COLOR_GREEN, 0);
            } else
#endif
            {
                WidgetList::setText(ui_text_box.index, "PASS", strlen("PASS"));
                WidgetList::setColor(ui_text_box.index,_TDA_COLOR_BLACK,_TDA_COLOR_BLUE, 0);
            }
#endif
        }
        else
        {
#if 0//PERFORM_H2_PRJ  //for next version
            if(h2_mode) {
                char ble_msg[PROPERTY_VALUE_MAX] = {0,};
                property_get("bluetooth.ble.msg", ble_msg, "");
                int h2_result = check_selfd_E_result();
                if(strcmp(ble_msg, "EMSEN:PASS") == 0 && h2_result == 0) {
                    result = PASS;
                }
            }
#endif
#if IS_TABLET
            WidgetList::setText(ui_text_box[LIST_PASS_FAIL].index, "FAIL", strlen("FAIL"));
            WidgetList::setColor(ui_text_box[LIST_PASS_FAIL].index,_TDA_COLOR_BLACK,_TDA_COLOR_RED, 0);
            WidgetList::setText(ui_text_box[LIST_TEST_ITEM].index, "SELF_D FAIL", strlen("SELF_D FAIL"));
            WidgetList::setColor(ui_text_box[LIST_TEST_ITEM].index,_TDA_COLOR_RED,_TDA_COLOR_WHITE, 0);
#else
            WidgetList::setText(ui_text_box.index, "FAIL", strlen("FAIL"));
            WidgetList::setColor(ui_text_box.index,_TDA_COLOR_BLACK,_TDA_COLOR_RED, 0);
#endif
        }
#if (PERFORM_GENESIS_WIRELESS_DL && PERFORM_H2_PRJ)
        char ble_result[PROPERTY_VALUE_MAX];
        char ble_msg[PROPERTY_VALUE_MAX];
        char ble_rssi[PROPERTY_VALUE_MAX];
        char str[LGFTM_WIRELESS_FACTORY_SIZE];
        memset(str, 0x00, LGFTM_WIRELESS_FACTORY_SIZE);

        property_get("bluetooth.ble.result", ble_result, "");
        property_get("bluetooth.ble.msg", ble_msg, "");
        property_get("bluetooth.ble.rssi", ble_rssi, "");

        if(ble_result[0] != '\0')
            sprintf(str, "%s(%s) %s", ble_result, ble_msg, ble_rssi);
        else
            sprintf(str, "no_init");

#if PERFORM_H2_PRJ
        {
            char setems_state[SIZE_PROPERTY] = {0,};

            property_get(SELFD_WEBSERVER_STATE, setems_state, "0"/*ing*/);
            if (strcmp(setems_state, "1") == 0) { // fail
                ALOGD("[%s] Set WIFI result to FAIL because transmission of H2 result is failed!(%s)", LOG_TITLE, setems_state);
                saveH2IntegratedResultForcely(NUM_AAT_QR_RESULT_WIFI, '0');
                need_to_update_qrcode_and_qrdisplay = true;
            }
        }
#endif

        WidgetList::setText(ui_text_box_desc.index, str, strlen(str));
        WidgetList::setColor(ui_text_box_desc.index, MW_BLACK, 0, 0);
#endif
    }

    //qr code
    if(mode == MODE_PID)
    {
        ui_image[mode].r.x = SCREEN_WIDTH*50/100 - PID_SIZE*50/100;
        ui_image[mode].r.y = SCREEN_HEIGHT*50/100 - PID_SIZE*50/100;
    }
#if IS_TABLET
    else if(mode == MODE_TABLET)
    {
        if(aat_byte_check == 0) { //if aat_result is passed.
            //ALOGD("[%s] aat_result is passed & pid qr display", LOG_TITLE);
            memset(property_value, 0, sizeof(property_value));
            property_get(SELFD_PROPERTY_RESULT, property_value, "999");
            if(strncmp(property_value, "pass", 4) == 0) {
                ui_image[2].r.x = 20;
                ui_image[2].r.y = 20;
                ui_image[0].r.x = SCREEN_WIDTH - PID_SIZE - 20;
                ui_image[0].r.y = SCREEN_HEIGHT - PID_SIZE - 20;
            } else {
                ui_image[2].r.x = 20;
                ui_image[2].r.y = 20;
            }

        } else {
            ui_image[mode].r.x = 20;
            ui_image[mode].r.y = 20;
        }
    }
#endif
#if PERFORM_AIO
    else if(mode == MODE_TAIO)
    {
        ui_image[3].r.x = 20;
        ui_image[3].r.y = 20;
        ui_image[0].r.x = SCREEN_WIDTH - PID_SIZE - 20;
        ui_image[0].r.y = SCREEN_HEIGHT - PID_SIZE - 20;
    }
#endif
    else if(mode == MODE_IMEI)
    {
        int temp_y = 0;

        ui_image[0].r.x = SCREEN_WIDTH*10/100;
        ui_image[0].r.y = SCREEN_HEIGHT*20/100;
        ui_text_box_imei_pid[LIST_TEST_ITEM].r.y = SCREEN_HEIGHT*20/100 + PID_SIZE;

        if (imeicnt == 1) {
            ui_image[LIST_IMAGE_IMEI1].r.x = SCREEN_WIDTH*10/100;
            ui_image[LIST_IMAGE_IMEI1].r.y = (SCREEN_HEIGHT*50/100) + (QR_SIZE*30/100);
        }else if (imeicnt == 2) {
            temp_y = (b_exit_height - SCREEN_HEIGHT*50/100 + (QR_SIZE*20/100))/2;
            ui_image[LIST_IMAGE_IMEI1].r.x = SCREEN_WIDTH*10/100;
            ui_image[LIST_IMAGE_IMEI1].r.y = SCREEN_HEIGHT*50/100 + (QR_SIZE*20/100);

            ui_image[LIST_IMAGE_IMEI2].r.x = SCREEN_WIDTH*10/100;
            ui_image[LIST_IMAGE_IMEI2].r.y = SCREEN_HEIGHT*50/100 + (QR_SIZE*20/100) + temp_y;
        }else if (imeicnt == 3) {
            temp_y = (b_exit_height - (SCREEN_HEIGHT*50/100 + (QR_SIZE*20/100)))/3;

            ui_image[LIST_IMAGE_IMEI1].r.x = SCREEN_WIDTH*10/100;
            ui_image[LIST_IMAGE_IMEI1].r.y = SCREEN_HEIGHT*50/100 + (QR_SIZE*20/100);

            ui_image[LIST_IMAGE_IMEI2].r.x = SCREEN_WIDTH*10/100;
            ui_image[LIST_IMAGE_IMEI2].r.y = SCREEN_HEIGHT*50/100 + (QR_SIZE*20/100) + temp_y;

            ui_image[LIST_IMAGE_IMEI3].r.x = SCREEN_WIDTH*10/100;
            ui_image[LIST_IMAGE_IMEI3].r.y = SCREEN_HEIGHT*50/100 + (QR_SIZE*20/100) + temp_y*2;

        }

    }
    else
    {
#ifdef QR_POSITION
        //QR_POSITION 0 (Default) : left, top
        int qr_x = 20;
        int qr_y = 20;

        switch(QR_POSITION)
        {
            //QR_POSITION 1 : right, top
            case 1:
                qr_x = SCREEN_WIDTH - QR_SIZE - 20;
                break;
            //QR_POSITION 2 : left, bottom
            case 2:
                qr_y = SCREEN_HEIGHT/2 - QR_SIZE - 20;
                break;
            //QR_POSITION 3 : right, bottom
            case 3:
                qr_x = SCREEN_WIDTH - QR_SIZE - 20;
                qr_y = SCREEN_HEIGHT/2 - QR_SIZE - 20;
                break;
            //QR_POSITION 4 : center
            case 4:
                qr_x = SCREEN_WIDTH*50/100 - QR_SIZE*50/100;
                qr_y = SCREEN_HEIGHT*50/100/2 - QR_SIZE*50/100;
                break;
            case 5:
                qr_x = SCREEN_WIDTH*50/100 - QR_SIZE*50/100;
                qr_y = SCREEN_HEIGHT*50/100 - QR_SIZE*50/100 - QR_SIZE*50/100/2;
                break;
        }

        ui_image[mode].r.x = qr_x;
        ui_image[mode].r.y = qr_y;
#else
        //QR_POSITION DEFAULT : left, top
        ui_image[mode].r.x = 20;
        ui_image[mode].r.y = 20;
#endif
    }

#if IS_WATCH_PHONE
    ui_image[mode].r.x = SCREEN_WIDTH*50/100 - QR_SIZE*50/100;
    ui_image[mode].r.y = SCREEN_HEIGHT*50/100 - QR_SIZE*50/100;
    ui_image[LIST_IMAGE_BG].r.x = ui_image[mode].r.x - _BORDER_;// + VIRTUAL_LCD_W/2;
    ui_image[LIST_IMAGE_BG].r.y = ui_image[mode].r.y - _BORDER_;// + VIRTUAL_LCD_H/2;
    ui_image[LIST_IMAGE_BG].r.w = QR_SIZE + (_BORDER_ * 2);
    ui_image[LIST_IMAGE_BG].r.h = QR_SIZE + (_BORDER_ * 2);


    tb = new MW::Textbox(&(ui_image[LIST_IMAGE_BG].r), &(ui_image[LIST_IMAGE_BG].t));
    tb->setBgColor(_TDA_COLOR_WHITE);
    ui_image[LIST_IMAGE_BG].index = tb->add_MWWList();
    WidgetList::show(ui_image[LIST_IMAGE_BG].index);
    delete tb;
#if HAS_SENSOR_PPG
    int ppg_result = -1;

    memset(property_value, 0, sizeof(property_value));
    property_get(SELFD_PROPERTY_35, property_value, "0");
    if (strncmp(property_value, SELFD_FAIL, strlen(SELFD_FAIL)) == 0)
        ppg_result = 0;    //fail
    else
        ppg_result = 1;

    if(ppg_result == 0){
        ui_ppg_text_box.r.x = ui_image[LIST_IMAGE_BG].r.x + _GAP_;
        ui_ppg_text_box.r.h = FSIZE_TITLE_AD * 2;
        ui_ppg_text_box.r.y = ui_image[LIST_IMAGE_BG].r.y - ui_text_box.r.h;
        ui_ppg_text_box.r.w = QR_SIZE;

        ui_ppg_text_box.t.color = _TDA_COLOR_BLUE;
        ui_ppg_text_box.t.font_size = ui_ppg_text_box.r.h;        //FSIZE_TITLE_AD;
        tb = new MW::Textbox(&(ui_ppg_text_box.r), &(ui_ppg_text_box.t));
        ui_ppg_text_box.index = tb->add_MWWList();
        delete tb;

        WidgetList::setText(ui_ppg_text_box.index, "PPG is Failed", strlen("PPG is Failed"));
        WidgetList::show(ui_ppg_text_box.index);
    }
#endif
#endif

#if (PERFORM_SDCARD_COUNT > 0)
    int gap_sdcard_count = 20; //gap between result and sdcard.

    ui_text_sdcard_count.r.x = SCREEN_WIDTH/4;
    ui_text_sdcard_count.r.h = FSIZE_TITLE_AD;
    ui_text_sdcard_count.r.w = SCREEN_WIDTH/2;

#if IS_TABLET
    ui_text_sdcard_count.r.y = ui_text_box[LIST_TEST_ITEM].r.y + ui_text_box[LIST_TEST_ITEM].r.h + gap_sdcard_count;
#else
    ui_text_sdcard_count.r.y = ui_text_box.r.y + ui_text_box.r.h + gap_sdcard_count;
#endif

    ui_text_sdcard_count.t.color = _TDA_COLOR_RED;
    ui_text_sdcard_count.t.font_size = ui_text_sdcard_count.r.h;
    MW::Textbox *tb_sdcard_count = new MW::Textbox(&(ui_text_sdcard_count.r), &(ui_text_sdcard_count.t));
    ui_text_sdcard_count.index = tb_sdcard_count->add_MWWList();
    delete tb_sdcard_count;

    int n_sdcount;
    char sz_sdcount[PROPERTY_VALUE_MAX];
    memset(sz_sdcount, 0, sizeof(sz_sdcount));

    n_sdcount = check_sdcard_count(false);
    if(n_sdcount >= PERFORM_SDCARD_COUNT){
        sprintf(sz_sdcount, "<WARNING> SD : %d", n_sdcount);
        WidgetList::setText(ui_text_sdcard_count.index, sz_sdcount, strlen(sz_sdcount));
        WidgetList::show(ui_text_sdcard_count.index);
    }
#endif

#if PERFORM_AIO
    if (mode == MODE_TAIO) {
        ui_image[0].t.string_len = strlen(ui_image[0].t.string);
        MW::Image* i = new MW::Image(&(ui_image[0].r), &(ui_image[0].t));
        ui_image[0].index =  i->add_MWWList();
        WidgetList::show(ui_image[0].index);
        delete i;
        ui_image[3].t.string_len = strlen(ui_image[3].t.string);
        MW::Image* j = new MW::Image(&(ui_image[3].r), &(ui_image[3].t));
        ui_image[3].index =  j->add_MWWList();
        WidgetList::show(ui_image[3].index);
        delete j;
    } else {
#endif
        ui_image[mode].t.string_len = strlen(ui_image[mode].t.string);
        MW::Image* i = new MW::Image(&(ui_image[mode].r), &(ui_image[mode].t));
        ui_image[mode].index =  i->add_MWWList();
        WidgetList::show(ui_image[mode].index);
        delete i;
#if PERFORM_AIO
    }
#endif

#if IS_TABLET
    if (mode == MODE_TABLET && aat_byte_check == 0) {
        property_get(SELFD_PROPERTY_RESULT, property_value, "999");
        if(strncmp(property_value, "pass", 4) == 0) {
            ui_image[0].t.string_len = strlen(ui_image[0].t.string);
            MW::Image* i = new MW::Image(&(ui_image[0].r), &(ui_image[0].t));
            ui_image[0].index =  i->add_MWWList();
            WidgetList::show(ui_image[0].index);
            delete i;
        }
    }
#endif

    if (mode == MODE_IMEI) {
        ui_image[0].t.string_len = strlen(ui_image[0].t.string);
        MW::Image* i = new MW::Image(&(ui_image[0].r), &(ui_image[0].t));
        ui_image[0].index =  i->add_MWWList();
        WidgetList::show(ui_image[0].index);
        delete i;

        ui_image[LIST_IMAGE_IMEI1].t.string_len = strlen(ui_image[LIST_IMAGE_IMEI1].t.string);
        MW::Image* j = new MW::Image(&(ui_image[LIST_IMAGE_IMEI1].r), &(ui_image[LIST_IMAGE_IMEI1].t));
        ui_image[LIST_IMAGE_IMEI1].index =  j->add_MWWList();
        WidgetList::show(ui_image[LIST_IMAGE_IMEI1].index);
        delete j;

        if (imeicnt == 2) {
            ui_image[LIST_IMAGE_IMEI2].t.string_len = strlen(ui_image[LIST_IMAGE_IMEI2].t.string);
            MW::Image* k = new MW::Image(&(ui_image[LIST_IMAGE_IMEI2].r), &(ui_image[LIST_IMAGE_IMEI2].t));
            ui_image[LIST_IMAGE_IMEI2].index =  k->add_MWWList();
            WidgetList::show(ui_image[LIST_IMAGE_IMEI2].index);
            delete k;
        }else if (imeicnt == 3) {
            ui_image[LIST_IMAGE_IMEI2].t.string_len = strlen(ui_image[LIST_IMAGE_IMEI2].t.string);
            MW::Image* l = new MW::Image(&(ui_image[LIST_IMAGE_IMEI2].r), &(ui_image[LIST_IMAGE_IMEI2].t));
            ui_image[LIST_IMAGE_IMEI2].index =  l->add_MWWList();
            WidgetList::show(ui_image[LIST_IMAGE_IMEI2].index);
            delete l;

            ui_image[LIST_IMAGE_IMEI3].t.string_len = strlen(ui_image[LIST_IMAGE_IMEI3].t.string);
            MW::Image* m = new MW::Image(&(ui_image[LIST_IMAGE_IMEI3].r), &(ui_image[LIST_IMAGE_IMEI3].t));
            ui_image[LIST_IMAGE_IMEI3].index =  m->add_MWWList();
            WidgetList::show(ui_image[LIST_IMAGE_IMEI3].index);
            delete m;
        }
#if !IS_TABLET
        SKIA_Point p1 = {0, SCREEN_HEIGHT*50/100, SCREEN_WIDTH, SCREEN_HEIGHT*50/100, _TDA_COLOR_BLACK, 3};

        MW::Line    *line_1  = NULL;
        MW::Line    *line_2  = NULL;
        line_1 = new MW::Line(&(p1));
        line_1->add_MWWList();
        delete line_1;
#endif
    }

    //line
    if(mode == MODE_PID)
    {
        int line_index = 0;
        SKIA_Point p_top = {0, _TITLE_H_, SCREEN_WIDTH, _TITLE_H_, MW_BLACK, MW_DEFAULT_STROKE_W};
        SKIA_Point p_bottom = {0, SCREEN_HEIGHT-_TITLE_H_+_GAP_, SCREEN_WIDTH, SCREEN_HEIGHT-_TITLE_H_+_GAP_, MW_BLACK, MW_DEFAULT_STROKE_W};

        MW::Line* line_top;
        line_top = new MW::Line(&p_top);
        line_index = line_top->add_MWWList();
        delete line_top;
        WidgetList::show(line_index);

        MW::Line* line_bottom;
        line_bottom = new MW::Line(&p_bottom);
        line_index = line_bottom->add_MWWList();
        delete line_bottom;
        WidgetList::show(line_index);
    }
#if IS_TABLET
    SKIA_Point p1 = {0, SCREEN_HEIGHT*50/100, SCREEN_WIDTH, SCREEN_HEIGHT*50/100, _TDA_COLOR_BLACK, 3};
    SKIA_Point p2 = {SCREEN_WIDTH*50/100, 0, SCREEN_WIDTH*50/100, SCREEN_HEIGHT, _TDA_COLOR_BLACK, 3};
#if PERFORM_AIO
    if (mode == MODE_TAIO) {

    } else {
#endif
        memset(property_value, 0, sizeof(property_value));
        property_get(SELFD_PROPERTY_RESULT, property_value, "999");
        if ((strncmp(property_value, "pass", 4) != 0 && mode == MODE_TABLET && aat_byte_check == 0) || (strncmp(property_value, "pass", 4) != 0 && mode == MODE_RESULT) || (strncmp(property_value, "pass", 4) == 0 && mode == MODE_TABLET && aat_byte_check != 0) || (strncmp(property_value, "pass", 4) != 0 && mode == MODE_TABLET && aat_byte_check != 0)) {
            p2 = {SCREEN_WIDTH*50/100, 0, SCREEN_WIDTH*50/100, SCREEN_HEIGHT*50/100, _TDA_COLOR_BLACK, 3};
        }
#if PERFORM_AIO
    }
#endif
    line_1 = new MW::Line(&(p1));
    line_1->add_MWWList();
    delete line_1;
    line_2 = new MW::Line(&(p2));
    line_2->add_MWWList();
    delete line_2;
#endif
}

#if IS_TABLET
void convert_num_to_name() {
    char property_aat_fail[256];
    char *ptr = NULL;
    char *token = NULL;
    memset(property_aat_fail, 0, sizeof(property_aat_fail));

    char aat_set[SIZE_AATSET];
    memset(aat_set, 0x00, sizeof(aat_set));
    char aat_order[SIZE_AATORDER];
    memset(aat_order, 0x00, sizeof(aat_order));

    int featured_list_num = get_full_list(NUM_LIST_MAX, aat_set, aat_order, mList, featured_mList);
    property_get(AAT_FAIL_ITEM, property_aat_fail, "999");
    ALOGD("[%s] property_aat_fail : %s", LOG_TITLE, property_aat_fail);

    if(strncmp(property_aat_fail, "999", 3) != 0) {
        token = strtok_r(property_aat_fail, ",", &ptr);
        while(token != NULL) {
            ALOGD("[%s] property_aat_fail token : %s, ptr : %s", LOG_TITLE, token, ptr);
            strncat(tmp, mList[atoi(token)].list_name, strlen(mList[atoi(token)].list_name)); // "-2" mean Auto Detecting Item & VT Camera.
            strcat(tmp, "\n");
            ALOGD("[%s] tmp : %s", LOG_TITLE, tmp);
            token = strtok_r(ptr, ",", &ptr);
        }
    }
}
#endif

void init() {
#if PERFORM_QR_FLASH
    mCamera = new minios3_Camera(CAMERA_ID_BACK);
    if(mCamera != NULL) {
        if(mCamera->getStatus() == CAM_STATUS::READY) {
            mCamera->start_preview();
            mCamera->setFlashTorch();
        }
    }
#endif

#if PERFORM_TOUCH_FW_UPLOAD
    char buf[PROPERTY_VALUE_MAX];
    memset(buf, 0, sizeof(buf));
    property_get("minios.blank.touch",  buf, NULL);

    if(strncmp(buf, "1", strlen("1")) == 0 )
    {
        property_set("minios.blank.touch", 0);
        memset(buf, 0, sizeof(buf));
        sprintf(buf, "%d", 0);
        if( unified_sysfs_write(UNI_SYSFS_PM_KEY_LED_RED_BRIGHTNESS, buf) < 0 ) {
            ALOGE("[%s] can't write fail : %s!", LOG_TITLE, PATH_LCD_BRIGHTNESS);
        }
        setSurfaceBlank(1);
    }
#endif

    if (skip_init) return;

    char property_value[PROPERTY_VALUE_MAX];
    memset(property_value, 0, sizeof(property_value));
    property_get("qrcode.img.size", property_value, "999");
    QR_SIZE = atoi(property_value);
    display_start_queue_c(SURFACE_MAIN);
#if IS_WATCH_PHONE
    init_light_device();
#endif
#if IS_TABLET
    init_light_device();
    memset(property_value, 0, sizeof(property_value));
    property_get(SELFD_PROPERTY_RESULT, property_value, "999");

    convert_num_to_name();

    if(strncmp(property_value, "pass", 4) == 0) {
        SKIA_Rect r_fail_item;
        r_fail_item.x = SCREEN_WIDTH*10/100;
        r_fail_item.y = SCREEN_HEIGHT*60/100;
        r_fail_item.w = SCREEN_WIDTH*80/100;
        r_fail_item.h = SCREEN_HEIGHT*40/100;

        fail_item = new MW::StaticText(&r_fail_item, &t_fail_item);
        fail_item->setText(tmp);
        fail_item->setActive(true);
    }
#endif
    draw_widget();
    Window::init();
    k_event = new MW::Key();
    k_event->onPressKeyCallback = key_press_event;
#if PERFORM_H2_PRJ
    if (h2_mode) {
        k_event->onReleaseKeyCallback = key_release_event;  // Key Test
        k_event->onTripleKeyCallback = onTripleKey;
    } else
#endif
    {
        k_event->onTripleKeyCallback = onTripleKey;
        k_event->onDoubleKeyCallback = onDubbleKey;
#if (IS_TABLET || IS_WATCH_PHONE)
        k_event->onReleaseKeyCallback = key_release_event;
        k_event->onPressTouchCallback = touchPressEvent;
        k_event->onReleaseTouchCallback = touchReleaseEvent;
#endif
    }
    EventService_AppEvent_c(1);
}

void end() {
#if PERFORM_QR_FLASH
    if(mCamera != NULL) {
        mCamera->setFlashOff();
        mCamera->stop_preview();
        mCamera->device_release();
        delete mCamera;
        mCamera = NULL;
    }
#endif
    WidgetList::destroy();
    Window::destroy();
    delete k_event;
    if (fail_item) delete fail_item;
#if (IS_TABLET || IS_WATCH_PHONE)
    free(mDevices);
#endif
    ALOGD("[%s] release qr display", LOG_TITLE);
}

void display_update() {
#if (!IS_TABLET && !IS_WATCH_PHONE)

    if( unified_sysfs_write(UNI_SYSFS_PM_LCD_BRIGHTNESS, "255") < 0 ) {
        ALOGD("[%s] Fail! LCD Brightness 255", LOG_TITLE);
    }
#if HAS_MARGIN_EXLCD
    if( unified_sysfs_write(UNI_SYSFS_PM_LCD_EX_BRIGHTNESS, "255") < 0 ) {
        ALOGD("[%s] Fail! ExD Brightness 255", LOG_TITLE);
    }
#endif
    if(check_usb_connection() == 1 && mode != MODE_IMEI) exit_loop = true;
#endif

#if PERFORM_H2_PRJ
    char h2_mode_property[SIZE_PROPERTY] ={0,};
    property_get(SELFD_EXT2_START, h2_mode_property, "");
    if( strcmp(h2_mode_property, "1" ) == 0 && h2_mode == false) {
        ALOGW("[%s] Current QR does'nt represent H2 Result, but H2 property was set", LOG_TITLE);
        not_mainlist = 1;
        usleep(1000*1000);
        exit_loop = true;
        return;
    }

// Update H2 retry cnt
#ifdef H2_RETRY_CNT
    if (h2_mode) {
        ALOGE("[%s] H2.0 RETRY_CNT DISPLAY", LOG_TITLE);
        char str[16] = {0,};

        h2_retry_cnt = readH2RetryCount();

        if (h2_retry_cnt >= 0) {
            ALOGE("[%s] H2.0 RETRY_CNT : %d", LOG_TITLE, h2_retry_cnt);
            memset(str, 0, sizeof(str));
            sprintf(str, "%d", h2_retry_cnt);
            WidgetList::setText(ui_text_box_h2_retry.index, str, strlen(str));
            WidgetList::setColor(ui_text_box_h2_retry.index, MW_BLACK, 0, 0);
            WidgetList::show(ui_text_box_h2_retry.index);
        }
    }
#endif

    // Enable display_update() when ems_history is failed
    if(need_to_update_qrcode_and_qrdisplay) {
        ALOGD("[%s] Need to update QR code and QR display because WIFI result is updated!!!", LOG_TITLE);
        int h2_result = check_selfd_E_result();
        if(h2_result == 0) {
            result = PASS;
        } else {
            result = FAIL;
        }
        system("/sbin/minios3_qrcode H2_RESULT");
        mode = MODE_RESULT;
        need_to_update_qrcode_and_qrdisplay = false;
    }

    int has_h2_result = has_selfd_E_result();
    if(has_h2_result == 1) {
        if(check_battery() == 0) {
            //battery check and color change
            if(battery <= BATTERY_PASS_LEVEL + 5) {
                if(battery_color == BG_SELFD_RESULT_COLOR) {
                    battery_color = _TDA_COLOR_YELLOW;
                } else {
                    battery_color = BG_SELFD_RESULT_COLOR;
                }
            }
        } else {
            bool e_log = false;
            if(e_log == false ) {
                ALOGE("[%s] Battery check fail", LOG_TITLE);
                e_log = true;
            }
        }
    }
#endif

#if (IS_TABLET || IS_WATCH_PHONE)
    check_tablet_off_scene();
#endif

    Delay(1000);
    draw_widget();
    display_start_queue_c(SURFACE_MAIN);
#if IS_WATCH_PHONE
    if(result == PASS)
        skia_add_erase_c(_TDA_COLOR_BLUE);
    else if(result == FAIL)
        skia_add_erase_c(_TDA_COLOR_RED);
    else
#endif
#if PERFORM_H2_PRJ
    if(has_h2_result == 1) {
        skia_add_erase_c(battery_color);
    } else {
        skia_add_erase_c(BG_SELFD_RESULT_COLOR);
    }
#else
    skia_add_erase_c(BG_SELFD_RESULT_COLOR);
#endif
    WidgetList::show();
    Window::show();
    display_update_queue_c(SURFACE_MAIN);
}

#if PERFORM_H2_PRJ
#ifdef KEYTEST_IN_QRDISPLAY
/*
 * Save the test result of keypress test.
 * Send the 2nd result including the result of keypress test
 * Please return after completing tranmission.
 */
static void add_key_test_result() {
    // Save the test result of keypress test.
    saveH2Result(NUM_AAT_QR_RESULT_KEYPRESS, '1');

    // To do... Adds the code to send the result
#if PERFORM_GENESIS_WIRELESS_DL
    char setems_state[SIZE_PROPERTY] ={0,};
    property_set(SELFD_WEBSERVER_STATE, "0"/*ing*/);
    system("/sbin/Ems_History");
    property_get(SELFD_WEBSERVER_STATE, setems_state, "0"/*ing*/);
    if(strncmp(setems_state, "2", 1) ==0) {
        ALOGD("[%s]: QR EMSEND SUCCESS !! ", LOG_TITLE);
#if !PERFORM_NO_WIFITURNON_IN_BOOTUP
        /* turnoff wifiminid */
        genesis_set_gstatus_c("G3");
#endif
    } else {
        ALOGD("[%s]: QR EMSEND FAIL !! ", LOG_TITLE);
    }
    wifim_turnoff();
#endif
}
#endif
#endif
int main (int argc, char * const args[]) {

#if (IS_TABLET || IS_WATCH_PHONE)
    aat_byte_check = check_aat_finished();
    ALOGD("[%s] aat_byte_check : %d", LOG_TITLE, aat_byte_check);
#endif

#if PERFORM_AIO && HAS_HDMI
    hdmi_result_check = read_T_result(NUM_LIST_TAIO_HDMI);
    ALOGD("[%s] hdmi_result_check : %d", LOG_TITLE, hdmi_result_check);
#endif

    property_set(SELFD_QR_DISPLAY_RUNNING, "1"); //enable to lcd always on

#if PERFORM_H2_PRJ
    battery_color = BG_SELFD_RESULT_COLOR;
#endif

    if(argc == 2 && (strncmp(args[1], "selfd_result", 12) == 0))
    {
        if (h2_mode) {
            skip_init = true;
        } else {
            char property_value[PROPERTY_VALUE_MAX];
            property_get("selfd.result", property_value, "");
            if(strncmp(property_value, "pass", 4) == 0)
                result = PASS;
            else
                result = FAIL;
#if IS_TABLET
            if(result == PASS)
                system("/sbin/minios3_qrcode SELFD_PASS");
            else
                system("/sbin/minios3_qrcode SELFD_FAIL");
#else
            system("/sbin/minios3_qrcode selfd_result");
#endif
            mode = MODE_RESULT;
        }
    }
    else if(argc == 3 && (strncmp(args[1], "aat_qr_result", 13) == 0))
    {
        if(strncmp(args[2], "pass", 4) == 0)
            result = PASS;
        else
            result = FAIL;
        system("/sbin/minios3_qrcode aat_qr_result");
        mode = MODE_RESULT;
    }
#if (IS_TABLET || IS_WATCH_PHONE)
    else if(argc == 2 && strncmp(args[1], "aat_result", 10) == 0)
    {
        system("/sbin/minios3_qrcode AAT_RESULT");
#if IS_TABLET
        mode = MODE_TABLET;
#else

        char property_value[PROPERTY_VALUE_MAX];
        property_get("selfd.result", property_value, "");

        aat_byte_check = check_aat_finished();

        ALOGD("[%s] aat_byte_check : %d, selfd:%s", LOG_TITLE, aat_byte_check, property_value);
        if(aat_byte_check  == 0 && (strncmp(property_value, "pass", 4) == 0))
            result = PASS;
        else
            result = FAIL;

        mode = MODE_RESULT;
#endif
    }
#endif
#if PERFORM_H2_PRJ
    else if(argc == 2 && strncmp(args[1], "H2_RESULT", 10) == 0) {
        integrateH2TestResult();

        int h2_result = check_selfd_E_result();
        h2_mode = true;

        property_set(SELFD_TRIGGER, "");
        property_set(SELFD_EXT2_START, "0");

        if(h2_result == 0) {
            result = PASS;
        } else {
            result = FAIL;
        }
#ifdef H2_RETRY_CNT
        h2_retry_cnt = readH2RetryCount();
#endif
        system("/sbin/minios3_qrcode H2_RESULT");
        mode = MODE_RESULT;
    }
    else if(argc == 3 && strncmp(args[1], "H2_RESULT", 10) == 0 && strncmp(args[2], "MENU_H2_RESULT", 15) == 0) {
        integrateH2TestResult();

        int h2_result = check_selfd_E_result();
        h2_mode = true;

        property_set(SELFD_TRIGGER, "");
        property_set(SELFD_EXT2_START, "0");

        if(h2_result == 0) {
            result = PASS;
        } else {
            result = FAIL;
        }
#ifdef H2_RETRY_CNT
        h2_retry_cnt = readH2RetryCount();
#endif
        system("/sbin/minios3_qrcode H2_RESULT");
        mode = MODE_RESULT;
    }
#endif
#if PERFORM_AIO
    else if(argc == 2 && strncmp(args[1], "TAIO_RESULT", 11) == 0) {
        system("/sbin/minios3_qrcode TAIO_RESULT");
        mode = MODE_TAIO;
        end_ble = check_end_ble();
        aio_result = check_aio_result();
        ALOGD("[%s] end_ble : %d, aio_result : %d", LOG_TITLE, end_ble, aio_result);
        ble_spoon_on_off(1);
    }
#endif
    else if(strncmp(args[1], "IMEI_RESULT", 11) == 0)
    {
        if (argc == 3) { //USIM
            ALOGD("[%s] USIM1", LOG_TITLE);
            imei[0] = (char*) malloc(strlen(args[2]) * sizeof(char));
            memset(imei[0], 0x00, sizeof(imei[0]));

            strcpy(imei[0], args[2]);
            imeicnt = 1;
        }
        else if (argc == 4) { //USIM2
            ALOGD("[%s] USIM2", LOG_TITLE);
            imei[0] = (char*) malloc(strlen(args[2]) * sizeof(char));
            memset(imei[0], 0x00, sizeof(imei[0]));
            imei[1] = (char*) malloc(strlen(args[3]) * sizeof(char));
            memset(imei[1], 0x00, sizeof(imei[1]));

            strcpy(imei[0], args[2]);
            strcpy(imei[1], args[3]);
            imeicnt = 2;
        }
        else if (argc == 5) { //USIM3
            imei[0] = (char*) malloc(strlen(args[2]) * sizeof(char));
            memset(imei[0], 0x00, sizeof(imei[0]));
            imei[1] = (char*) malloc(strlen(args[3]) * sizeof(char));
            memset(imei[1], 0x00, sizeof(imei[1]));
            imei[2] = (char*) malloc(strlen(args[4]) * sizeof(char));
            memset(imei[2], 0x00, sizeof(imei[2]));

            strcpy(imei[0], args[2]);
            strcpy(imei[1], args[3]);
            strcpy(imei[2], args[4]);
            imeicnt = 3;
        }

        ALOGD("[%s] IMEI & PID Result", LOG_TITLE);
        mode = MODE_IMEI;
    }
    else
    {
        system("/sbin/minios3_qrcode");
        result = PASS;
        mode = MODE_PID;
    }

    init();
    while(1) {
        display_update();
        if(exit_loop == true) break;
    }
    end();

    if(mode == MODE_RESULT) property_set(SELFD_READY_QR_DISPLAY, "0");
    property_set(SELFD_QR_DISPLAY_RUNNING, "0"); //disable to lcd always on

    /* back to current UI */
    int cur_test = find_next_test_cpp() -1;

#if PERFORM_AIO
    if(mode == MODE_TAIO) {
        /* ble off */
        ble_spoon_on_off(-1);
    }
#endif

#if (IS_TABLET || IS_WATCH_PHONE)

    rebase_trigger_qr_code_exit();

    //AAT result add.
    char aat_set[SIZE_AATSET];
    memset(aat_set, 0x00, sizeof(aat_set));
    char aat_order[SIZE_AATORDER];
    memset(aat_order, 0x00, sizeof(aat_order));
    int index = 0;

    int featured_test_num = get_featured_test_num_cpp();
    ALOGD("[%s] cur_test : %d, featured_test_num : %d", LOG_TITLE, cur_test, featured_test_num);
    if (cur_test == featured_test_num + 2) {
        property_set(PROPERTY_PRE_QR_DISPLAY, "1");
    }
#if IS_WATCH_PHONE
    char property_aat_status[256];
    memset(property_aat_status, 0, sizeof(property_aat_status));
    property_get(PROPERTY_AAT_STATUS, property_aat_status, "999");
    ALOGD("[%s] before :%s", LOG_TITLE, property_aat_status);
    if (strncmp(property_aat_status, "qrcode_f_aatresult", strlen("qrcode_f_aatresult")) == 0){
        cur_test = NUM_PRE_LIST_QR_AAT_FULL_RESULT;
        property_set(PROPERTY_AAT_STATUS, "aatresult_f_mainlist");
    }
    else
#endif
    cur_test = -1;
#endif
#if PERFORM_H2_PRJ
    if(argc == 2 && strncmp(args[1], "H2_RESULT", 10) == 0) {
        run_next_test_cpp(-1); //go to aat_main
    } else if(argc == 3 && strncmp(args[1], "H2_RESULT", 10) == 0 && strncmp(args[2], "MENU_H2_RESULT", 15) == 0) {
        property_set("ctl.start", "H2_Result"); // Return to H2_Result
    }else {
        if(not_mainlist == 0) {
        run_next_test_cpp(cur_test);
        }
    }
#else
    run_next_test_cpp(cur_test); //go to aat_main
#endif
    return 0;
}
