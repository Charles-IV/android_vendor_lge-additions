/**
 * qrencode - QR Code encoder
 *
 * Copyright (C) 2006-2012 Kentaro Fukuchi <kentaro@fukuchi.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */
#ifndef __QRENCODE_H__
#define __QRENCODE_H__

#if defined(__cplusplus)
extern "C" {
#endif

typedef enum {
  QR_MODE_NUL = -1,  ///< Terminator (NUL character). Internal use only
  QR_MODE_NUM = 0,   ///< Numeric mode
  QR_MODE_AN,        ///< Alphabet-numeric mode
  QR_MODE_8,         ///< 8-bit data mode
  QR_MODE_KANJI,     ///< Kanji (shift-jis) mode
  QR_MODE_STRUCTURE, ///< Internal use only
  QR_MODE_ECI,       ///< ECI mode
  QR_MODE_FNC1FIRST,  ///< FNC1, first position
  QR_MODE_FNC1SECOND, ///< FNC1, second position
} QRencodeMode;

typedef enum {
  QR_ECLEVEL_L = 0, ///< lowest
  QR_ECLEVEL_M,
  QR_ECLEVEL_Q,
  QR_ECLEVEL_H      ///< highest
} QRecLevel;

#define QRSPEC_VERSION_MAX 40

#define MQRSPEC_VERSION_MAX 4

typedef struct _QRinput QRinput;

extern QRinput *QRinput_new(void);
extern QRinput *QRinput_new2(int version, QRecLevel level);
extern QRinput *QRinput_newMQR(int version, QRecLevel level);
extern int QRinput_append(QRinput *input, QRencodeMode mode, int size, const unsigned char *data);
extern int QRinput_appendECIheader(QRinput *input, unsigned int ecinum);
extern int QRinput_getVersion(QRinput *input);
extern int QRinput_setVersion(QRinput *input, int version);
extern QRecLevel QRinput_getErrorCorrectionLevel(QRinput *input);
extern int QRinput_setErrorCorrectionLevel(QRinput *input, QRecLevel level);
extern int QRinput_setVersionAndErrorCorrectionLevel(QRinput *input, int version, QRecLevel level);
extern void QRinput_free(QRinput *input);
extern int QRinput_check(QRencodeMode mode, int size, const unsigned char *data);
typedef struct _QRinput_Struct QRinput_Struct;
extern QRinput_Struct *QRinput_Struct_new(void);
extern void QRinput_Struct_setParity(QRinput_Struct *s, unsigned char parity);
extern int QRinput_Struct_appendInput(QRinput_Struct *s, QRinput *input);
extern void QRinput_Struct_free(QRinput_Struct *s);
extern QRinput_Struct *QRinput_splitQRinputToStruct(QRinput *input);
extern int QRinput_Struct_insertStructuredAppendHeaders(QRinput_Struct *s);
extern int QRinput_setFNC1First(QRinput *input);
extern int QRinput_setFNC1Second(QRinput *input, unsigned char appid);

typedef struct {
  int version;         ///< version of the symbol
  int width;           ///< width of the symbol
  unsigned char *data; ///< symbol data
} QRcode;

typedef struct _QRcode_List QRcode_List;

struct _QRcode_List {
  QRcode *code;
  QRcode_List *next;
};

extern QRcode *QRcode_encodeInput(QRinput *input);
extern QRcode *QRcode_encodeString(const char *string, int version, QRecLevel level, QRencodeMode hint, int casesensitive);
extern QRcode *QRcode_encodeString8bit(const char *string, int version, QRecLevel level);
extern QRcode *QRcode_encodeStringMQR(const char *string, int version, QRecLevel level, QRencodeMode hint, int casesensitive);
extern QRcode *QRcode_encodeString8bitMQR(const char *string, int version, QRecLevel level);
extern QRcode *QRcode_encodeData(int size, const unsigned char *data, int version, QRecLevel level);
extern QRcode *QRcode_encodeDataMQR(int size, const unsigned char *data, int version, QRecLevel level);
extern void QRcode_free(QRcode *qrcode);
extern QRcode_List *QRcode_encodeInputStructured(QRinput_Struct *s);
extern QRcode_List *QRcode_encodeStringStructured(const char *string, int version, QRecLevel level, QRencodeMode hint, int casesensitive);
extern QRcode_List *QRcode_encodeString8bitStructured(const char *string, int version, QRecLevel level);
extern QRcode_List *QRcode_encodeDataStructured(int size, const unsigned char *data, int version, QRecLevel level);
extern int QRcode_List_size(QRcode_List *qrlist);
extern void QRcode_List_free(QRcode_List *qrlist);
extern void QRcode_APIVersion(int *major_version, int *minor_version, int *micro_version);
extern char *QRcode_APIVersionString(void);
extern void QRcode_clearCache(void);

#if defined(__cplusplus)
}
#endif

#endif /* __QRENCODE_H__ */
