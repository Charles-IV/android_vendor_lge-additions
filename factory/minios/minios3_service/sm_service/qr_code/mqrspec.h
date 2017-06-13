/*
 * qrencode - QR Code encoder
 *
 * Micro QR Code specification in convenient format.
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

#ifndef __MQRSPEC_H__
#define __MQRSPEC_H__

#include "qrencode.h"

#define MQRSPEC_WIDTH_MAX 17

extern int MQRspec_getDataLengthBit(int version, QRecLevel level);
extern int MQRspec_getDataLength(int version, QRecLevel level);
extern int MQRspec_getECCLength(int version, QRecLevel level);
extern int MQRspec_getMinimumVersion(int size, QRecLevel level);
extern int MQRspec_getWidth(int version);
extern int MQRspec_getRemainder(int version);
extern int MQRspec_lengthIndicator(QRencodeMode mode, int version);
extern int MQRspec_maximumWords(QRencodeMode mode, int version);
extern unsigned int MQRspec_getVersionPattern(int version);
extern unsigned int MQRspec_getFormatInfo(int mask, int version, QRecLevel level);
extern unsigned char *MQRspec_newFrame(int version);
extern void MQRspec_clearCache(void);

#define MQRSPEC_MODEID_NUM       0
#define MQRSPEC_MODEID_AN        1
#define MQRSPEC_MODEID_8         2
#define MQRSPEC_MODEID_KANJI     3

#endif /* __MQRSPEC_H__ */
