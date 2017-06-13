/*
 * Copyright(c) 2015 by LG Electronics. Confidential and Proprietary
 * All Rights Reserved.
 */

#define LOG_NDEBUG 0
#define LOG_TAG "AAT"
#define LOG_TITLE "minios3_qrcode"

#include <string.h>
#include <errno.h>
#include <ctype.h>
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <wchar.h>
#include <unistd.h>
#include <utils/Log.h>
#include <atd_define.h>
#include <cutils/properties.h>
#include <lgftm/ftmitem.h>
#include "qrencode.h"
#include "AatList.h"
#include "AatCommon.h"
#include "SelfDiagnosis.h"
#if PERFORM_H2_PRJ
#include "SelfDExt.h"
#endif
#if PERFORM_AIO
#include "T_Common.h"
#endif
#include "string_table.h"

#define QRCODE_FILE     "/data/minios/qrcode"     // Output file name
#define RESULT_FILE     "/data/minios/result"     // Output file name
#define AAT_RESULT_FILE "/data/minios/aat_result" // Output file name
#define H2_RESULT_FILE  "/data/minios/h2_result"  // Output file name

#if PERFORM_AIO
#define TAIO_RESULT_FILE "/data/minios/taio_result" // Output file name
#endif
#define IMEI_FILE1 "/data/minios/imei1" // Output file name
#define IMEI_FILE2 "/data/minios/imei2" // Output file name
#define IMEI_FILE3 "/data/minios/imei3" // Output file name

#define QRCODE_PIXEL_COLOR_R 0x00 // Color of bmp pixels
#define QRCODE_PIXEL_COLOR_G 0x00
#define QRCODE_PIXEL_COLOR_B 0x00

#if (IS_TABLET || IS_WATCH_PHONE)
#define AAT_FAIL_ITEM "aat.fail.item"
#define SELFD_PROCESS "SE"
#define AAT_PROCESS   "AA"
#endif

int FILE_PIXEL_PRESCALER; // Prescaler (number of pixels in bmp file for each QRCode pixel, on each dimension)
#define QRCODE_IMG_SIZE "qrcode.img.size"
//BMP defines
typedef unsigned short WORD;
typedef unsigned long DWORD;
typedef signed long LONG;

#define BI_RGB 0L
#pragma pack(push, 2)

typedef struct
{
  WORD  bfType;
  DWORD bfSize;
  WORD  bfReserved1;
  WORD  bfReserved2;
  DWORD bfOffBits;
} BITMAPFILEHEADER;

typedef struct
{
  DWORD biSize;
  LONG  biWidth;
  LONG  biHeight;
  WORD  biPlanes;
  WORD  biBitCount;
  DWORD biCompression;
  DWORD biSizeImage;
  LONG  biXPelsPerMeter;
  LONG  biYPelsPerMeter;
  DWORD biClrUsed;
  DWORD biClrImportant;
} BITMAPINFOHEADER;

#pragma pack(pop)

#if (IS_TABLET || IS_WATCH_PHONE)
char* touch_aat_set_check()
{
  char aat_set[SIZE_AATSET] = {'\0',};
  char *ptr = NULL;
  char *token = NULL;
  int i = 0;
  char tmp[SIZE_AATSET][SIZE_AATSET] = {'\0',};

  memset(aat_set, NULL, sizeof(aat_set));
  device_read(PATH_MISC, LGFTM_AATSET*BLOCK_SIZE, SIZE_AATSET, aat_set);

  ALOGD("[%s] Get aat_set from MISC : %s", LOG_TITLE, aat_set);

  token = strtok_r(aat_set, ",", &ptr);
  while(token != NULL) {
    strncpy(tmp[i], token, strlen(token));
    token = strtok_r(ptr, ",", &ptr);
    i++;
  }
  return &tmp[NUM_LIST_TOUCH_DRAW_MANUAL-1];
}

char* check_aat_test_name(int test_num)
{
  char test_name[10];
  memset(test_name, 0, sizeof(test_name));
  switch(test_num) {
    case NUM_LIST_AUTO_DETECT_ITEM:  strncpy(test_name,"AD1",3); break;
    case NUM_LIST_KEY_PRESS:         strncpy(test_name,"KP1",3); break;
    case NUM_LIST_QWERTY:            strncpy(test_name,"QK1",3); break;
    case NUM_LIST_CAMERA_REAR:       strncpy(test_name,"CA6",3); break;
    case NUM_LIST_CAMERA_RECORDING:  strncpy(test_name,"CC1",3); break;
    case NUM_LIST_CAMERA_FRONT:      strncpy(test_name,"VT2",3); break;
    case NUM_LIST_CAMERA_3D:         strncpy(test_name,"TD1",3); break;
    case NUM_LIST_LED_DISPLAY:       strncpy(test_name,"LC1",3); break;
    case NUM_LIST_AUDIO:             strncpy(test_name,"SO1",3); break;
    case NUM_LIST_VIBRATOR:          strncpy(test_name,"VI3",3); break;
    case NUM_LIST_IRRC_IRDA:         strncpy(test_name,"II1",3); break;
    case NUM_LIST_TOUCH_DRAW_MANUAL: strncpy(test_name,"TO2",3); break;
    case NUM_LIST_MOTION_SENSOR:     strncpy(test_name,"MS1",3); break;
    case NUM_LIST_GPS_BT_WIFI:       strncpy(test_name,"GP2",3); break;
    case NUM_LIST_DMB_ONESEG_HDMI:   strncpy(test_name,"DH1",3); break;
    case NUM_LIST_SLIDE_TEST:        strncpy(test_name,"SL1",3); break;
    case NUM_LIST_NFC_FELICA:        strncpy(test_name,"NF2",3); break;
    case NUM_LIST_CHARGING_TEST:     strncpy(test_name,"CH1",3); break;
    case NUM_LIST_FM_RADIO:          strncpy(test_name,"FM1",3); break;
    case NUM_LIST_LOOPBACK:          strncpy(test_name,"LB1",3); break;
    case NUM_LIST_DISLPAY_3D:        strncpy(test_name,"TL1",3); break;
    case NUM_LIST_KEY_SENSOR_AUTO:                               break;
    case NUM_LIST_TOUCH_DRAW_AUTO:                               break;
    case NUM_LIST_CAMERA_OIS:        strncpy(test_name,"OI2",3); break;
#if TEMP_AAT_ORDER_25
    case NUM_LIST_MP3:               strncpy(test_name,"EM1",3); break;
#endif
#if TEMP_AAT_ORDER_26
    case NUM_LIST_PPG:               strncpy(test_name,"PP1",3); break;
#endif
#if IS_SEPERATED_OTG
    case NUM_LIST_OTG:               strncpy(test_name,"OT2",3); break;
#endif
  }
  return test_name;
}

int read_aat_data(QRCODE_AAT *qrcode_aat)
{
  int i = 0;

  memset(qrcode_aat->aat_result, 0x00, sizeof(qrcode_aat->aat_result));
  device_read(PATH_MISC, (LGFTM_DEVICETEST_RESULT*BLOCK_SIZE), sizeof(qrcode_aat->aat_result), qrcode_aat->aat_result);
  ALOGD("[%s] aat_result:%s", LOG_TITLE, qrcode_aat->aat_result);

  if(strlen(qrcode_aat->aat_result) == 0)
  {
    for(i=0; i<NUM_LIST_MAX; i++)
    {
      qrcode_aat->aat_result[i] = ' ';
    }
  }

  ALOGD("[%s] aat_result:%s", LOG_TITLE, qrcode_aat->aat_result);
  return 0;
}
#endif

int read_selfd_result(QRCODE_SELFD_RESULT *qrcode_selfd_result)
{
  memset(qrcode_selfd_result->pid, NULL, sizeof(qrcode_selfd_result->pid));
  memset(qrcode_selfd_result->selfd_result, NULL, sizeof(qrcode_selfd_result->selfd_result));
  device_read(PATH_MISC, (LGFTM_SELFD_RESULT*BLOCK_SIZE), sizeof(qrcode_selfd_result->selfd_result), qrcode_selfd_result->selfd_result);
#if defined(MINIOS_PLATFORM_MTK)
  device_read(PATH_MISC2, (LGFTM_PID*BLOCK_SIZE_MISC2), sizeof(qrcode_selfd_result->pid), qrcode_selfd_result->pid);
  if( strncmp(qrcode_selfd_result->pid, "00000000000000000", 17) == 0 )
    qrcode_selfd_result->pid[17] = 0x00 ;
#else
  device_read(PATH_MISC, (LGFTM_PID*BLOCK_SIZE), sizeof(qrcode_selfd_result->pid), qrcode_selfd_result->pid);
  if(strlen(qrcode_selfd_result->pid) == 0)
   strncpy(qrcode_selfd_result->pid, "                 ", 17);
#endif

  return 0;
}

int read_aat_qr_result(QRCODE_AAT_QR_RESULT *qrcode_aat_qr_result)
{
  memset(qrcode_aat_qr_result->pid, NULL, sizeof(qrcode_aat_qr_result->pid));
  memset(qrcode_aat_qr_result->aat_qr_result, NULL, sizeof(qrcode_aat_qr_result->aat_qr_result));
  device_read(PATH_MISC, (LGFTM_AAT_QR_RESULT*BLOCK_SIZE), sizeof(qrcode_aat_qr_result->aat_qr_result), qrcode_aat_qr_result->aat_qr_result);
#if defined(MINIOS_PLATFORM_MTK)
  device_read(PATH_MISC2, (LGFTM_PID*BLOCK_SIZE_MISC2), sizeof(qrcode_aat_qr_result->pid), qrcode_aat_qr_result->pid);
  if( strncmp(qrcode_aat_qr_result->pid, "00000000000000000", 17) == 0 )
    qrcode_aat_qr_result->pid[17] = 0x00 ;
#else
  device_read(PATH_MISC, (LGFTM_PID*BLOCK_SIZE), sizeof(qrcode_aat_qr_result->pid), qrcode_aat_qr_result->pid);
  if(strlen(qrcode_aat_qr_result->pid) == 0)
    strncpy(qrcode_aat_qr_result->pid, "                 ", 17);
#endif

  return 0;
}

#if PERFORM_H2_PRJ
int read_H2_qr_result(QRCODE_AAT_QR_RESULT *qrcode_aat_qr_result)
{
  memset(qrcode_aat_qr_result->pid, NULL, sizeof(qrcode_aat_qr_result->pid));
  memset(qrcode_aat_qr_result->aat_qr_result, NULL, sizeof(qrcode_aat_qr_result->aat_qr_result));
  device_read(PATH_MISC, (LGFTM_SELFD_E_INTEGRATED_RESULT*BLOCK_SIZE), sizeof(qrcode_aat_qr_result->aat_qr_result), qrcode_aat_qr_result->aat_qr_result);
#if defined(MINIOS_PLATFORM_MTK)
  device_read(PATH_MISC2, (LGFTM_PID*BLOCK_SIZE_MISC2), sizeof(qrcode_aat_qr_result->pid), qrcode_aat_qr_result->pid);
  if( strncmp(qrcode_aat_qr_result->pid, "00000000000000000", 17) == 0 )
    qrcode_aat_qr_result->pid[17] = 0x00 ;
#else
  device_read(PATH_MISC, (LGFTM_PID*BLOCK_SIZE), sizeof(qrcode_aat_qr_result->pid), qrcode_aat_qr_result->pid);
  if(strlen(qrcode_aat_qr_result->pid) == 0)
    strncpy(qrcode_aat_qr_result->pid, "                 ", 17);
#endif

  return 0;
}
#endif

int read_pid(char *str)
{
  char pid[LGFTM_PID_SIZE];
  memset(pid, NULL, sizeof(pid));
#if defined(MINIOS_PLATFORM_MTK)
  device_read(PATH_MISC2, (LGFTM_PID*BLOCK_SIZE_MISC2), sizeof(pid), pid);
  if(strncmp(pid, "00000000000000000", 17) == 0)
    pid[17] = 0x00 ;
  else
    sprintf(str, "%s", pid);

#else
  device_read(PATH_MISC, (LGFTM_PID*BLOCK_SIZE), sizeof(pid), pid);
  if(strlen(pid) == 0)
    strncpy(str, "                 ", 17);
  else
    sprintf(str, "%s", pid);
#endif

  return 0;
}

#if PERFORM_H2_PRJ
char* QRCreate_H2()
{
    ALOGD("[%s] H2 QR CREATE", LOG_TITLE);

    QRCODE_AAT_QR_RESULT *result = NULL;
    FILE_PIXEL_PRESCALER = ceil((double)DEVICE_LCD_WIDTH / (double)540 * (double)4);
    ALOGD("[%s] device lcd width:%d, set qrcode pixel prescaler:%d", LOG_TITLE, DEVICE_LCD_WIDTH, FILE_PIXEL_PRESCALER);

    if (!(result = (unsigned char*)malloc(sizeof(QRCODE_AAT_QR_RESULT))))
    {
        ALOGD("[%s] qrcode_H2_result Allocate Fail", LOG_TITLE);
        return -1;
    }

    memset(result, NULL, sizeof(QRCODE_AAT_QR_RESULT));

    if (read_H2_qr_result(result) == -1) {
        ALOGD("[%s] read_pid_selfd_result Fail", LOG_TITLE);
        return -1;
    }

    int index, offset = 0;
    int list_size = (NUM_AAT_QR_RESULT_MAX * 3) + 1;
    int str_size = (NUM_AAT_QR_RESULT_MAX * 3) + LGFTM_PID_SIZE + 1;
    char pass_list[list_size];
    char fail_list[list_size];
    char na_list[list_size];
    char *str = (char*)malloc(sizeof(char)*str_size);
    memset(pass_list, NULL, sizeof(pass_list));
    memset(fail_list, NULL, sizeof(fail_list));
    memset(na_list, NULL, sizeof(na_list));
    memset(str, NULL, sizeof(str));

    if(strlen(result->aat_qr_result) == 0) {
      ALOGD("[%s] error! aat_qr_result is null", LOG_TITLE);
    } else {
        for(index=0; index<NUM_AAT_QR_RESULT_MAX; index++) {
            char item[4] = "   ";
            strncpy(item, mAatQrResult[index].string, 3);

            offset = index + 2;
            if(result->aat_qr_result[offset] == '1') { //pass
                strncat(pass_list, item, 3);
            } else if(result->aat_qr_result[offset] == '0') { //fail
                strncat(fail_list, item, 3);
            } else if(result->aat_qr_result[offset] == '2') { //nt
                strncat(na_list, item, 3);
            }
        }
        sprintf(str, "%s,%s,%s,%s,SE,H20", result->pid, pass_list, fail_list, na_list);
    }
    ALOGD("[%s] str:%s", LOG_TITLE, str);
    if(result != NULL)
      free(result);

    return str;
}
#endif

#if PERFORM_AIO
int read_taio_qr_result(QRCODE_TAIO_QR_RESULT *qrcode_taio_qr_result)
{
  memset(qrcode_taio_qr_result->pid, NULL, sizeof(qrcode_taio_qr_result->pid));
  memset(qrcode_taio_qr_result->taio_qr_result, NULL, sizeof(qrcode_taio_qr_result->taio_qr_result));

  device_read(PATH_MISC, (LGFTM_TAIO_RESULT*BLOCK_SIZE), sizeof(qrcode_taio_qr_result->taio_qr_result), qrcode_taio_qr_result->taio_qr_result);
  ALOGD("[%s] read_taio_qr_result, qrcode_taio_qr_result->taio_qr_result : %s", LOG_TITLE, qrcode_taio_qr_result->taio_qr_result);

  //WS_RESULT [S]
  memset(qrcode_taio_qr_result->ws_result, NULL, sizeof(qrcode_taio_qr_result->ws_result));
  device_read(PATH_MISC, (LGFTM_WS_QR_STATUS*BLOCK_SIZE), sizeof(qrcode_taio_qr_result->ws_result), qrcode_taio_qr_result->ws_result);
  ALOGD("[%s] read_taio_qr_result, qrcode_taio_qr_result->ws_result : %s", LOG_TITLE, qrcode_taio_qr_result->ws_result);
  //WS_RESULT [E]

#if defined(MINIOS_PLATFORM_MTK)
  device_read(PATH_MISC2, (LGFTM_PID*BLOCK_SIZE_MISC2), sizeof(qrcode_taio_qr_result->pid), qrcode_taio_qr_result->pid);
  if( strncmp(qrcode_taio_qr_result->pid, "00000000000000000", 17) == 0 )
    qrcode_taio_qr_result->pid[17] = 0x00 ;
#else
  device_read(PATH_MISC, (LGFTM_PID*BLOCK_SIZE), sizeof(qrcode_taio_qr_result->pid), qrcode_taio_qr_result->pid);
  if(strlen(qrcode_taio_qr_result->pid) == 0)
    strncpy(qrcode_taio_qr_result->pid, "                 ", 17);
#endif

  return 0;
}

char* QRCreate_taio()
{
    ALOGD("[%s] taio QR Create", LOG_TITLE);

    QRCODE_TAIO_QR_RESULT *qrcode_taio_result = NULL;
    QRCODE_SELFD_RESULT *qrcode_selfd_result = NULL;

    FILE_PIXEL_PRESCALER = ceil((double)DEVICE_LCD_WIDTH / (double)540 * (double)4);
    ALOGD("[%s] device lcd width:%d, set qrcode pixel prescaler:%d", LOG_TITLE, DEVICE_LCD_WIDTH, FILE_PIXEL_PRESCALER);

    if (!(qrcode_selfd_result = (unsigned char*)malloc(sizeof(QRCODE_SELFD_RESULT))))
    {
        ALOGD("[%s] qrcode_selfd_result allocate Fail", LOG_TITLE);
        return -1;
    }

    if (!(qrcode_taio_result = (unsigned char*)malloc(sizeof(QRCODE_TAIO_QR_RESULT))))
    {
        ALOGD("[%s] qrcode_TAIO_result allocate Fail", LOG_TITLE);
        return -1;
    }

    memset(qrcode_taio_result, NULL, sizeof(QRCODE_TAIO_QR_RESULT));
    memset(qrcode_selfd_result, NULL, sizeof(QRCODE_SELFD_RESULT));

    if (read_selfd_result(qrcode_selfd_result) == -1) {
        ALOGD("[%s] read_selfd_result fail", LOG_TITLE);
        return -1;
    }

    if (read_taio_qr_result(qrcode_taio_result) == -1) {
        ALOGD("[%s] read_taio_qr_result fail", LOG_TITLE);
        return -1;
    }

    int index, offset = 0;
    int list_size = ((NUM_SELFD_PROPERTY_MAX + 50) * 3) + 1;
    int str_size = ((NUM_LIST_TAIO_MAX +50) * 3 + (NUM_SELFD_PROPERTY_MAX + 50)* 6) + LGFTM_PID_SIZE + 1;
    char pass_list[list_size];
    char fail_list[list_size];
    char na_list[list_size];
    char test_list[list_size];
    char *str = (char*)malloc(sizeof(char)*str_size);
    memset(pass_list, NULL, sizeof(pass_list));
    memset(fail_list, NULL, sizeof(fail_list));
    memset(na_list, NULL, sizeof(na_list));
    memset(test_list, NULL, sizeof(test_list));
    memset(str, NULL, sizeof(str));

    if(strlen(qrcode_selfd_result->selfd_result) == 0) {
        ALOGD("[%s] error! selfd_result is null", LOG_TITLE);
    } else {

        for(index=0; index<NUM_SELFD_PROPERTY_MAX; index++) {
            char item[4] = {'\0',};
            strncpy(item, mSelfdResult[index].string, 3);
            offset = index + 2; //selfd result index 0, 1 means self-d all result
            if(qrcode_selfd_result->selfd_result[offset] == '1') //pass
            {
                strncat(pass_list, item, 3);
            }
            else if (qrcode_selfd_result->selfd_result[offset] == '0') //fail
            {
                strncat(fail_list, item, 3);
            }
        }
    }

    ALOGD("[%s] pass_list : %s, fail_list : %s", LOG_TITLE, pass_list, fail_list);

    if(strlen(qrcode_taio_result->taio_qr_result) == 0) {
      ALOGD("[%s] error! taio_qr_result is null", LOG_TITLE);
    } else {
        for(index=0; index<NUM_LIST_TAIO_MAX; index++) {
            char item[4] = {'\0',};
            strncpy(item, mTaioQrResult[index].string, 3);

            offset = index + 2;
            if(qrcode_taio_result->taio_qr_result[offset] == '1') { //pass
                strncat(pass_list, item, 3);
            } else if(qrcode_taio_result->taio_qr_result[offset] == '0') { //fail
                strncat(fail_list, item, 3);
            } else if(qrcode_taio_result->taio_qr_result[offset] == '2') { //nt
                strncat(na_list, item, 3);
            }
        }
    }

    char property_ws_enable[256] = {0,};
    memset(property_ws_enable, 0, sizeof(property_ws_enable));
    property_get(WS_ENABLE, property_ws_enable, "999");

    if(strlen(qrcode_taio_result->ws_result) == 0) {
      ALOGE("[%s] error! ws_result is null", LOG_TITLE);
    } else {
        if (strncmp(property_ws_enable, "1", 1) == 0 || strncmp(property_ws_enable, "3", 1) == 0) {
            for(index=NUM_AAT_QR_RESULT_ANTENNA1; index<=NUM_AAT_QR_RESULT_ANTENNA50; index++) {
                char item[4] = {'\0',};
                strncpy(item, mAatQrResult[index].string, 3);
                //ALOGD("[%s] ws_item check : %s", LOG_TITLE, item);

                offset = index - NUM_AAT_QR_RESULT_ANTENNA1;

                if(index == NUM_AAT_QR_RESULT_ANTENNA12) { // WS Total Fail.
                    if(qrcode_taio_result->ws_result[offset] == '0') {
                        strncat(test_list, "AA", 2);
                    } else if(qrcode_taio_result->ws_result[offset] == '1') {
            }
            continue;
                }

                if(qrcode_taio_result->ws_result[offset] == '1') { //pass
                    strncat(pass_list, item, 3);
                } else if(qrcode_taio_result->ws_result[offset] == '0') { //fail
                    strncat(fail_list, item, 3);
                } else if(qrcode_taio_result->ws_result[offset] == '2') { //nt
                    strncat(na_list, item, 3);
                }
            }
        } else {
            ALOGE("[%s] ws_result exist. but, ws.enable was '0'", LOG_TITLE);
        }
    }

    // fail process check routine
    if (qrcode_selfd_result->selfd_result[0] != '0') {
        memset(test_list, NULL, sizeof(test_list));
        strncat(test_list, "SE", 2);
    } else if (qrcode_taio_result->taio_qr_result[0] != '0') {
        memset(test_list, NULL, sizeof(test_list));
        strncat(test_list, "AA", 2);
    }

    sprintf(str, "%s,%s,%s,%s", qrcode_taio_result->pid, pass_list, fail_list, test_list);

    ALOGD("[%s] str:%s", LOG_TITLE, str);
    if(qrcode_taio_result != NULL)
      free(qrcode_taio_result);

    if(qrcode_selfd_result != NULL)
      free(qrcode_selfd_result);

    return str;
}
#endif

int main (int argc, char * const args[])
{
  unsigned int unWidth, x, y, l, n, unWidthAdjusted, unDataBytes;
  unsigned char* pRGBData, *pSourceData, *pDestData;
  char* str = NULL;
  QRcode* pQRC;
  FILE* f;
#if IS_TABLET
  if(argc == 2  && ((strncmp(args[1], "SELFD_PASS", 10) == 0) || (strncmp(args[1], "SELFD_FAIL", 10) == 0)))
#else
  if(argc == 2 && strncmp(args[1], "selfd_result", 12) == 0)
#endif
  {
    QRCODE_SELFD_RESULT *qrcode_selfd_result = NULL;
    FILE_PIXEL_PRESCALER = ceil((double)DEVICE_LCD_WIDTH / (double)540 * (double)4);
    ALOGD("[%s] device lcd width:%d, set qrcode pixel prescaler:%d", LOG_TITLE, DEVICE_LCD_WIDTH, FILE_PIXEL_PRESCALER);

    if (!(qrcode_selfd_result = (unsigned char*)malloc(sizeof(QRCODE_SELFD_RESULT))))
    {
      ALOGD("[%s] qrcode_selfd_result Allocate Fail", LOG_TITLE);
      return -1;
    }
    memset(qrcode_selfd_result, NULL, sizeof(QRCODE_SELFD_RESULT));

    if (read_selfd_result(qrcode_selfd_result) == -1) {
      ALOGD("[%s] read_pid_selfd_result Fail", LOG_TITLE);
      return -1;
    }

#if IS_TABLET || IS_WATCH_PHONE
     char property_selfd_result[256];
     memset(property_selfd_result, 0, sizeof(property_selfd_result));
     property_get(SELFD_PROPERTY_RESULT, property_selfd_result, "999");
#endif

    int index, offset = 0;
    int list_size = (NUM_SELFD_PROPERTY_MAX * 3) + 1;
    int str_size = (NUM_SELFD_PROPERTY_MAX * 3) + LGFTM_PID_SIZE + 1;
    char pass_list[list_size];
    char fail_list[list_size];
    char na_list[list_size];
#if IS_TABLET || IS_WATCH_PHONE
    char test_list[list_size];
    memset(test_list, 0x00, sizeof(test_list));
#endif
    str = (char*)malloc(sizeof(char)*str_size);
    memset(pass_list, NULL, sizeof(pass_list));
    memset(fail_list, NULL, sizeof(fail_list));
    memset(na_list, NULL, sizeof(na_list));
    memset(str, NULL, sizeof(str));

    if(strlen(qrcode_selfd_result->selfd_result) == 0)
    {
      ALOGD("[%s] error! selfd_result is null", LOG_TITLE);
    }
    else
    {
      for(index=0; index<NUM_SELFD_PROPERTY_MAX; index++)
      {
        char item[4] = "   ";
        strncpy(item, mSelfdResult[index].string, 3);

        offset = index + 2; //selfd result index 0, 1 means self-d all result
        if(qrcode_selfd_result->selfd_result[offset] == '1') //pass
        {
          strncat(pass_list, item, 3);
        }
        else if(qrcode_selfd_result->selfd_result[offset] == '0') //fail
        {
          strncat(fail_list, item, 3);
        }
#if (!IS_TABLET && !IS_WATCH_PHONE)
        else if(qrcode_selfd_result->selfd_result[offset] == '2') //not test
        {
          strncat(na_list, item, 3);
        }
#endif
      }

#if IS_TABLET || IS_WATCH_PHONE
      if (strncmp(property_selfd_result, "fail", 4) == 0)
        strncat(test_list, SELFD_PROCESS, strlen(SELFD_PROCESS));

      sprintf(str, "%s,%s,%s,%s", qrcode_selfd_result->pid, pass_list, fail_list, test_list);
#else
      sprintf(str, "%s,%s,%s,%s,SE", qrcode_selfd_result->pid, pass_list, fail_list, na_list);
#endif
    }
    ALOGD("[%s] str:%s", LOG_TITLE, str);
    if(qrcode_selfd_result != NULL)
    free(qrcode_selfd_result);
  }
  //Real-time tracing
  else if( (argc == 2 && strncmp(args[1], "aat_qr_result", 13) == 0) )
  {
    QRCODE_AAT_QR_RESULT *qrcode_aat_qr_result = NULL;
    FILE_PIXEL_PRESCALER = ceil((double)DEVICE_LCD_WIDTH / (double)540 * (double)4);
    ALOGD("[%s] device lcd width:%d, set qrcode pixel prescaler:%d", LOG_TITLE, DEVICE_LCD_WIDTH, FILE_PIXEL_PRESCALER);

    if (!(qrcode_aat_qr_result = (unsigned char*)malloc(sizeof(QRCODE_AAT_QR_RESULT))))
    {
      ALOGD("[%s] aat_qr_result Allocate Fail", LOG_TITLE);
      return -1;
    }

    memset(qrcode_aat_qr_result, NULL, sizeof(QRCODE_AAT_QR_RESULT));
    if (read_aat_qr_result(qrcode_aat_qr_result) == -1) {
      ALOGD("[%s] read_aat_qr_result Fail", LOG_TITLE);
      return -1;
    }

    int index;
    int list_size = (NUM_AAT_QR_RESULT_MAX * 3) + 1;
    int str_size = (NUM_AAT_QR_RESULT_MAX * 3) + LGFTM_PID_SIZE + 1;
    char pass_list[list_size];
    char fail_list[list_size];
    str = (char*)malloc(sizeof(char)*str_size);
    memset(pass_list, NULL, sizeof(pass_list));
    memset(fail_list, NULL, sizeof(fail_list));
    memset(str, NULL, sizeof(str));

    if(strlen(qrcode_aat_qr_result->aat_qr_result) == 0)
    {
      ALOGD("[%s] error! aat_qr_result is null", LOG_TITLE);
    }
    else
    {
      for(index=0; index<NUM_AAT_QR_RESULT_MAX; index++)
      {
        char item[4] = "   ";
        strncpy(item, mAatQrResult[index].string, 3);

        if(qrcode_aat_qr_result->aat_qr_result[index] == '1') //pass
        {
          strncat(pass_list, item, 3);
        }
        else if(qrcode_aat_qr_result->aat_qr_result[index] == '0') //fail
        {
          strncat(fail_list, item, 3);
        }
      }
      sprintf(str, "%s,%s,%s", qrcode_aat_qr_result->pid, pass_list, fail_list);
    }
    ALOGD("[%s] str:%s", LOG_TITLE, str);
    if(qrcode_aat_qr_result != NULL)
      free(qrcode_aat_qr_result);
  }
#if (IS_TABLET || IS_WATCH_PHONE)
  else if(argc == 2  && strncmp(args[1], "AAT_RESULT", 10) == 0)
  {
    QRCODE_SELFD_RESULT *qrcode_selfd_result = NULL;
    QRCODE_AAT *qrcode_aat = NULL;
    FILE_PIXEL_PRESCALER = ceil((double)DEVICE_LCD_WIDTH / (double)540 * (double)4);
#if IS_WATCH_PHONE
    FILE_PIXEL_PRESCALER = 5;
#endif
    ALOGD("[%s] device lcd width:%d, set qrcode pixel prescaler:%d", LOG_TITLE, DEVICE_LCD_WIDTH, FILE_PIXEL_PRESCALER);

    if (!(qrcode_selfd_result = (unsigned char*)malloc(sizeof(QRCODE_SELFD_RESULT))))
    {
      ALOGD("[%s] qrcode_selfd_result Allocate Fail", LOG_TITLE);
      return -1;
    }
    memset(qrcode_selfd_result, 0x00, sizeof(QRCODE_SELFD_RESULT));

    if (!(qrcode_aat = (unsigned char*)malloc(sizeof(QRCODE_AAT))))
    {
      ALOGD("[%s] qrcode_aat Allocate Fail", LOG_TITLE);
      return -1;
    }
    memset(qrcode_aat, 0x00, sizeof(QRCODE_AAT));

    if (read_selfd_result(qrcode_selfd_result) == -1)
    {
      ALOGD("[%s] AAT_RESULT QR, read_misc_data Fail", LOG_TITLE);
      return -1;
    }

    if (read_aat_data(qrcode_aat) == -1)
    {
      ALOGD("[%s] AAT_RESULT QR, read_aat_data Fail", LOG_TITLE);
      return -1;
    }

    char property_selfd_result[256];
    memset(property_selfd_result, 0, sizeof(property_selfd_result));
    property_get(SELFD_PROPERTY_RESULT, property_selfd_result, "999");

    //QR result add.
    int index, offset = 0;
    int list_size = (NUM_LIST_MAX * 3) + 1;
    int str_size = ((NUM_LIST_MAX + NUM_SELFD_PROPERTY_MAX) * 3) + LGFTM_PID_SIZE + 1;
    char pass_list[list_size];
    char fail_list[list_size];
    char na_list[list_size];
    char test_list[list_size];
    char FailOrSkip[1024];
    char FailOrSkip_temp[1024];
    str = (char*)malloc(sizeof(char)*str_size);
    memset(pass_list, 0x00, sizeof(pass_list));
    memset(fail_list, 0x00, sizeof(fail_list));
    memset(na_list, 0x00, sizeof(na_list));
    memset(test_list, 0x00, sizeof(test_list));
    memset(FailOrSkip, 0x00, sizeof(FailOrSkip));
    memset(FailOrSkip_temp, 0x00, sizeof(FailOrSkip_temp));
    memset(str, NULL, sizeof(str));

    for(index=0; index<NUM_SELFD_PROPERTY_MAX; index++)
    {
      char item[4] = "   ";
      strncpy(item, mSelfdResult[index].string, 3);
      offset = index + 2; //selfd result index 0, 1 means self-d all result
      if(qrcode_selfd_result->selfd_result[offset] == '1') //pass
      {
        strncat(pass_list, item, 3);
      }
      else if(qrcode_selfd_result->selfd_result[offset] == '0') //fail
      {
        strncat(fail_list, item, 3);
      }
    }

    //AAT result add.
    char aat_set[SIZE_AATSET];
    memset(aat_set, 0x00, sizeof(aat_set));
    char aat_order[SIZE_AATORDER];
    memset(aat_order, 0x00, sizeof(aat_order));
    int skip_count = 0;

    int featured_list_num = get_full_list(NUM_LIST_MAX, aat_set, aat_order, mList, featured_mList);
#if TYPE_KEY_SOFTKEY
    int featured_test_num = featured_list_num - 4;
#else
    int featured_test_num = featured_list_num - 3;
#endif
    for(index = 0; index < featured_test_num; index++) {

#if PERFORM_SELF_D
      if (featured_mList[index+1].test_order == NUM_LIST_AUTO_DETECT_ITEM || featured_mList[index+1].test_order == NUM_LIST_KEY_SENSOR_AUTO || featured_mList[index+1].test_order == NUM_LIST_TOUCH_DRAW_AUTO || featured_mList[index+1].test_order == NUM_LIST_CAMERA_FRONT) {
        skip_count++;
        continue;
      }
#endif

      if(featured_mList[index+1].test_order == NUM_LIST_TOUCH_DRAW_MANUAL) {
      if(strncmp(touch_aat_set_check(), "4", 1) == 0) {
        ALOGD("[%s] TOUCH_DRAW_MANUAL Test active only selfd", LOG_TITLE);
        skip_count++;
        continue;
      } else {
        ALOGD("[%s] TOUCH_DRAW_MANUAL Test active aat full & selfd", LOG_TITLE);
      }
      }

      char item[256] = "   ";
      offset = index + 2; //selfd result index 0, 1 means self-d all result
      if(qrcode_aat->aat_result[offset-skip_count] == '1') //pass
      {
        strncpy(item, check_aat_test_name(featured_mList[index+1].test_order), strlen(check_aat_test_name(featured_mList[index+1].test_order)));
        strncat(pass_list, item, 3);
      }
      else if(qrcode_aat->aat_result[offset-skip_count] == '0'
#if PERFORM_SKIP_SCENARIO
        || qrcode_aat->aat_result[offset-skip_count] == '2'
#endif
      ) //fail or skip
      {
        strncpy(item, check_aat_test_name(featured_mList[index+1].test_order), strlen(check_aat_test_name(featured_mList[index+1].test_order)));
        strncat(fail_list, item, 3);
        memset(FailOrSkip_temp, 0, sizeof(FailOrSkip_temp));
        sprintf(FailOrSkip_temp, "%d,", featured_mList[index+1].test_order);
        strcat(FailOrSkip, FailOrSkip_temp);
      }
    }

    if (strncmp(property_selfd_result, "fail", 4) == 0) {
      ALOGD("[%s] SELFD TEST not all finished", LOG_TITLE);
      strncat(test_list, SELFD_PROCESS, strlen(SELFD_PROCESS));
   } else if (qrcode_aat->aat_result[0] != '0') {
      ALOGD("[%s] AAT TEST not all finished", LOG_TITLE);
      strncat(test_list, AAT_PROCESS, strlen(AAT_PROCESS));
    } else {
      ALOGD("[%s] TEST all finished", LOG_TITLE);
    }

#if (IS_TABLET || IS_WATCH_PHONE)
    property_set(AAT_FAIL_ITEM, FailOrSkip);
    ALOGD("[%s] FailOrSkip : %s", LOG_TITLE, FailOrSkip);

    sprintf(str, "%s,%s,%s,%s", qrcode_selfd_result->pid, pass_list, fail_list, test_list);
#else
    sprintf(str, "%s,%s,%s,%s,%s", qrcode_selfd_result->pid, pass_list, fail_list, na_list, test_list);
#endif
    ALOGD("[%s] SELFD & AAT, str:%s", LOG_TITLE, str);

    if(qrcode_aat != NULL)
      free(qrcode_aat);

    if(qrcode_selfd_result != NULL)
      free(qrcode_selfd_result);
  }
#endif
#if PERFORM_H2_PRJ
    else if(argc == 2 && strncmp(args[1], "H2_RESULT", 9) == 0)
    {
       str = QRCreate_H2();
    }
#endif
#if PERFORM_AIO
  else if(argc == 2 && strncmp(args[1], "TAIO_RESULT", 11) == 0)
  {
      str = QRCreate_taio();
  }
#endif
  else if( (argc == 4 && strncmp(args[1], "IMEI", 4) == 0) )
  {
    char imeistr[1024] = {0,};
    strcpy(imeistr, args[3]);
    ALOGD("[%s] IMEI %s : %s", LOG_TITLE, args[2], imeistr);

    FILE_PIXEL_PRESCALER = ceil((double)DEVICE_LCD_WIDTH / (double)540 * (double)4);
    //FILE_PIXEL_PRESCALER = 10;
    str = (char*)malloc(sizeof(char) * LGFTM_IMEI_SIZE*2+1);
    memset(str, NULL, LGFTM_IMEI_SIZE);
    sprintf(str, "%s", imeistr);
  }
  else if( (argc == 3 && strncmp(args[1], "IMEI", 4) == 0) )
  {
    char imeistr[1024] = {0,};
    strcpy(imeistr, args[2]);
    ALOGD("[%s] IMEI : %s", LOG_TITLE, imeistr);

    FILE_PIXEL_PRESCALER = ceil((double)DEVICE_LCD_WIDTH / (double)540 * (double)4);
    //FILE_PIXEL_PRESCALER = 10;
    str = (char*)malloc(sizeof(char) * LGFTM_IMEI_SIZE);
    memset(str, NULL, LGFTM_IMEI_SIZE);
    sprintf(str, "%s", imeistr);
  }
  else
  {
    str = (char*)malloc(sizeof(char) * LGFTM_PID_SIZE);
    memset(str, NULL, LGFTM_PID_SIZE);
    FILE_PIXEL_PRESCALER = 8;
    read_pid(str);
  }
  //ALOGD("minios3_qrcode String : %s", str );

#if IS_TABLET
pid_qr_generation:
#endif
  //Compute QRCode
  if (pQRC = QRcode_encodeString(str, 0, QR_ECLEVEL_L, QR_MODE_8, 1))
  {
    unWidth = pQRC->width;
    unWidthAdjusted = unWidth * FILE_PIXEL_PRESCALER * 3;
    if(unWidthAdjusted % 4)
    {
      unWidthAdjusted = (unWidthAdjusted / 4 + 1) * 4;
    }
    unDataBytes = unWidthAdjusted * unWidth * FILE_PIXEL_PRESCALER;

    //Allocate pixels buffer
    if (!(pRGBData = (unsigned char*)malloc(unDataBytes)))
    {
      ALOGD("[%s] Allocate Fail", LOG_TITLE);
      return -1;
    }

    //Preset to white
    memset(pRGBData, 0xFF, unDataBytes);

    //Prepare bmp headers
    BITMAPFILEHEADER kFileHeader;
    kFileHeader.bfType = 0x4d42; // "BM"
    kFileHeader.bfSize =  sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + unDataBytes;
    kFileHeader.bfReserved1 = 0;
    kFileHeader.bfReserved2 = 0;
    kFileHeader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

    BITMAPINFOHEADER kInfoHeader;
    kInfoHeader.biSize = sizeof(BITMAPINFOHEADER);
    kInfoHeader.biWidth = unWidth * FILE_PIXEL_PRESCALER;
    kInfoHeader.biHeight = -((int)unWidth * FILE_PIXEL_PRESCALER);

    char property_qr_size[256];
    memset(property_qr_size, 0, sizeof(property_qr_size));
    sprintf(property_qr_size, "%d", kInfoHeader.biWidth);
    property_set(QRCODE_IMG_SIZE, property_qr_size );

    kInfoHeader.biPlanes = 1;
    kInfoHeader.biBitCount = 24;
    kInfoHeader.biCompression = BI_RGB;
    kInfoHeader.biSizeImage = 0;
    kInfoHeader.biXPelsPerMeter = 0;
    kInfoHeader.biYPelsPerMeter = 0;
    kInfoHeader.biClrUsed = 0;
    kInfoHeader.biClrImportant = 0;

    //Convert QrCode bits to bmp pixels
    pSourceData = pQRC->data;
    for(y = 0; y < unWidth; y++)
    {
      pDestData = pRGBData + unWidthAdjusted * y * FILE_PIXEL_PRESCALER;
      for(x = 0; x < unWidth; x++)
      {
        if (*pSourceData & 1)
        {
          for(l = 0; l < FILE_PIXEL_PRESCALER; l++)
          {
            for(n = 0; n < FILE_PIXEL_PRESCALER; n++)
            {
              *(pDestData +     n * 3 + unWidthAdjusted * l) =  QRCODE_PIXEL_COLOR_B;
              *(pDestData + 1 + n * 3 + unWidthAdjusted * l) =  QRCODE_PIXEL_COLOR_G;
              *(pDestData + 2 + n * 3 + unWidthAdjusted * l) =  QRCODE_PIXEL_COLOR_R;
            }
          }
        }
        pDestData += 3 * FILE_PIXEL_PRESCALER;
        pSourceData++;
      }
    }

    //Output bmp file
#if IS_TABLET
    if(argc == 2  && ((strncmp(args[1], "SELFD_PASS", 10) == 0) || (strncmp(args[1], "SELFD_FAIL", 10) == 0)))
#else
    if(argc == 2)
#endif
    {
      if(access(RESULT_FILE, F_OK) == 0)
      {
        remove(RESULT_FILE);
      }

      f = fopen(RESULT_FILE, "wb");
    }
#if (IS_TABLET || IS_WATCH_PHONE)
    else if(argc == 2  && strncmp(args[1], "AAT_RESULT", 10) == 0)
    {
      if(access(AAT_RESULT_FILE, F_OK) == 0)
      {
        remove(AAT_RESULT_FILE);
      }

      f = fopen(AAT_RESULT_FILE, "wb");
    }
#endif
#if PERFORM_H2_PRJ
    else if(argc == 2 && strncmp(args[1], "H2_RESULT", 9) == 0)
    {
        if(access(H2_RESULT_FILE, F_OK) == 0)
        {
          remove(H2_RESULT_FILE);
        }

        f = fopen(H2_RESULT_FILE, "wb");
    }
#endif

#if PERFORM_AIO
    else if(argc == 2 && strncmp(args[1], "TAIO_RESULT", 11) == 0)
    {
        if(access(TAIO_RESULT_FILE, F_OK) == 0)
        {
          remove(TAIO_RESULT_FILE);
        }

        f = fopen(TAIO_RESULT_FILE, "wb");
    }
#endif
    else if((argc == 3 || argc == 4) && strncmp(args[1], "IMEI", 4) == 0)
    {

      if (argc == 3) {
        if(access(IMEI_FILE1, F_OK) == 0)
        {
          remove(IMEI_FILE1);
        }

        f = fopen(IMEI_FILE1, "wb");
      }
      else if (argc == 4) {
        if (strncmp(args[2], "1", 1) == 0) {
          if(access(IMEI_FILE1, F_OK) == 0)
          {
            remove(IMEI_FILE1);
          }

          f = fopen(IMEI_FILE1, "wb");
        }
        else if(strncmp(args[2], "2", 1) == 0) {
          if(access(IMEI_FILE2, F_OK) == 0)
          {
            remove(IMEI_FILE2);
          }

          f = fopen(IMEI_FILE2, "wb");
        }
        else if(strncmp(args[2], "3", 1) == 0) {
          if(access(IMEI_FILE3, F_OK) == 0)
          {
            remove(IMEI_FILE3);
          }

          f = fopen(IMEI_FILE3, "wb");
        }
      }
    }
    else
    {
      if(access(QRCODE_FILE, F_OK) == 0)
      {
        remove(QRCODE_FILE);
      }

      f = fopen(QRCODE_FILE, "wb");
    }

    if (f != NULL)
    {
      fwrite(&kFileHeader, sizeof(BITMAPFILEHEADER), 1, f);
      fwrite(&kInfoHeader, sizeof(BITMAPINFOHEADER), 1, f);
      fwrite(pRGBData, sizeof(unsigned char), unDataBytes, f);
      fclose(f);

#if IS_TABLET
      if(argc == 2  && (strncmp(args[1], "AAT_RESULT", 10) == 0 || strncmp(args[1], "TAIO_RESULT", 11) == 0))
      {
        FILE_PIXEL_PRESCALER = 8;
        read_pid(str);
        argc = 0;
        ALOGD("[%s] AAT_RESULT generated & needed pid qr code", LOG_TITLE);
        goto pid_qr_generation;
      }
#endif
    }
    else
    {
      ALOGD("[%s] fopen fail", LOG_TITLE);
      return -1;
    }

    //Free data
    free(pRGBData);
    QRcode_free(pQRC);
    if(str != NULL)
      free(str);
  }
  else
  {
    ALOGD("[%s] Encoding Fail", LOG_TITLE);
    return -1;
  }

  return 0;
}
