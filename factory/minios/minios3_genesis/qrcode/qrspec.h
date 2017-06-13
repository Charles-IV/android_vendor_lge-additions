/*
 * qrencode - QR Code encoder
 *
 * QR Code specification in convenient format.
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

#ifndef __QRSPEC_H__
#define __QRSPEC_H__

#include "qrencode.h"

#define QRSPEC_WIDTH_MAX 177

extern int QRspec_getDataLength(int version, QRecLevel level);
extern int QRspec_getECCLength(int version, QRecLevel level);
extern int QRspec_getMinimumVersion(int size, QRecLevel level);
extern int QRspec_getWidth(int version);
extern int QRspec_getRemainder(int version);
extern int QRspec_lengthIndicator(QRencodeMode mode, int version);
extern int QRspec_maximumWords(QRencodeMode mode, int version);
void QRspec_getEccSpec(int version, QRecLevel level, int spec[5]);

#define QRspec_rsBlockNum(__spec__) (__spec__[0] + __spec__[3])
#define QRspec_rsBlockNum1(__spec__) (__spec__[0])
#define QRspec_rsDataCodes1(__spec__) (__spec__[1])
#define QRspec_rsEccCodes1(__spec__) (__spec__[2])
#define QRspec_rsBlockNum2(__spec__) (__spec__[3])
#define QRspec_rsDataCodes2(__spec__) (__spec__[4])
#define QRspec_rsEccCodes2(__spec__) (__spec__[2])

#define QRspec_rsDataLength(__spec__) \
  ((QRspec_rsBlockNum1(__spec__) * QRspec_rsDataCodes1(__spec__)) + \
   (QRspec_rsBlockNum2(__spec__) * QRspec_rsDataCodes2(__spec__)))
#define QRspec_rsEccLength(__spec__) \
  (QRspec_rsBlockNum(__spec__) * QRspec_rsEccCodes1(__spec__))

extern unsigned int QRspec_getVersionPattern(int version);
extern unsigned int QRspec_getFormatInfo(int mask, QRecLevel level);
extern unsigned char *QRspec_newFrame(int version);
extern void QRspec_clearCache(void);

#define QRSPEC_MODEID_ECI        7
#define QRSPEC_MODEID_NUM        1
#define QRSPEC_MODEID_AN         2
#define QRSPEC_MODEID_8          4
#define QRSPEC_MODEID_KANJI      8
#define QRSPEC_MODEID_FNC1FIRST  5
#define QRSPEC_MODEID_FNC1SECOND 9
#define QRSPEC_MODEID_STRUCTURE  3
#define QRSPEC_MODEID_TERMINATOR 0

#endif /* __QRSPEC_H__ */
