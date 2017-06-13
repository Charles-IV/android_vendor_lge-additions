#ifndef _AAT_RESULT_
#define _AAT_RESULT_

#include "minios_string_list.h"
#include <AatCommon.h>

#define _TITLE_H_ (SCREEN_HEIGHT*7/100)
#define _GAP_ (SCREEN_HEIGHT*1/100)
#define QRCODE_PATH "/data/minios/qrcode"
#define RESULT_PATH "/data/minios/result"
#define AAT_RESULT_PATH "/data/minios/aat_result"
#if PERFORM_AIO
#define TAIO_RESULT_PATH "/data/minios/taio_result"
#endif

#define IMEI_PATH1 "/data/minios/imei1"
#define IMEI_PATH2 "/data/minios/imei2"
#define IMEI_PATH3 "/data/minios/imei3"

extern "C" int wifim_turnoff();
static SKIA_Text t_fail_item = {"", 0, _TDA_COLOR_RED,  SKIAW_LEFT, FSIZE_TITLE_AD};

typedef enum {
#if IS_TABLET
  LIST_PASS_FAIL,
#endif
  LIST_TEST_ITEM,
  LIST_IMEI1,
  LIST_IMEI2,
  LIST_IMEI3,
  LIST_TEXT_MAX
} ui_text_box_a_type;

typedef enum {
  LIST_LABEL_TITLE,
  LIST_LABEL_TEXT,
  LIST_LABEL_MAX
} ui_label_a_type;

typedef enum {
  LIST_IMAGE_QRCODE,
  LIST_IMAGE_RESULT,
  LIST_IMAGE_AAT_RESULT,
#if PERFORM_AIO
  LIST_IMAGE_TAIO_RESULT,
#endif
#if IS_WATCH_PHONE
  LIST_IMAGE_BG,
#endif
  LIST_IMAGE_IMEI1,
  LIST_IMAGE_IMEI2,
  LIST_IMAGE_IMEI3,
  LIST_IMAGE_MAX
} ui_image_a_type;

static SKIA_UiList_t ui_label[LIST_LABEL_MAX] = {
  {0, {"", 0, _TDA_COLOR_BLACK, SKIAW_CENTER, FSIZE_TITLE_RESULT}, {0, 0, SCREEN_WIDTH, _TITLE_H_, 0}},
  {0, {"", 0, _TDA_COLOR_BLACK, SKIAW_CENTER, FSIZE_TITLE_RESULT}, {0, SCREEN_HEIGHT-_TITLE_H_, SCREEN_WIDTH, _TITLE_H_, 0}},
};

#if IS_TABLET
static SKIA_UiList_t ui_text_box[LIST_TEXT_MAX] = {
    {0, {"", 0, _TDA_COLOR_BLACK, SKIAW_CENTER, FSIZE_TITLE_AD}, {SCREEN_WIDTH*50/100, 0, (SCREEN_WIDTH*50/100)+1, (SCREEN_HEIGHT*50/100)+1, 0}},
    {0, {"", 0, _TDA_COLOR_BLACK, SKIAW_CENTER, FSIZE_TITLE_AD}, {SCREEN_WIDTH*30/100, SCREEN_HEIGHT*70/100, SCREEN_WIDTH*40/100, SCREEN_HEIGHT*30/100, 0}},
};
#else
static SKIA_UiList_t ui_text_box = {0, {"", 0, _TDA_COLOR_BLACK, SKIAW_CENTER, FSIZE_TITLE_AD}, {0, SCREEN_HEIGHT*50/100, (SCREEN_WIDTH)+1, (SCREEN_HEIGHT*50/100)+1, 0}};
static SKIA_UiList_t ui_text_box_desc = {0, {"", 0, _TDA_COLOR_BLACK, SKIAW_CENTER, FSIZE_BARO_AD}, {0, SCREEN_HEIGHT*50/100, (SCREEN_WIDTH)+1, (SCREEN_HEIGHT*50/100)+1, 0}};
#ifdef H2_RETRY_CNT
static SKIA_UiList_t ui_text_box_h2_retry = {0, {"", 0, _TDA_COLOR_BLACK, SKIAW_CENTER, (FSIZE_TITLE_AD*3)/4}, {0, SCREEN_HEIGHT*80/100, (SCREEN_WIDTH/2)+1, (SCREEN_HEIGHT*10/100), 0}};
#endif
#ifdef H2_DC_JIG_NUM
static SKIA_UiList_t ui_text_box_h2_dc_num = {0, {"", 0, _TDA_COLOR_BLACK, SKIAW_CENTER, (FSIZE_TITLE_AD*3)/4}, {0, SCREEN_HEIGHT*90/100, (SCREEN_WIDTH/2)+1, (SCREEN_HEIGHT*10/100), 0}};
#endif
#if HAS_SENSOR_PPG
static SKIA_UiList_t ui_ppg_text_box = {0, {"", 0,0, SKIAW_CENTER,0}, {0,0,0,0,0}};
#endif
#endif

#if (PERFORM_SDCARD_COUNT > 0)
static SKIA_UiList_t ui_text_sdcard_count = {0, {"", 0, _TDA_COLOR_RED, SKIAW_CENTER, 0}, {0, 0, 0, 0, 0}};
#endif

static SKIA_UiList_t ui_image[LIST_IMAGE_MAX] = {
  {1, {QRCODE_PATH, 0, 0, 0, 0},    {0,0,0,0,_TDA_COLOR_TRANS}},
  {1, {RESULT_PATH, 0, 0, 0, 0},    {0,0,0,0,_TDA_COLOR_TRANS}},
  {1, {AAT_RESULT_PATH, 0, 0, 0, 0},{0,0,0,0,_TDA_COLOR_TRANS}},
#if PERFORM_AIO
  {1, {TAIO_RESULT_PATH, 0, 0, 0, 0},{0,0,0,0,_TDA_COLOR_TRANS}},
#endif
#if IS_WATCH_PHONE
  {1, {"", 0, 0, 0, 0},{0,0,0,0,_TDA_COLOR_BLUE}},
#endif
  {LIST_IMAGE_IMEI1, {IMEI_PATH1, 0, 0, 0, 0},{0,0,0,0,_TDA_COLOR_TRANS}},
  {LIST_IMAGE_IMEI2, {IMEI_PATH2, 0, 0, 0, 0},{0,0,0,0,_TDA_COLOR_TRANS}},
  {LIST_IMAGE_IMEI3, {IMEI_PATH3, 0, 0, 0, 0},{0,0,0,0,_TDA_COLOR_TRANS}},
};

static SKIA_UiList_t ui_text_box_imei_pid[LIST_TEXT_MAX] = {
    {LIST_TEST_ITEM, {"", 0, _TDA_COLOR_BLACK, SKIAW_LEFT, FONT_SIZE_LIST}, {SCREEN_WIDTH*10/100, SCREEN_HEIGHT*65/100, SCREEN_WIDTH*10/100, SCREEN_HEIGHT*5/100, 0}},
    {LIST_IMEI1, {"", 0, _TDA_COLOR_BLACK, SKIAW_LEFT, FONT_SIZE_LIST}, {SCREEN_WIDTH*10/100, (SCREEN_HEIGHT*50/100) + 84 + (SCREEN_HEIGHT*10/100), SCREEN_WIDTH*50/100, SCREEN_HEIGHT*5/100, 0}},
    {LIST_IMEI2, {"", 0, _TDA_COLOR_BLACK, SKIAW_LEFT, FONT_SIZE_LIST}, {SCREEN_WIDTH*10/100, (SCREEN_HEIGHT*50/100) + 84 + (SCREEN_HEIGHT*20/100), SCREEN_WIDTH*50/100, SCREEN_HEIGHT*5/100, 0}},
    {LIST_IMEI3, {"", 0, _TDA_COLOR_BLACK, SKIAW_LEFT, FONT_SIZE_LIST}, {SCREEN_WIDTH*10/100, (SCREEN_HEIGHT*50/100) + 84 + (SCREEN_HEIGHT*30/100), SCREEN_WIDTH*50/100, SCREEN_HEIGHT*5/100, 0}},
};
#endif /*_AAT_RESULT_*/
