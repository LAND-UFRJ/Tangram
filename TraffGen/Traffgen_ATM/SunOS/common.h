/*
 * $Id: common.h,v 1.1.1.1 2005/01/19 16:14:32 develop Exp $
 *
 * Copyright (c) 1997-1999 FORE Systems, Inc., as an unpublished work.
 * This notice does not imply unrestricted or public access to these
 * materials which are a trade secret of FORE Systems, Inc. or its
 * subsidiaries or affiliates (together referred to as "FORE"), and
 * which may not be reproduced, used, sold or transferred to any third
 * party without FORE's prior written consent.
 *
 * All rights reserved.
 *
 * U.S. Government Restricted Rights.  If you are licensing the
 * Software on behalf of the U.S. Government ("Government"), the
 * following provisions apply to you.  If the Software is supplied to
 * the Department of Defense ("DoD"), it is classified as "Commercial
 * Computer Software" under paragraph 252.227-7014 of the DoD Supplement
 * to the Federal Acquisition Regulations ("DFARS") (or any successor
 * regulations) and the Government is acquiring only the license
 * rights granted herein (the license rights customarily provided to
 * non-Government users).  If the Software is supplied to any unit
 * or agency of the Government other than DoD, it is classified as
 * "Restricted Computer Software" and the Government's rights in the
 * Software are defined in paragraph 52.227-19 of the Federal Acquisition
 * Regulations ("FAR") (or any successor regulations) or, in the cases
 * of NASA, in paragraph 18.52.227-86 of the NASA Supplement to the FAR
 * (or any successor regulations).
 */

/*
 * common.h: definitions for XTI example programs
 */

#ifdef __cplusplus
extern "C" {
#endif

#include <fore_xti/xti_user_types.h>
#include <fore_xti/xti_atm.h>
#include <fore_xti/xti.h>


#ifndef __GENPACKET__
#define __GENPACKET__

/* Constants to the generation of packets of data and finalize*/
#define END 'E'
#define DATA 'D'

/* Class of services */
#define CBR    1
#define UBR    2
#define ABR    3
#define VBR    4


#define MINPACKSIZE  32
#define MAXPACKSIZE  ( (64*1024) - MINPACKSIZE )

typedef struct{
    u_longlong_t   nseq;                  /* Sequence number */
    char           type;                  /* Type of packet  */
    u_longlong_t   timestamp_tx;          /* Timestamp in msec */
    u_longlong_t   timestamp_rx;          /* Timestamp in msec */
    char           payload[MAXPACKSIZE];  /* payload */
} t_gen_packet;

#endif


typedef struct uservc_qos {
    uint32_t qosclass;	    /* QoS class */
    int32_t pcr;	    /* peak cell rate (cells/sec) */
    int32_t scr;	    /* sustained cell rate (cells/sec) */
    int32_t mbs;	    /* maximum burst size (cells) */
    int32_t mcr;	    /* minimum cell rate (cells/sec) */
    int32_t icr;	    /* initial cell rate (cells/sec) */
} uservc_qos_t;

typedef enum {
    vc_dir_absent   = -1,               /* T_ATM_ABSENT */
    vc_dir_incoming = 0x1,              /* incoming VC */
    vc_dir_outgoing = 0x2,              /* outgoing VC */
    vc_dir_duplex = 0x3                 /* bidirectional VC */
} vc_dir_t;


/*
 * user configuration structure
 */
typedef struct usercfg_opts {
    int aal;
    int vpi;
    int vci;
    int batchsize;
    vc_dir_t vc_direction;
    uservc_qos_t vc_qos;
    int flags;
    long optlen;
} usercfg_opts_t;

/*
 * Commonly used definitions
 */

#define OC3_LINE_RATE	353207	/* OC-3 */
#define OC12_LINE_RATE  1412830 /* OC-12 */
#define OPT_FLAGS_CPID	0x1
#define OPT_FLAGS_PMP	0x2

#define SVC_DEVICE	"/dev/xtisvc0"
#define PVC_DEVICE	"/dev/xtipvc0"
#define VPI		0
#define VCI		100
#define SELECTOR	0x99
#define PDU_LEN		8192
#define NUM_PDUS	50000
#define AAL		5
#define QOS		0

/*
 * external functions from common.c
 */

int open_device(char *device);
void clean_up(int fd);

int svc_bind(int fd, unsigned char selector, int qlen, char *addr);
int svc_bind_initiator(int fd);
int svc_bind_listener(int fd, unsigned char selector);
int svc_connect(int fd, char *hostname, unsigned char selector,
		char *options, size_t optlen);
int svc_disconnect(int fd);
void svc_recv_disconnect(int fd);
int svc_accept(int fd, int batchsize);
int svc_bind_listener(int fd, unsigned char selector);
int svc_bind_initiator(int fd);

int pvc_bind(int fd);
int pvc_connect(int fd, int vpi, int vci, char *options, size_t optlen);
int pvc_disconnect(int fd);

void send_data(int fd, int len, int num);
void recv_data(int fd, int len, int num);

char *construct_options(int fd, usercfg_opts_t *user_optionsP);
char *construct_pvc_options(int fd, usercfg_opts_t *user_optionsP);

int pmp_connect(int fd, char *hostname, unsigned char selector, int leaf_id);
int pmp_disconnect(int fd, int leaf_id);

#ifdef aix
int t_getprotoaddr(int fd, struct t_bind *laddr, struct t_bind *raddr);
#else /* aix */
int t_getname(int fd, struct netbuf *addr, int type);
#endif /* aix */

#ifdef __cplusplus
}
#endif
