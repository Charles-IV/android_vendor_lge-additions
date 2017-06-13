/****************************************************************************
 *  (c) Copyright 2007 Wi-Fi Alliance.  All Rights Reserved
 *
 *
 *  LICENSE
 *
 *  License is granted only to Wi-Fi Alliance members and designated
 *  contractors ($B!H(BAuthorized Licensees$B!I(B)..AN  Authorized Licensees are granted
 *  the non-exclusive, worldwide, limited right to use, copy, import, export
 *  and distribute this software:
 *  (i) solely for noncommercial applications and solely for testing Wi-Fi
 *  equipment; and
 *  (ii) solely for the purpose of embedding the software into Authorized
 *  Licensee$B!G(Bs proprietary equipment and software products for distribution to
 *  its customers under a license with at least the same restrictions as
 *  contained in this License, including, without limitation, the disclaimer of
 *  warranty and limitation of liability, below..AN  The distribution rights
 *  granted in clause
 *  (ii), above, include distribution to third party companies who will
 *  redistribute the Authorized Licensee$B!G(Bs product to their customers with or
 *  without such third party$B!G(Bs private label. Other than expressly granted
 *  herein, this License is not transferable or sublicensable, and it does not
 *  extend to and may not be used with non-Wi-Fi applications..AN  Wi-Fi Alliance
 *  reserves all rights not expressly granted herein..AN
 *.AN
 *  Except as specifically set forth above, commercial derivative works of
 *  this software or applications that use the Wi-Fi scripts generated by this
 *  software are NOT AUTHORIZED without specific prior written permission from
 *  Wi-Fi Alliance.
 *.AN
 *  Non-Commercial derivative works of this software for internal use are
 *  authorized and are limited by the same restrictions; provided, however,
 *  that the Authorized Licensee shall provide Wi-Fi Alliance with a copy of
 *  such derivative works under a perpetual, payment-free license to use,
 *  modify, and distribute such derivative works for purposes of testing Wi-Fi
 *  equipment.
 *.AN
 *  Neither the name of the author nor "Wi-Fi Alliance" may be used to endorse
 *  or promote products that are derived from or that use this software without
 *  specific prior written permission from Wi-Fi Alliance.
 *
 *  THIS SOFTWARE IS PROVIDED BY WI-FI ALLIANCE "AS IS" AND ANY EXPRESS OR
 *  IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 *  OF MERCHANTABILITY, NON-INFRINGEMENT AND FITNESS FOR A.AN PARTICULAR PURPOSE,
 *  ARE DISCLAIMED. IN NO EVENT SHALL WI-FI ALLIANCE BE LIABLE FOR ANY DIRECT,
 *  INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 *  (INCLUDING, BUT NOT LIMITED TO, THE COST OF PROCUREMENT OF SUBSTITUTE
 *  GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 *  HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 *  STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE) ARISING IN ANY WAY OUT OF
 *  THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. ******************************************************************************
 */


/*
 * File: wfa_tlv.h
 *  definitions for each of command types.
 *  format:
 *  ---------------------------------------------------
 *  |  tag   | length |   value
 *  ---------------------------------------------------
 *   2 bytes   2bytes    defined by length
 */
#ifndef _WFA_TLV_H
#define _WFA_TLV_H

#include "wfa_types.h"

#define INTEGER_1 0x0001            /* byte */
#define INTEGER_2 0x0002            /* word */
#define OCTET_STRING 0x0003         /* string */

typedef struct _wfatlvHdr
{
    WORD tag;   /* tag/type */
    WORD len;   /* value length */
} wfaTLV;

#define WFA_TLV_HDR_LEN sizeof(wfaTLV)

enum cmd_tags {
/* Commands */
   WFA_GET_VERSION_TLV =                 0x01,

/* Generic TRAFFIC GENERATOR Commands */
   WFA_TRAFFIC_SEND_PING_TLV,            /* 2 */
   WFA_TRAFFIC_STOP_PING_TLV,            /* 3 */
   WFA_TRAFFIC_AGENT_CONFIG_TLV,         /* 4 */
   WFA_TRAFFIC_AGENT_SEND_TLV,           /* 5 */
   WFA_TRAFFIC_AGENT_RECV_START_TLV,     /* 6 */
   WFA_TRAFFIC_AGENT_RECV_STOP_TLV,      /* 7 */
   WFA_TRAFFIC_AGENT_RESET_TLV,          /* 8 */
   WFA_TRAFFIC_AGENT_STATUS_TLV,         /* 9 */

/* STATION/DUT Commands */
   WFA_STA_GET_IP_CONFIG_TLV,            /* 10 */
   WFA_STA_SET_IP_CONFIG_TLV,            /* 11 */
   WFA_STA_GET_MAC_ADDRESS_TLV,          /* 12 */
   WFA_STA_SET_MAC_ADDRESS_TLV,          /* 13 */
   WFA_STA_IS_CONNECTED_TLV,             /* 14 */
   WFA_STA_VERIFY_IP_CONNECTION_TLV,     /* 15 */
   WFA_STA_GET_BSSID_TLV,                /* 16 */
   WFA_STA_GET_STATS_TLV,                /* 17 */
   WFA_STA_SET_ENCRYPTION_TLV,           /* 18 */
   WFA_STA_SET_PSK_TLV,                  /* 19 */
   WFA_STA_SET_EAPTLS_TLV,               /* 20 */
   WFA_STA_SET_UAPSD_TLV,                /* 21 */
   WFA_STA_ASSOCIATE_TLV,                /* 22 */
   WFA_STA_SET_EAPTTLS_TLV,              /* 23 */
   WFA_STA_SET_EAPSIM_TLV,               /* 24 */
   WFA_STA_SET_PEAP_TLV,                 /* 25 */
   WFA_STA_SET_IBSS_TLV,                 /* 26 */
   WFA_STA_GET_INFO_TLV,                 /* 27 */
   WFA_DEVICE_GET_INFO_TLV,              /* 28 */
   WFA_DEVICE_LIST_IF_TLV,               /* 29 */

   WFA_STA_DEBUG_SET_TLV,                /* 30 */
   WFA_STA_SET_MODE_TLV,                 /* 31 */
   WFA_STA_UPLOAD_TLV,                   /* 32 */
   WFA_STA_SET_WMM_TLV,                  /* 33 */
   WFA_STA_REASSOCIATE_TLV,              /* 34 */
   WFA_STA_SET_PWRSAVE_TLV,              /* 35 */
   WFA_STA_SEND_NEIGREQ_TLV,             /* 36 */

   WFA_STA_PRESET_PARAMETERS_TLV,        /* 37 */
   WFA_STA_SET_EAPFAST_TLV,              /* 38 */
   WFA_STA_SET_EAPAKA_TLV,               /* 39 */
   WFA_STA_SET_SYSTIME_TLV,              /* 40 */

   WFA_STA_SET_11N_TLV,                  /* 41 */
   WFA_STA_SET_WIRELESS_TLV,             /* 42 */
   WFA_STA_SEND_ADDBA_TLV,               /* 43 */

   WFA_STA_SEND_COEXIST_MGMT_TLV,        /* 44 */
   WFA_STA_SET_RIFS_TEST_TLV,            /* 45 */
   WFA_STA_RESET_DEFAULT_TLV,            /* 46 */
   WFA_STA_DISCONNECT_TLV,               /* 47 */
   WFA_STA_SENDFRAME_TLV,                /* 48 */
   WFA_STA_SET_SECURITY_TLV,             /* 49 */
   WFA_STA_SET_RADIO_TLV,                /* 50 */
   WFA_STA_SET_RFEATURE_TLV,             /* 51 */
   WFA_STA_COMMANDS_END                  /* 52 */
};


enum resp_tags {
/* Version response */
   WFA_GET_VERSION_RESP_TLV = 0x01,            /* 01 */

/* Generic Traffic Generator Responses */
   WFA_TRAFFIC_SEND_PING_RESP_TLV,             /* 02 */
   WFA_TRAFFIC_STOP_PING_RESP_TLV,             /* 03 */
   WFA_TRAFFIC_AGENT_CONFIG_RESP_TLV,          /* 04 */
   WFA_TRAFFIC_AGENT_SEND_RESP_TLV,            /* 05 */
   WFA_TRAFFIC_AGENT_RECV_START_RESP_TLV,      /* 06 */
   WFA_TRAFFIC_AGENT_RECV_STOP_RESP_TLV,       /* 07 */
   WFA_TRAFFIC_AGENT_RESET_RESP_TLV,           /* 08 */
   WFA_TRAFFIC_AGENT_STATUS_RESP_TLV,          /* 09 */

/* STATION/DUT Responses */
   WFA_STA_GET_IP_CONFIG_RESP_TLV,             /* 10 */
   WFA_STA_SET_IP_CONFIG_RESP_TLV,             /* 11 */
   WFA_STA_GET_MAC_ADDRESS_RESP_TLV,           /* 12 */
   WFA_STA_SET_MAC_ADDRESS_RESP_TLV,           /* 13 */
   WFA_STA_IS_CONNECTED_RESP_TLV,              /* 14 */
   WFA_STA_VERIFY_IP_CONNECTION_RESP_TLV,      /* 15 */
   WFA_STA_GET_BSSID_RESP_TLV,                 /* 16 */
   WFA_STA_GET_STATS_RESP_TLV,                 /* 17 */
   WFA_STA_SET_ENCRYPTION_RESP_TLV,            /* 18 */
   WFA_STA_SET_PSK_RESP_TLV,                   /* 19 */
   WFA_STA_SET_EAPTLS_RESP_TLV,                /* 20 */
   WFA_STA_SET_UAPSD_RESP_TLV,                 /* 21 */
   WFA_STA_ASSOCIATE_RESP_TLV,                 /* 22 */
   WFA_STA_SET_EAPTTLS_RESP_TLV,               /* 23 */
   WFA_STA_SET_EAPSIM_RESP_TLV,                /* 24 */
   WFA_STA_SET_PEAP_RESP_TLV,                  /* 25 */
   WFA_STA_SET_IBSS_RESP_TLV,                  /* 26 */
   WFA_STA_GET_INFO_RESP_TLV,                  /* 27 */
   WFA_DEVICE_GET_INFO_RESP_TLV,               /* 28 */
   WFA_DEVICE_LIST_IF_RESP_TLV,                /* 29 */

   WFA_STA_DEBUG_SET_RESP_TLV,                 /* 30 */
   WFA_STA_SET_MODE_RESP_TLV,                  /* 31 */
   WFA_STA_UPLOAD_RESP_TLV,                    /* 32 */
   WFA_STA_SET_WMM_RESP_TLV,                   /* 33 */
   WFA_STA_REASSOCIATE_RESP_TLV,               /* 34 */
   WFA_STA_SET_PWRSAVE_RESP_TLV,               /* 35 */
   WFA_STA_SEND_NEIGREQ_RESP_TLV,              /* 36 */

   WFA_STA_PRESET_PARAMETERS_RESP_TLV,         /* 37 */
   WFA_STA_SET_EAPFAST_RESP_TLV,               /* 38 */
   WFA_STA_SET_EAPAKA_RESP_TLV,                   /* 39 */
   WFA_STA_SET_SYSTIME_RESP_TLV,               /* 40 */

   WFA_STA_SET_11N_RESP_TLV,                   /* 41 */
   WFA_STA_SET_WIRELESS_RESP_TLV,              /* 42 */
   WFA_STA_SET_SEND_ADDBA_RESP_TLV,            /* 43 */

   WFA_STA_SEND_COEXIST_MGMT_RESP_TLV,         /* 44 */
   WFA_STA_SET_RIFS_TEST_RESP_TLV,             /* 45 */
   WFA_STA_RESET_DEFAULT_RESP_TLV,             /* 46 */
   WFA_STA_DISCONNECT_RESP_TLV,                /* 47 */
   WFA_STA_SENDFRAME_RESP_TLV,                 /* 48 */
   WFA_STA_SET_SECURITY_RESP_TLV,              /* 49 */
   WFA_STA_SET_RADIO_RESP_TLV,                 /* 50 */
   WFA_STA_SET_RFEATURE_RESP_TLV,                  /* 51 */
   WFA_STA_RESPONSE_END                        /* 52 */
};

#define WFA_TLV_HEAD_LEN 1+2

extern WORD wfaGetTag(BYTE *tlv_data);
extern void wfaSetTag(BYTE *tlv_data, BYTE new_tag);
extern WORD wfaGetTLVLen(BYTE *tlv_data);
extern WORD wfaGetValueLen(BYTE *tlv_data);
extern BOOL wfaGetValue(BYTE *pstr, int value_len, BYTE *tlv_data);
extern BOOL wfaIsValidTag(BYTE the_tag);
extern void wfaAliasByTag(BYTE the_tag, char *aliasStr);
extern BOOL wfaDecodeTLV(BYTE *tlv_data, int tlv_len, WORD *ptlv_tag, int *ptlv_val_len, BYTE *ptlv_value);
extern BOOL wfaEncodeTLV(WORD the_tag, WORD the_len, BYTE *the_value, BYTE *tlv_data);

extern WORD wfaGetValueType(BYTE the_tag, BYTE *tlv_data);

#endif