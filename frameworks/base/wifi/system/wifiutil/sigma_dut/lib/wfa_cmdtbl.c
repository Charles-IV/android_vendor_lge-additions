
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
 * File: wfa_cmdtbl.c
 *   The file contains a predefined function array. The command process and
 *   execution functions of a DUT traffic generator and control will be
 *   registered in the array/table by the order of the defined commands TLV
 *   values.
 *
 *   Revision History:
 *      2006/03/10 -- initially created by qhu
 *      2006/06/01 -- BETA release by qhu
 *      2006/06/13 -- 00.02 release by qhu
 *      2006/06/30 -- 00.10 Release by qhu
 *      2006/07/10 -- 01.00 Release by qhu
 *      2006/09/01 -- 01.05 Release by qhu
 *      2007/02/15  -- WMM Extension Beta released by qhu, mkaroshi
 *      2007/03/30 -- 01.40 WPA2 and Official WMM Beta release by qhu
 *      2007/04/20 -- 02.00 WPA2 and Official WMM release by qhu
 *      2007/08/15 --  02.10 WMM-Power Save release by qhu
 *      2007/10/10 --  02.20 Voice SOHO beta -- qhu
 *      2007/11/07 --  02.30 Voice HSO -- qhu
 *      2007/12/10 --  02.32 Add a function to upload test results.
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include "wfa_debug.h"
#include "wfa_types.h"
#include "wfa_main.h"
#include "wfa_tlv.h"
#include "wfa_tg.h"
#include "wfa_miscs.h"
#include "wfa_ca.h"
#include "wfa_sock.h"
#include "wfa_agt.h"
#include "wfa_rsp.h"

/* extern defined variables */
extern int gxcSockfd, btSockfd;

int NotDefinedYet(int len, unsigned char *params, int *respLen, BYTE *respBuf);
extern int agtCmdProcGetVersion(int len, BYTE *parms, int *respLen, BYTE *respBuf);

extern unsigned short wfa_defined_debug;

/* globally define the function table */
xcCommandFuncPtr gWfaCmdFuncTbl[WFA_STA_COMMANDS_END] =
{
    /* Traffic Agent Commands */
    NotDefinedYet,            /*    None                               (0) */
    agtCmdProcGetVersion,     /*    WFA_GET_VERSION_TLV                (1) */
    wfaTGSendPing,            /*    WFA_TRAFFIC_SEND_PING_TLV          (2) */
    wfaTGStopPing,            /*    WFA_TRAFFIC_STOP_PING_TLV          (3) */
    wfaTGConfig,              /*    WFA_TRAFFIC_AGENT_CONFIG_TLV       (4) */
    wfaTGSendStart,           /*    WFA_TRAFFIC_AGENT_SEND_TLV         (5) */
    wfaTGRecvStart,           /*    WFA_TRAFFIC_AGENT_RECV_START_TLV   (6) */
    wfaTGRecvStop,            /*    WFA_TRAFFIC_AGENT_RECV_STOP_TLV    (7) */
    wfaTGReset,               /*    WFA_TRAFFIC_AGENT_RESET_TLV        (8) */
    NotDefinedYet,            /*    WFA_TRAFFIC_AGENT_STATUS_TLV       (9) */

    /* Control and Configuration Commands */
    wfaStaGetIpConfig,        /*    WFA_STA_GET_IP_CONFIG_TLV          (10)*/
    wfaStaSetIpConfig,        /*    WFA_STA_SET_IP_CONFIG_TLV          (11)*/
    wfaStaGetMacAddress,      /*    WFA_STA_GET_MAC_ADDRESS_TLV        (12)*/
    wfaStaSetMacAddr,         /*    WFA_STA_SET_MAC_ADDRESS_TLV        (13)*/
    wfaStaIsConnected,        /*    WFA_STA_IS_CONNECTED_TLV           (14)*/
    wfaStaVerifyIpConnection, /*    WFA_STA_VERIFY_IP_CONNECTION_TLV   (15)*/
    wfaStaGetBSSID,           /*    WFA_STA_GET_BSSID_TLV              (16)*/
    wfaStaGetStats,           /*    WFA_STA_GET_STATS_TLV              (17)*/
    wfaSetEncryption,         /*    WFA_STA_SET_ENCRYPTION_TLV         (18)*/
    wfaStaSetPSK,             /*    WFA_STA_SET_PSK_TLV                (19)*/
    wfaStaSetEapTLS,          /*    WFA_STA_SET_EAPTLS_TLV             (20)*/
    wfaStaSetUAPSD,           /*    WFA_STA_SET_UAPSD_TLV              (21)*/
    wfaStaAssociate,          /*    WFA_STA_ASSOCIATE_TLV              (22)*/
    wfaStaSetEapTTLS,         /*    WFA_STA_SET_EAPTTLS_TLV            (23)*/
    wfaStaSetEapSIM,          /*    WFA_STA_SET_EAPSIM_TLV             (24)*/
    wfaStaSetPEAP,            /*    WFA_STA_SET_PEAP_TLV               (25)*/
    wfaStaSetIBSS,            /*    WFA_STA_SET_IBSS_TLV               (26)*/
    wfaStaGetInfo,            /*    WFA_STA_GET_INFO_TLV               (27)*/
    wfaDeviceGetInfo,         /*    WFA_DEVICE_GET_INFO_TLV            (28)*/
    wfaDeviceListIF,          /*    WFA_DEVICE_LIST_IF_TLV]            (29)*/
    wfaStaDebugSet,           /*    WFA_STA_DEBUG_SET                  (30)*/
    wfaStaSetMode,            /*    WFA_STA_SET_MODE                   (31)*/
    wfaStaUpload,             /*    WFA_STA_UPLOAD                     (32)*/
    wfaStaSetWMM,             /*    WFA_STA_SETWMM                     (33)*/
    wfaStaReAssociate,        /*    WFA_STA_REASSOCIATE                (34)*/
    wfaStaSetPwrSave,         /*    WFA_STA_SET_PWRSAVE                (35)*/
#ifdef WFA_STA_TB
    wfaStaSendNeigReq,        /*    WFA_STA_SEND_NEIGREQ               (36)*/
    wfaStaPresetParams,       /*    WFA_STA_PRESET_PARAMETERS          (37)*/
#else
    wfaStaTestBedCmd,         /*    WFA_STA_SEND_NEIGREQ               (36)*/
    wfaStaTestBedCmd,         /*    WFA_STA_PRESET_PARAMETERS          (37)*/
#endif
    wfaStaSetEapFAST,          /*    WFA_STA_SET_EAPFAST_TLV           (38)*/
    wfaStaSetEapAKA,          /*    WFA_STA_SET_EAPAKA_TLV             (49)*/
    wfaStaSetSystime,          /*    WFA_STA_SET_SYSTIME_TLV           (40)*/
#ifdef WFA_STA_TB
    wfaStaSet11n,            /*    WFA_STA_SET_11n_TLV                   (41)*/
    wfaStaSetWireless,          /*    WFA_STA_SET_WIRELESS_TLV           (42)*/
    wfaStaSendADDBA,          /*    WFA_STA_SEND_ADDBA_TLV           (43)*/
    wfaStaSendCoExistMGMT,    /*    WFA_STA_SET_COEXIST_MGMT_TLV       (44)*/
    wfaStaSetRIFS,            /*    WFA_STA_SET_RIFS_TEST_TLV          (45)*/
    wfaStaResetDefault,       /*    WFA_STA_RESET_DEFAULT_TLV          (46)*/
    wfaStaDisconnect,         /*    WFA_STA_DISCONNECT_TLV             (47)*/
#else
    wfaStaTestBedCmd,            /*    WFA_STA_SET_11n_TLV                   (41)*/
    wfaStaTestBedCmd,          /*    WFA_STA_SET_WIRELESS_TLV           (42)*/
    wfaStaTestBedCmd,          /*    WFA_STA_SEND_ADDBA_TLV           (43)*/
    wfaStaTestBedCmd,         /*    WFA_STA_SET_COEXIST_MGMT_TLV       (44)*/
    wfaStaTestBedCmd,         /*    WFA_STA_SET_RIFS_TEST_TLV          (45)*/
    wfaStaTestBedCmd,         /*    WFA_STA_RESET_DEFAULT_TLV          (46)*/
    wfaStaDisconnect,         /*    WFA_STA_DISCONNECT_TLV             (47)*/
#endif
    wfaStaSendFrame,          /*    WFA_STA_SENDFRAME_TLV              (48)*/
    wfaStaSetSecurity,        /*    WFA_STA_SET_SECURITY_TLV           (49)*/
    wfaStaSetRadio,           /*    WFA_STA_SET_RADIO_TLV              (50)*/
    wfaStaSetRFeature,        /*    WFA_STA_RFEATURE_TLV               (51)*/
};


/*
 * NotDefinedYet(): a dummy function
 */
int NotDefinedYet(int len, unsigned char *params, int *respLen, BYTE *respBuf)
{
    DPRINT_WARNING(WFA_WNG, "The command processing function not defined.\n");


    /* need to send back a response */

    return WFA_SUCCESS;
}
