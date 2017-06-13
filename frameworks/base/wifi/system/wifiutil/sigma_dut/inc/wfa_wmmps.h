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
 * * APTS messages/tests
 * */
#define B_D        1
#define    B_H            2
#define B_B             3
#define B_M             4
#define M_D             5
#define B_Z             6
#define M_Y             7
#define    L_1        8
#define    A_Y        9        // active mode version of M_Y
#define    B_W        10        //
#define    A_J        11        // Active test of sending 4 down
#define M_V             12
#define M_U             13
#define A_U             14
#define M_L             15
#define B_K             16
#define M_B             17
#define M_K             18
#define M_W             19
#ifdef WFA_WMM_AC
#define WMMAC_422_T02B            20
#define WMMAC_422_T03A            21
#define WMMAC_422_T04B            22
#define WMMAC_422_T05B            23
#define WMMAC_422_T06B            24
#define WMMAC_422_T07B            25
#define WMMAC_422_T08B            26

#define WMMAC_423_T04             27
#define WMMAC_424_T07t14          28
#define WMMAC_425_T04t06          29

#define WMMAC_521_T03             30
#define WMMAC_521_T05             31

#define WMMAC_522_T04             32
#define WMMAC_522_T06             33
#define WMMAC_522_T06o            34
#define WMMAC_524_T03             35
#define WMMAC_524_T03i            36
#define WMMAC_525_T07t10          37

#define LAST_TEST       WMMAC_525_T07t10
#else
#define LAST_TEST       M_W
#endif
#define    APTS_DEFAULT    (LAST_TEST + 0x01)        // message codes
#define    APTS_HELLO    (APTS_DEFAULT + 0x01)
#define    APTS_BCST    (APTS_HELLO + 0x01)
#define    APTS_CONFIRM    (APTS_BCST + 0x01)
#define    APTS_STOP    (APTS_CONFIRM + 0x01)
#define APTS_CK_BE      (APTS_STOP + 0x01)
#define APTS_CK_BK      (APTS_CK_BE + 0x01)
#define APTS_CK_VI      (APTS_CK_BK + 0x01)
#define APTS_CK_VO      (APTS_CK_VI + 0x01)
#define APTS_RESET      (APTS_CK_VO + 0x01)
#define APTS_RESET_RESP (APTS_RESET + 0x01)
#define APTS_RESET_STOP (APTS_RESET_RESP + 0x01)
#define APTS_LAST       99
#define    MAXRETRY    3
#define    MAXHELLO    20
#define MAX_STOPS 10
#define    NTARG    32            // number of target names
#define EQ(a,b)     (strcmp(a,b)==0)

#define PORT            12345          // use a common port for sending/receiving
#define   LII  2000000
#ifdef WFA_WMM_AC
/*Listen Inteval for station,to be changed to the actual value*/
#define   lis_int  500000
#define   becon_int  100000
#endif
#define NTARG           32

#define WFA_DEFAULT_CODEC_SEC         0
#define WFA_DEFAULT_CODEC_USEC        10000

#define WFA_WMMPS_UDP_PORT            12345        // it must sync with console

/* wmm defs */
#define TOS_VO7     0xE0                // 111 0  0000 (7)  AC_VO tos/dscp values
#define TOS_VO      0xD0                // 110 0  0000 (6)  AC_VO tos/dscp values
//Atheros Madwifi use 0x88 for UPSD/Voice
//#define TOS_VO      0x88               // 110 0  0000 (6)  AC_VO tos/dscp values

#define TOS_VI      0xA0                // 101 0  0000 (5)  AC_VI
#define TOS_VI4     0x80                // 100 0  0000 (4)  AC_VI

#define TOS_BE      0x00                // 000 0  0000 (0)  AC_BE
#define TOS_EE      0x60                // 011 0  0000 (3)  AC_BE

#define TOS_BK      0x20                // 001 0  0000 (1)  AC_BK
#define TOS_LE      0x40                // 010 0  0000 (2)  AC_BK


/*
 * * power management
 * */
#define PS_ON    1
#define PS_OFF   0
#define P_ON    1
#define P_OFF   0

#if WFA_DEBUG
#define PRINTF(s,args...) printf(s,## args)
#else
#define PRINTF(s,args...) ;
#endif

/*
 * * internal table
 * */
struct apts_msg {                       //
    char *name;                     // name of test
    int cmd;                        // msg num
    int param0;                     // number of packet exchanges
    int param1;                     // number of uplink frames
    int param2;                     // number of downlink frames
    int param3;
};

/*
 * * Wait/Timer states
 * */
typedef enum {
    WFA_WAIT_NEXT_CODEC,
    WFA_WAIT_FOR_AP_RESPONSE,
    WFA_WAIT_STAUT_00,
    WFA_WAIT_STAUT_01,
    WFA_WAIT_STAUT_02,
    WFA_WAIT_STAUT_03,
    WFA_WAIT_STAUT_04,
    WFA_WAIT_STAUT_0E,
    WFA_WAIT_STAUT_VOLOAD,
    WFA_WAIT_STAUT_SEQ,
} WAIT_MODE;

typedef struct wfa_wmmps
{
    int my_sta_id;
    int my_group_cookie;
    int my_cookie;
    int thr_flag;
    int sta_state;
    int sta_test;
    int wait_state;
    int nextsleep;
    int nsent;
    int msgno;
    int ps_thread;
    int rcv_state;
    int dscp;
    int reset;
    int streamid;
    tgThrData_t *tdata;
    struct sockaddr_in psToAddr;
    pthread_t thr;
    pthread_cond_t thr_flag_cond;
    pthread_mutex_t thr_flag_mutex;

}wfaWmmPS_t;



int WfaStaSndHello(char,int,int *state);
int WfaStaSndConfirm(char,int,int *state);
int WfaStaSndVO(char,int,int *state);
int WfaStaSndVOCyclic(char,int,int *state);
int WfaStaSnd2VO(char,int,int *state);
int WfaStaWaitStop(char,int,int *state);
int WfaStaSndVI(char,int,int *state);
int WfaStaSndBE(char,int,int *state);
int WfaStaSndBK(char,int,int *state);
int WfaStaSndVIE(char,int,int *state);
//int WfaStaSndVO(char,int,int *state);
int WfaStaSndBEE(char,int,int *state);
int WfaStaSnd2VOE(char,int,int *state);
void create_apts_msg(int msg, unsigned int txbuf[],int id);
int WfaRcvStop(unsigned int *,int ,int *);
int WfaRcvVO(unsigned int *,int ,int *);
int WfaRcvProc(unsigned int *,int ,int *);
int WfaRcvVOCyclic(unsigned int *,int ,int *);
int WfaRcvVI(unsigned int *,int ,int *);
int WfaRcvBE(unsigned int *,int ,int *);
int WfaRcvBK(unsigned int *,int ,int *);
int WfaRcvNotCare(unsigned int *,int ,int *);
