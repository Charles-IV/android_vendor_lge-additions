/*
 * qrencode - QR Code encoder
 *
 * Input data chunk class
 * Copyright (C) 2006-2011 Kentaro Fukuchi <kentaro@fukuchi.org>
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

#ifndef __QRINPUT_H__
#define __QRINPUT_H__

#include "qrencode.h"
#include "bitstream.h"

int QRinput_isSplittableMode(QRencodeMode mode);

typedef struct _QRinput_List QRinput_List;

struct _QRinput_List {
  QRencodeMode mode;
  int size;
  unsigned char *data;
  BitStream *bstream;
  QRinput_List *next;
};

struct _QRinput {
  int version;
  QRecLevel level;
  QRinput_List *head;
  QRinput_List *tail;
  int mqr;
  int fnc1;
  unsigned char appid;
};

typedef struct _QRinput_InputList QRinput_InputList;

struct _QRinput_InputList {
  QRinput *input;
  QRinput_InputList *next;
};

struct _QRinput_Struct {
  int size;
  int parity;
  QRinput_InputList *head;
  QRinput_InputList *tail;
};

extern unsigned char *QRinput_getByteStream(QRinput *input);


extern int QRinput_estimateBitsModeNum(int size);
extern int QRinput_estimateBitsModeAn(int size);
extern int QRinput_estimateBitsMode8(int size);
extern int QRinput_estimateBitsModeKanji(int size);

extern QRinput *QRinput_dup(QRinput *input);

extern const signed char QRinput_anTable[128];

#define QRinput_lookAnTable(__c__) \
  ((__c__ & 0x80)?-1:QRinput_anTable[(int)__c__])

#define MODE_INDICATOR_SIZE 4

#define STRUCTURE_HEADER_SIZE 20

#define MAX_STRUCTURED_SYMBOLS 16

#ifdef WITH_TESTS
extern BitStream *QRinput_mergeBitStream(QRinput *input);
extern BitStream *QRinput_getBitStream(QRinput *input);
extern int QRinput_estimateBitStreamSize(QRinput *input, int version);
extern int QRinput_splitEntry(QRinput_List *entry, int bytes);
extern int QRinput_lengthOfCode(QRencodeMode mode, int version, int bits);
extern int QRinput_insertStructuredAppendHeader(QRinput *input, int size, int index, unsigned char parity);
#endif

#endif /* __QRINPUT_H__ */