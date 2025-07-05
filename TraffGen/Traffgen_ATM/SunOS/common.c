/*
 * $Id: common.c,v 1.1.1.1 2005/01/19 16:14:32 develop Exp $
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
 * common.c: common routines for XTI example programs
 */

#include <poll.h>
#include <sys/param.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stropts.h>
#include <unistd.h>
#ifdef aix
#include <fcntl.h>
#else
#include <sys/fcntl.h>
#endif /* aix */
#include <sys/stream.h>
#include <sys/time.h>

#include <fore_xti/xti_user_types.h>
#include <fore_xti/xti.h>
#include <fore_xti/xti_atm.h>
#include <fore_xti/netatm/atm.h>
#include <fore_xti/ans.h>
#include <errno.h>
extern int errno;

#include "common.h"

#if defined(aix)
#undef PRIuMAX
#define PRIuMAX "llu"
#ifndef _H_INTTYPES
typedef unsigned long long uintmax_t;
#endif /* _H_INTTYPES */
#endif /* aix */
#if defined(sgi)
#define PRIuMAX "llu"
#endif /* sgi */
#if defined(SunOS5)
#ifndef _SYS_INT_TYPES_H
#define PRIuMAX "llu"
typedef unsigned long long uintmax_t;
#else
#include <inttypes.h>
#endif /* _SYS_INT_TYPES_H */
#endif /* SunOS5 */

typedef unsigned long long uintmax_t;

/* internal function prototypes */
static int get_local_address(int fd, unsigned char addr[]);
static char *sap_to_string(ATMSAPAddress addr);
static int handle_error(int fd, char *func);
static int get_cause(int fd);
static int get_option_value(int fd, int name, char *opt_return, int opt_size);
static char *causemsg(int causenumber);

/* simple timing macros */
#define GET_TIME(x)	(gettimeofday(&x, (struct timezone *) NULL))
#define SUB_TIME(s,e)	((double) ((e.tv_sec - s.tv_sec) + \
				   ((e.tv_usec - s.tv_usec) / 1000000.0)))
#define ADJ_TIME(x,t)	((double) (x - t/1000.0))
#define CELL_BITS	(53 * 8)
#define LONG_WAIT	20		/* seconds */
#define SHORT_WAIT	3		/* seconds */

/* Globals */
char	     device_name[30] = " "; /* Name of device being opened*/
uint32_t    hw_type = 0;	    /* Adapter card hardware type */

/* BLHI (8 octet) userid for our app.  We use this for SVC connections to
 * help ensure that the address we're listening on is unique, even if another
 * app is using the same selector.
 */

#define BHLI_MAGIC "FORE_ATM"


/*
 * open_device(device)
 *
 * Opens the transport device.
 *
 * In:
 *	char *device	pathname of device to open
 *
 * Returns:
 *	int		the return value from t_open()
 */
int
open_device(char *device)
{
    int fd;
    struct t_info info;

    printf("Opening device %s...\n", device);

    fd = t_open(device, O_RDWR, &info);

    if (fd < 0) {
	t_error("t_open");
    } else {
	/* max AAL5 PDU length = 65535 bytes */
	printf("Maximum TSDU length is %ld\n", info.tsdu);
	/* this is large enough to hold all possible options */
	printf("Maximum options length is %ld\n\n", info.options);
	strcpy(device_name, device);
    }

    return fd;
}

/*
 * clean_up(fd)
 *
 * Unbinds and closes the given endpoint.
 *
 * In:
 *	int fd		fd to clean up
 */
void
clean_up(int fd)
{
    int event=0;
    printf("Unbinding and closing...\n");

    if (t_unbind(fd) < 0 && t_errno == TLOOK) {
	event = t_look(fd);
    }

    if (event && event != T_DATA) {
	handle_error(fd,"t_unbind");
    }

    if (t_close(fd) < 0) {
	t_error("t_close");
    }
}

/*
 * svc_bind_listener(fd, selector)
 *
 * Binds the local endpoint of an SVC listening process.
 *
 * In:
 *	int fd			fd for transport endpoint to bind
 *	u_char selector		local selector bytes to listen on
 *
 * Returns:
 *	int			0	on success
 *				-1	on failure
 *
 * Notes:
 *	The FORE CLIP listener consumes selector bytes 0x00-0x03.
 *	These should not be used by user applications.
 */
int
svc_bind_listener(int fd, unsigned char selector)
{
    struct t_bind bind_req;
    ATMSAPAddress addr_req;
    struct t_atm_card_prop card_prop;

    addr_req.addressType = AF_ATM;

    /*
     * Note: this approach may be FORE implementation-specific.
     *
     * When we bind with tag_addr ABSENT and tag_selector PRESENT,
     * only the selector (i.e. address[19]) is used by the TP.	The rest
     * of the local address is filled in by the TP and can be obtained
     * via the 'ret' parameter or with t_getname()/t_getprotaddr().
     */

    addr_req.sap.t_atm_sap_addr.SVE_tag_addr = (int8_t)T_ATM_ABSENT;
    addr_req.sap.t_atm_sap_addr.SVE_tag_selector = (int8_t)T_ATM_PRESENT;

    addr_req.sap.t_atm_sap_addr.address_format = (uint8_t)T_ATM_ENDSYS_ADDR;
    addr_req.sap.t_atm_sap_addr.address_length = 20;
    addr_req.sap.t_atm_sap_addr.address[19] = selector;

    addr_req.sap.t_atm_sap_layer2.SVE_tag = (int8_t)T_ATM_ABSENT;
    addr_req.sap.t_atm_sap_layer3.SVE_tag = (int8_t)T_ATM_ABSENT;

    /* this is optional; we use BHLI (appl) ID here to avoid conflicts */
    addr_req.sap.t_atm_sap_appl.SVE_tag = (int8_t)T_ATM_PRESENT;
    addr_req.sap.t_atm_sap_appl.ID_type = (uint8_t)T_ATM_USER_APP_ID;
    memcpy(addr_req.sap.t_atm_sap_appl.ID.user_defined_ID, BHLI_MAGIC, 8);

    memset(&bind_req, 0, sizeof(bind_req));
    bind_req.qlen = 1;
    bind_req.addr.len = sizeof(addr_req);
    bind_req.addr.buf = (char *) &addr_req;

    printf("Binding SVC listening endpoint, SEL=%#x...\n",selector);

    if (t_bind(fd, &bind_req, (struct t_bind *) NULL) < 0) {
	t_error("t_bind");
	return -1;
    }

    if (get_option_value(fd, T_ATM_CARD_PROP,
			(char *)&card_prop,
			sizeof(t_atm_card_prop)) != 0) {
	fprintf(stderr, "Unable to obtain ATM Card properties.\n");
    } else {
	hw_type = card_prop.hw_type;
    }

    return 0;
}

/*
 * svc_bind_initiator(fd)
 *
 * Binds the local endpoint of an SVC initiating process.
 *
 * In:
 *	int fd		fd for transport endpoint to bind
 *
 * Returns:
 *	int		0	on success
 *			-1	on failure
 */
int
svc_bind_initiator(int fd)
{
    /* For the initiator it is suggested to set 'req' to NULL, since it is
     * ignored.	 The local address can be retrieved through 'ret', or later
     * with t_getname()/t_getprotaddr().
     */

    struct t_bind bind_ret;
    ATMSAPAddress addr_ret;
    struct t_atm_card_prop card_prop;

    memset(&bind_ret, 0, sizeof(bind_ret));
    bind_ret.addr.maxlen= sizeof(addr_ret);
    bind_ret.addr.buf	= (char *) &addr_ret;

    printf("Binding SVC endpoint...\n\n");

    if (t_bind(fd, (struct t_bind *) NULL, &bind_ret) < 0) {
	t_error("t_bind");
	return -1;
    }

    printf("Initiator local address: %s\n", sap_to_string(addr_ret));

    if (get_option_value(fd, T_ATM_CARD_PROP,
			(char *)&card_prop,
			sizeof(t_atm_card_prop)) != 0) {
	fprintf(stderr, "Unable to obtain ATM Card properties.\n");
    } else {
	hw_type = card_prop.hw_type;
    }

    return 0;
}

/*
 * svc_connect(fd,hostname,selector,options,optlen)
 *
 * Connects to an SVC listener.
 *
 * In:
 *	int fd		fd for transport endpoint
 *	char *hostname	name of remote host
 *	u_char selector	selector byte for remote host
 *	char *options	buffer containing options to pass to t_connect
 *	size_t optlen	length of options buffer
 *		  (if options is NULL or optlen is 0, no options are used)
 *
 * Returns:
 *	int		0	on success
 *			-1	on failure
 */
int
svc_connect(int fd, char *hostname, unsigned char selector,
	    char *options, size_t optlen)
{
    struct t_call call_req;
    ATMSAPAddress remote_addr;
    struct hostent *remote;
    struct t_atm_conn_prop conn_prop;

    remote=gethostbyname_atmnsap(hostname);
    if (remote == (struct hostent *) NULL) {
	fprintf(stderr,"Unable to resolve hostname %s\n",hostname);
	return -1;
    }

    remote_addr.addressType = (uint16_t)AF_ATM;
    remote_addr.sap.t_atm_sap_addr.SVE_tag_addr = (int8_t)T_ATM_PRESENT;
    remote_addr.sap.t_atm_sap_addr.SVE_tag_selector = (int8_t)T_ATM_PRESENT;
    remote_addr.sap.t_atm_sap_addr.address_format =
	(uint8_t)T_ATM_ENDSYS_ADDR;
    remote_addr.sap.t_atm_sap_addr.address_length = 20;
    memcpy(remote_addr.sap.t_atm_sap_addr.address, remote->h_addr_list[0], 19);
    remote_addr.sap.t_atm_sap_addr.address[19]=selector;
    remote_addr.sap.t_atm_sap_layer2.SVE_tag = (int8_t)T_ATM_ABSENT;
    remote_addr.sap.t_atm_sap_layer3.SVE_tag = (int8_t)T_ATM_ABSENT;
    remote_addr.sap.t_atm_sap_appl.SVE_tag = (int8_t)T_ATM_PRESENT;
    remote_addr.sap.t_atm_sap_appl.ID_type = T_ATM_USER_APP_ID;
    memcpy(remote_addr.sap.t_atm_sap_appl.ID.user_defined_ID, BHLI_MAGIC, 8);

    printf("Remote address is %s\n",sap_to_string(remote_addr));

    memset(&call_req, 0, sizeof(call_req));
    call_req.addr.len = sizeof(remote_addr);
    call_req.addr.buf = (char *)&remote_addr;

    if (options != (char *)NULL && optlen > 0) {
	call_req.opt.len = optlen;
	call_req.opt.buf = options;
    }

    printf("Connecting to %s...\n\n",hostname);

    if (t_connect(fd, &call_req, (struct t_call *) NULL) < 0) {
	int fail_cause;
	fail_cause = get_cause(fd);
	if ( fail_cause > 0 )
	    fprintf(stderr,"t_connect: failed with UNI message : %s (%d)\n",
		    causemsg(fail_cause), fail_cause);
	return -1;
    } else {
	/* For information purposes, get vpi, vci assigned for connection */
	if (get_option_value(fd, T_ATM_CONN_PROP, (char *)&conn_prop,
			    sizeof(t_atm_conn_prop)) == 0) {
	    printf("Connected using vpi %d, vci %d\n",
		    conn_prop.vpi, conn_prop.vci);
	}
    }

    return 0;
}

int
pmp_disconnect(int fd, int leaf_id)
{
    if (t_removeleaf(fd, leaf_id, T_ATM_CAUSE_UNSPECIFIED_NORMAL) < 0) {
	handle_error(fd, "t_removeleaf");
	return -1;
    }

    return 0;
}

int
pmp_connect(int fd, char *hostname, unsigned char selector, int leaf_id)
{
    struct netbuf call_req;
    ATMSAPAddress remote_addr;
    struct hostent *remote;

    remote=gethostbyname_atmnsap(hostname);
    if (remote == (struct hostent *) NULL) {
	fprintf(stderr,"Unable to resolve hostname %s\n",hostname);
	return -1;
    }

    remote_addr.addressType = (uint16_t)AF_ATM;
    remote_addr.sap.t_atm_sap_addr.SVE_tag_addr = (int8_t)T_ATM_PRESENT;
    remote_addr.sap.t_atm_sap_addr.SVE_tag_selector = (int8_t)T_ATM_PRESENT;
    remote_addr.sap.t_atm_sap_addr.address_format =
	(uint8_t)T_ATM_ENDSYS_ADDR;
    remote_addr.sap.t_atm_sap_addr.address_length = 20;
    memcpy(remote_addr.sap.t_atm_sap_addr.address, remote->h_addr_list[0], 19);
    remote_addr.sap.t_atm_sap_addr.address[19]=selector;
    remote_addr.sap.t_atm_sap_layer2.SVE_tag = (int8_t)T_ATM_ABSENT;
    remote_addr.sap.t_atm_sap_layer3.SVE_tag = (int8_t)T_ATM_ABSENT;
    remote_addr.sap.t_atm_sap_appl.SVE_tag = (int8_t)T_ATM_PRESENT;
    remote_addr.sap.t_atm_sap_appl.ID_type = (uint8_t)T_ATM_USER_APP_ID;
    memcpy(remote_addr.sap.t_atm_sap_appl.ID.user_defined_ID, BHLI_MAGIC, 8);

    printf("Remote address is %s\n",sap_to_string(remote_addr));

    memset(&call_req, 0, sizeof(call_req));
    call_req.len = sizeof(remote_addr);
    call_req.buf = (char *)&remote_addr;

    printf("Connecting PMP to %s...\n\n",hostname);

    if (t_addleaf(fd, leaf_id, &call_req) < 0) {
    /* check if asynchronous event has occurred */
	if (t_errno == TLOOK) {
	    int event;
	    event = t_look(fd);
	    if (event != TNODATA && event != T_DATA) {
		handle_error(fd, "t_addleaf");
		return -1;
	    }
	} else {
	/* some other problem has occurred */
	    handle_error(fd, "t_addleaf");
	    return -1;
	}
    }

    return 0;
}

/*
 * svc_accept(fd, batchsize)
 *
 * Listens for and accepts an incoming call.
 *
 * In:
 *   int fd		fd for transport endpoint
 *   int batchsize	batchsize to use when receiving AAL NULL cells
 *
 * Returns:
 *	int		0	on success
 *			-1	on failure
 */
int
svc_accept(int fd, int batchsize)
{
    struct t_call incoming;
    ATMSAPAddress remote_addr;
    struct t_info info;
    struct t_opthdr *popt;
    struct t_atm_conn_prop conn_prop;

    printf("Listening for call...\n");

    memset(&incoming, 0, sizeof(incoming));
    incoming.addr.maxlen = sizeof(remote_addr);
    incoming.addr.buf = (char *) &remote_addr;

    /* According to XNS Issue 5, we can set maxlen = 0 to ignore any
     * return options.	However, most OSes only implement Issue 4
     * currently, which doesn't have this feature.  Therefore, we
     * always allocate space for incoming options data.
     */

    if (t_getinfo(fd, &info) < 0) {
	t_error("t_getinfo");
	return -1;
    }

    incoming.opt.buf = (char *) malloc(info.options);
    if (incoming.opt.buf == (char *) NULL) {
	perror("malloc (options buffer)");
	return -1;
    }

    memset(incoming.opt.buf, 0, sizeof(info.options));

    incoming.opt.maxlen = info.options;

    if (t_listen(fd, &incoming) < 0) {
	handle_error(fd, "t_listen");
	free(incoming.opt.buf);
	return -1;
    }

    /*
     * the address will only be present if the initiator has supplied the
     * calling party address (T_ATM_ORIG_ADDR option).
     */

    if (incoming.addr.len > 0) {
	printf("Incoming call from %s\n",sap_to_string(remote_addr));
    }

    printf("Accepting incoming call...\n\n");

    /* If batchsize is indicated on input, use it */
    if (batchsize != T_ATM_ABSENT) {
	/* batchsize is an option only applicable on the receiving side */
	t_atm_conn_prop *connprop_p;
	popt = (struct t_opthdr *) incoming.opt.buf;

	while (popt && (popt->name != T_ATM_CONN_PROP))
	    popt = T_OPT_NEXTHDR(incoming.opt.buf, incoming.opt.len, popt);

	if (popt) {
	    /* Option already existed, let's update it */
	    popt->status = 0;
	    connprop_p = (t_atm_conn_prop *)((char *)popt
			+ sizeof(struct t_opthdr));
	    connprop_p->recv_batchsize = batchsize;
	} else {
	    /* Add batchsize to end of options */
	    popt = (struct t_opthdr *)
			((char *)incoming.opt.buf + incoming.opt.len);
	    popt->level	   = T_ATM_SIGNALING;
	    popt->name	   = T_ATM_CONN_PROP;
	    popt->len	   = sizeof(struct t_opthdr) + sizeof(t_atm_conn_prop);
	    popt->status   = 0;
	    connprop_p	    = (t_atm_conn_prop *)((char *)popt
				+ sizeof(struct t_opthdr));
	    connprop_p->recv_batchsize = batchsize;
	    connprop_p->vc_direction   = XTI_VC_DIR_DUPLEX;

	    /* Update the len for this option */
	    popt = T_OPT_NEXTHDR(incoming.opt.buf, incoming.opt.maxlen, popt);
	    incoming.opt.len = (char *) popt - (char *)incoming.opt.buf;
	}
    }

    /* If aal_type is AAL_TYPE_USER set backward and forward max_SDU_size */
    popt = (struct t_opthdr *) incoming.opt.buf;

    while (popt && (popt->name != T_ATM_AAL))
	popt = T_OPT_NEXTHDR(incoming.opt.buf, incoming.opt.len, popt);

    if (popt) {
	t_atm_aal *aal;
	aal = (struct t_atm_aal *)((char *)popt + sizeof(struct t_opthdr));
	if (aal->aal_type == T_ATM_AAL_TYPE_USER) {
	    /* set max_SDU_size to a multiple of 48 */
	    aal->uniaalu.backward_max_SDU_size = 65520;
	    aal->uniaalu.forward_max_SDU_size  = 65520;
	    aal->uniaalu.vc_aal_type = T_ATM_AAL_TYPE_NULL;
	}
    }

    if (t_accept(fd, fd, &incoming) < 0) {
	handle_error(fd, "t_accept");
	free(incoming.opt.buf);
	return -1;
    } else {
	/* For information purposes, get vpi, vci assigned for connection */
	if (get_option_value(fd, T_ATM_CONN_PROP, (char *)&conn_prop,
			    sizeof(t_atm_conn_prop)) == 0) {
	    printf("Accepting connection using vpi %d, vci %d\n",
		    conn_prop.vpi, conn_prop.vci);
	}
    }

    free(incoming.opt.buf);

    return 0;
}

/*
 * svc_disconnect(fd)
 *
 * Disconnects the SVC endpoint.
 *
 * In:
 *	int fd		fd for transport endpoint
 *
 * Returns:
 *	int		0	on success
 *			-1	on failure
 */
int
svc_disconnect(int fd)
{
    printf("Disconnecting...\n\n");

    if (t_snddis(fd, (struct t_call *) NULL)) {
	t_error("t_snddis");
	return -1;
    }

    return 0;
}


/*
 * svc_recv_disconnect(fd)
 *
 * Waits for and receives a disconnect message from the remote SVC endpoint.
 * Provides some handling for PMP leaf disconnects as well as simple SVCs.
 * While no event is present, will sleep for 1 second.	To avoid looping
 * indefinitely, we will limit waiting.
 *
 *
 *
 * In:
 *	int fd		fd for transport endpoint
 */
void
svc_recv_disconnect(int fd)
{
    int  event, waiting = 1;
    struct t_leaf_status change;

    printf("Waiting for Disconnect...\n");

    while (waiting) {
	event = t_look(fd);
	switch (event) {
	  case -1:
	      printf("Got t_look error %d\n", t_errno);
	      waiting = 0;
	      break;
	  case 0:
	      waiting++;
	      break;
	  case T_LEAFCHANGE:
	      if (t_rcvleafchange(fd, &change) == 0) {
		  printf("Got leaf %s: leaf ID %d\n",
			 ((change.status == T_LEAF_CONNECTED) ?
			  "connect" : "disconnect"),
			 change.leafid);
	      }
	      waiting++;
	      break;
	  case T_DISCONNECT:
#ifndef aix 
	  case T_ERROR:
#endif 
	      waiting = 0;
	      break;
	  default:
	      printf("Got t_look event %d\n", event);
	      break;
	}
	sleep(1);
	if (waiting == SHORT_WAIT*2) {
	    printf("Never received disconnect message.\n");
	    waiting = 0;
	}

    }

    return;
}

/*
 * pvc_bind(fd)
 *
 * Binds the local endpoint of a PVC.
 *
 * This serves only to move from T_IDLE to T_BOUND state.  The actual address
 * (i.e. VPI/VCI) is specified at t_connect() time and should not be given
 * to t_bind().
 *
 * In:
 *	int fd		file descriptor for device to bind
 *
 * Returns:
 *	int		0	on success
 *			-1	on failure
 */

int
pvc_bind(int fd)
{
    struct t_atm_card_prop card_prop;

    printf("Binding PVC endpoint...\n\n");

    if (t_bind(fd, (struct t_bind *) NULL, (struct t_bind *) NULL) < 0) {
	t_error("t_bind");
	return -1;
    }

    if (get_option_value(fd, T_ATM_CARD_PROP,
			(char *)&card_prop,
			sizeof(t_atm_card_prop)) != 0) {
	fprintf(stderr, "Unable to obtain ATM Card properties.\n");
    } else {
	hw_type = card_prop.hw_type;
    }

    return 0;
}

/*
 * pvc_connect(fd, vpi, vci, options, optlen)
 *
 * Connects a PVC endpoint.
 *
 * This associates the bound endpoint with the given (VPI/VCI).
 * No indication is sent to the remote end.
 *
 * In:
 *	int fd		file descriptor for device to connect
 *	int vpi		virtual path (must be 0 for FORE 200e)
 *	int vci		virtual channel
 *	int options	buffer containing options to pass to t_connect
 *	int optlen	length of options buffer
 *		  (if options is NULL or optlen is 0, no options are used)
 *
 * Returns:
 *	int		0	on success
 *			-1	on failure
 */
int
pvc_connect(int fd, int vpi, int vci, char *options, size_t optlen)
{
    struct t_call call_req;
    PVCAddress addr_req;

    addr_req.addressType=AF_ATM_PVC;
    addr_req.vpi = vpi;
    addr_req.vci = vci;

    memset(&call_req, 0, sizeof(call_req));
    call_req.addr.len = sizeof(addr_req);
    call_req.addr.buf = (char *) &addr_req;

    if (options != (char *)NULL && optlen > 0) {
	call_req.opt.len = optlen;
	call_req.opt.buf = options;
    }

    printf("Connecting to PVC (%d.%d)...\n\n", vpi, vci);

    if (t_connect(fd, &call_req, (struct t_call *) NULL) < 0) {
	t_error("t_connect");
	printf(" t_errno = %d", t_errno); 
	printf(" errno = %d",errno);
	return -1;
    }

    return 0;
}

/*
 * pvc_disconnect(fd)
 *
 * Disconnects a PVC endpoint.
 *
 * Like t_bind, this merely causes a state change, from T_DATAXFER to T_BOUND.
 * No indication of the disconnect is sent to the remote endpoint.
 *
 * In:
 *	int fd		file descriptor for device to connect
 *
 * Returns:
 *	0	on success
 *	-1	on failure
 */
int
pvc_disconnect(int fd)
{
    printf("Disconnecting...\n\n");

    if (t_snddis(fd, (struct t_call *) NULL)) {
	t_error("t_snddis");
	return -1;
    }
    return 0;
}

/*
 * send_data(fd, len, num)
 *
 * Sends data across a transport connection.
 *
 * In:
 *	int fd		fd to send on
 *	int len		length of PDUs to send
 *	int num		number of PDUs to send
 */
void
send_data(int fd, int len, int num)
{
    char *buf;
    int cnt, num_pdus;
    struct timeval start, end;
    double elapsed;
    uintmax_t total_bytes;

    buf = (char *)malloc(len);
    if (buf == (char *) NULL) {
	perror("malloc");
	return;
    }
    memset(buf, 0, len);

    printf("Sending %d PDUs of length %d...\n", num, len);

    total_bytes = 0;
    GET_TIME(start);

    for(num_pdus = 0; num_pdus < num; ++num_pdus) {
	cnt = t_snd(fd, buf, len, 0);

	if(cnt >= 0) {
	    total_bytes += cnt;
	} else {
	    handle_error(fd, "t_snd");
	    break;
	}
    }
    GET_TIME(end);
    elapsed = SUB_TIME(start, end);

    free(buf);
    /*    printf("Sent %d PDUs (%" PRIuMAX " bytes)\n", num_pdus, total_bytes); */
    printf("Elapsed time = %.3f seconds, throughput = %.3f Mb/sec\n\n",
	   elapsed, (total_bytes * 8 / 1000000) / elapsed);
}

/*
 * recv_data(fd, len, num)
 *
 * Receives data on a transport connection.
 *
 * In:
 *	int fd		fd to receive on
 *	int len		length of PDUs to receive
 *	int num		number of PDUs to send
 */
void
recv_data(int fd, int len, int num)
{
    char *buf;
    int cnt, num_pdus;
    struct timeval start, end;
    double elapsed;
    struct pollfd *pollfd;
    int timeout = SHORT_WAIT*1000;    /* units must be in milliseconds */
    int poll_result = 0;
    uintmax_t total_bytes;
    int flags;

    buf = (char *) malloc(len);
    if (buf == (char *) NULL) {
	perror("malloc");
	return;
    }

    /* allocate space for pollfd array */
    pollfd = malloc(sizeof(*pollfd));
    if (pollfd == NULL) {
	/* unable to obtain space needed for pollfd array */
	fprintf(stderr, "recv_data: "
		"unable to setup space for poll function\n");
	return;
    }

    pollfd->fd = fd;
    pollfd->events = POLLIN;

    printf("Receiving %d PDUs of length %d...\n", num, len);

    num_pdus=total_bytes=0;

    poll_result = poll(pollfd, 1, LONG_WAIT*1000);
    if (poll_result > 0) {
	GET_TIME(start);
	while (1) {
	    if (poll_result > 0) {
		/* an event has occurred on the fd */
		cnt = t_rcv(fd, buf, len, &flags);
		if (cnt >= 0) {
		    /* Valid data has been received, add to the tally */
		    total_bytes += cnt;
		    num_pdus++;
		    if (num_pdus >= num) {
			/* we have received all data, let's leave */
			break;
		    }
		} else {
		    handle_error(fd, "t_rcv");
		    break;
		}
	    } else {
		/* whether poll itself failed or timeout has occurred */
		/* let's end the loop				      */
		break;
	    }
	    poll_result = poll(pollfd, 1, timeout);
	}
    }
    GET_TIME(end);
    elapsed=SUB_TIME(start,end);
    free(buf);
    free(pollfd);

    if (poll_result < 0) {
	fprintf(stderr,"recv_data: unexpected error from poll function\n");
	return;
    }

    if (poll_result == 0) {
	printf("Note:  Timeout occurred, adjusting elapsed time.\n");
	elapsed=ADJ_TIME(elapsed,timeout);
    }

    /*    printf("Received %d PDUs (%" PRIuMAX " bytes)\n", num_pdus, total_bytes); */
    printf("Elapsed time = %.3f seconds, throughput = %.3f Mb/sec\n\n",
	elapsed, (total_bytes * 8 / 1000000) / elapsed);
}

int
get_cause(int fd)
{
    struct t_opthdr *popt, *popt_ret;
    char *buf_req, *buf_ret;
    struct t_info info;
    struct t_optmgmt opt_req, opt_ret;
    struct t_atm_cause *cause_ret;
    int cause_number;

    if (t_getinfo(fd, &info) < 0) {
	t_error("t_getinfo");
	return -1;
    }

    buf_req = (char *) malloc(info.options);
    if (buf_req == (char *) NULL) {
	fprintf(stderr,"Unable to allocate %ld bytes for options\n",
		info.options);
	return -1;
    }

    buf_ret = (char *) malloc(info.options);
    if (buf_ret == (char *) NULL) {
	fprintf(stderr,"Unable to allocate %ld bytes for options\n",
		info.options);
	return -1;
    }

    memset(&opt_req, 0, sizeof(opt_req));
    memset(&opt_ret, 0, sizeof(opt_ret));

    popt     = (struct t_opthdr *) buf_req;
    popt_ret = (struct t_opthdr *) buf_ret;

    popt->len	    = sizeof(struct t_opthdr) + sizeof(t_atm_cause);
    popt->level	    = T_ATM_SIGNALING;
    popt->name	    = T_ATM_CAUSE;
    popt->status    = 0;

    opt_req.opt.len = popt->len;
    opt_req.opt.buf = (char *)popt;
    opt_req.flags = T_CURRENT;

    popt = T_OPT_NEXTHDR(buf_req, info.options , popt);
    opt_ret.opt.maxlen	= info.options;
    opt_ret.opt.buf = (char *)popt_ret;

    if (t_optmgmt(fd, &opt_req, &opt_ret) < 0) {
	t_error("t_optmgmt");

    }
    cause_ret = (t_atm_cause *)((char *)popt_ret+sizeof(struct	t_opthdr));

    cause_number = cause_ret->cause_value;
    free(buf_ret);
    free(buf_req);
    return cause_number;
}

int
get_option_value(int fd, int name, char *opt_return, int opt_size)
{
    struct t_opthdr *popt, *popt_ret;
    char *buf_req, *buf_ret;
    struct t_info info;
    struct t_optmgmt opt_req, opt_ret;

    if (t_getinfo(fd, &info) < 0) {
	t_error("t_getinfo");
	return -1;
    }

    buf_req = (char *) malloc(info.options);
    if (buf_req == (char *) NULL) {
	fprintf(stderr,"Unable to allocate %ld bytes for options\n",
		info.options);
	return -1;
    }

    buf_ret = (char *) malloc(info.options);
    if (buf_ret == (char *) NULL) {
	fprintf(stderr,"Unable to allocate %ld bytes for options\n",
		info.options);
	return -1;
    }

    memset(&opt_req, 0, sizeof(opt_req));
    memset(&opt_ret, 0, sizeof(opt_ret));

    popt     = (struct t_opthdr *) buf_req;
    popt_ret = (struct t_opthdr *) buf_ret;

    popt->len= sizeof(struct t_opthdr) + opt_size;

    if (strstr(device_name,"xtisvc"))
	popt->level	= T_ATM_SIGNALING;
    else if (strstr(device_name,"xtipvc"))
	popt->level	= T_ATM_PVC;
    else {
	fprintf(stderr,"Unable to value for option 0x%x. "
		"Reason: unknown device name %s.\n", name, device_name);
	free(buf_ret);
	free(buf_req);
	return -1;
    }

    popt->name	    = name;
    popt->status    = 0;

    opt_req.opt.len = popt->len;
    opt_req.opt.buf = (char *)popt;
    opt_req.flags = T_CURRENT;

    popt = T_OPT_NEXTHDR(buf_req, info.options , popt);
    opt_ret.opt.maxlen	= info.options;
    opt_ret.opt.buf = (char *)popt_ret;

    if (t_optmgmt(fd, &opt_req, &opt_ret) < 0) {
	t_error("t_optmgmt");
    }

    memcpy(opt_return, (char *)popt_ret+sizeof(struct  t_opthdr), opt_size);

    free(buf_ret);
    free(buf_req);
    return 0;
}

/*
 * construct_options(fd, user_optionsP)
 *
 * Constructs a buffer with the requested options which can be passed
 * to t_optmgmt() or t_connect()
 *
 * In:
 *	int fd		fd of bound trasport endpoint.	Used to obtain
 *			information and possibly the local address.
 *
 * Pointer to User options which can indicate:
 *
 *	pcr	CBR quality of service in Kbit/s (1000bit/s).
 *			if 0 or >=line rate, then CBR is not signalled for and
 *			UBR at line rate is used.
 *
 *	flags	if OPT_FLAGS_CPID is set, the T_ATM_ORIG_ADDR option
 *			is created, corresponding to the Calling Party IE.
 *			if OPT_FLAGS_PMP is set, bearer and traffic IEs are
 *			created and set as appropriate for a PMP connection.
 *
 *   batchsize On receive, enables batching of cells before interrupt
 *	       application.
 *
 * Out:
 *	optlen	on return, holds the actual size of the options.
 *
 * Returns:
 *			pointer to buffer containing the options.  The
 *			caller must release the buffer using free()
 *
 *			returns NULL pointer on failure.
 */
char *
construct_options(int fd, usercfg_opts_t  *user_optionsP)
{
    struct t_opthdr *popt;
    char *buf;
    int qos_cells;
    struct t_info info;

    if (t_getinfo(fd, &info) < 0) {
	t_error("t_getinfo");
	return (char *) NULL;
    }

    buf = (char *) malloc(info.options);
    if (buf == (char *) NULL) {
	fprintf(stderr,"Unable to allocate %ld bytes for options\n",
		info.options);
	return (char *) NULL;
    }

    memset(buf, 0, info.options);

    popt = (struct t_opthdr *) buf;

    if (user_optionsP->batchsize > 0) {
	/* batchsize is an option only applicable on receiving cell side */
	/* Default batchsize is 1					 */
	t_atm_conn_prop *connprop;
	popt->len	= sizeof(struct t_opthdr) + sizeof(t_atm_conn_prop);
	popt->level	= T_ATM_SIGNALING;
	popt->name	= T_ATM_CONN_PROP;
	popt->status	= 0;

	connprop = (t_atm_conn_prop *)((char *)popt + sizeof(struct t_opthdr));
	connprop->recv_batchsize = user_optionsP->batchsize;
	connprop->vc_direction	 = XTI_VC_DIR_DUPLEX;

	popt = T_OPT_NEXTHDR(buf, info.options , popt);

    }

    if (user_optionsP->flags & OPT_FLAGS_CPID) {
	/*
	 * this constructs the T_ATM_ORIG_ADDR option, which is used to
	 * signal the UNI 3.1 Calling Party ID Information Element.
	 */

	t_atm_addr *source_addr;

	popt->len	= sizeof(struct t_opthdr) + sizeof(t_atm_addr);
	popt->level	= T_ATM_SIGNALING;
	popt->name	= T_ATM_ORIG_ADDR;
	popt->status	= 0;

	source_addr = (t_atm_addr *)((char *)popt + sizeof(struct t_opthdr));

	source_addr->address_format = T_ATM_ENDSYS_ADDR;
	source_addr->address_length = 20;
	if(get_local_address(fd, source_addr->address)) {
	    fprintf(stderr,"Can't get local address!\n");
	    free(buf);
	    return (char *) NULL;
	}

	popt = T_OPT_NEXTHDR(buf, info.options , popt);
    }

    /*
     * this constructs all options necessary (bearer cap., QoS, and
     * Traffic Descriptor) to signal for a connection with the
     * specified QoS, and/or specify a PMP connection.
     *
     * for FORE 200e cards, the adapter shapes traffic to CBR with
     * rate  equal to PCR CLP=0+1 (traffic.forward.PCR_all_traffic)
     */

    qos_cells = user_optionsP->vc_qos.pcr;

    if ((qos_cells > 0) ||
	(user_optionsP->flags & OPT_FLAGS_PMP)) {
	struct t_atm_bearer *bearer;
	struct t_atm_traffic *traffic;

	/* T_ATM_BEARER_CAP: Broadband bearer capability */
	popt->len	= sizeof(struct t_opthdr) +
			  sizeof(struct t_atm_bearer);
	popt->level	= T_ATM_SIGNALING;
	popt->name	= T_ATM_BEARER_CAP;
	popt->status	= 0;

	bearer = (struct t_atm_bearer *)((char *)popt +
					 sizeof(struct t_opthdr));
	bearer->bearer_class = T_ATM_CLASS_X;
	bearer->traffic_type = user_optionsP->vc_qos.qosclass;

	switch ( user_optionsP->vc_qos.qosclass ) {
	  case T_ATM_CBR:
	      bearer->timing_requirements = T_ATM_END_TO_END;
	      break;
	  case T_ATM_VBR:
	      bearer->timing_requirements = T_ATM_NO_END_TO_END;
	      break;
	  case T_ATM_ABR:
	      fprintf(stderr, "ABR connection for SVCs is not supported\n");
	      break;
	  case T_ATM_NULL:
	      bearer->timing_requirements = 0;
	      break;
	  default:
	      fprintf(stderr, "Unrecognized qos class 0x%x \n",
		      user_optionsP->vc_qos.qosclass);
	      break;
	}

	bearer->clipping_susceptibility	 = T_ATM_NULL;

	bearer->connection_configuration	   =
	    (user_optionsP->flags & OPT_FLAGS_PMP) ? T_ATM_1_TO_MANY
						   : T_ATM_1_TO_1;

	popt = T_OPT_NEXTHDR(buf, info.options, popt);

	/* T_ATM_TRAFFIC: traffic descriptor */
	popt->len	= sizeof(struct t_opthdr) +
			  sizeof(struct t_atm_traffic);
	popt->level	= T_ATM_SIGNALING;
	popt->name	= T_ATM_TRAFFIC;
	popt->status	= 0;

	traffic=(struct t_atm_traffic *)((char *)popt+sizeof(struct t_opthdr));

	/* PCR */
	traffic->forward.PCR_all_traffic	= (qos_cells) ? qos_cells
							      : OC3_LINE_RATE;
	traffic->backward.PCR_all_traffic	=
	    (user_optionsP->flags & OPT_FLAGS_PMP) ? 0
						   : qos_cells;

	if (user_optionsP->vc_qos.qosclass == T_ATM_VBR) {
	    /* SCR */
	    traffic->forward.SCR_all_traffic	= user_optionsP->vc_qos.scr;
	    traffic->backward.SCR_all_traffic	=
		(user_optionsP->flags & OPT_FLAGS_PMP) ? 0
						   : user_optionsP->vc_qos.scr;

	    /* MBS */
	    traffic->forward.MBS_all_traffic	= user_optionsP->vc_qos.mbs;
	    traffic->backward.MBS_all_traffic	=
		(user_optionsP->flags & OPT_FLAGS_PMP) ? 0
						   : user_optionsP->vc_qos.mbs;
	} else {
	    traffic->forward.SCR_all_traffic	= T_ATM_ABSENT;
	    traffic->backward.SCR_all_traffic	= T_ATM_ABSENT;
	    traffic->forward.MBS_all_traffic	= T_ATM_ABSENT;
	    traffic->backward.MBS_all_traffic	= T_ATM_ABSENT;
	}

	traffic->forward.tagging		= T_NO;
	traffic->backward.PCR_all_traffic	=
	    (user_optionsP->flags & OPT_FLAGS_PMP) ? 0
						   : qos_cells;

	traffic->forward.PCR_high_priority	= T_ATM_ABSENT;
	traffic->forward.SCR_high_priority	= T_ATM_ABSENT;
	traffic->forward.MBS_high_priority	= T_ATM_ABSENT;
	traffic->backward.PCR_high_priority	= T_ATM_ABSENT;
	traffic->backward.SCR_high_priority	= T_ATM_ABSENT;
	traffic->backward.MBS_high_priority	= T_ATM_ABSENT;
	traffic->backward.tagging		= T_NO;

	traffic->best_effort			= (qos_cells) ? T_NO : T_YES;

	popt = T_OPT_NEXTHDR(buf, info.options, popt);
    }

    if (qos_cells > 0) {
	struct t_atm_qos *qos;

	/* T_ATM_QOS: Quality of Service */
	popt->len	= sizeof(struct t_opthdr) +
			  sizeof(struct t_atm_qos);
	popt->level	= T_ATM_SIGNALING;
	popt->name	= T_ATM_QOS;
	popt->status	= 0;

	qos = (struct t_atm_qos *)((char *)popt + sizeof(struct t_opthdr));
	qos->coding_standard	= T_ATM_ITU_CODING;
	qos->forward.qos_class	= T_ATM_QOS_CLASS_1;
	qos->backward.qos_class	= T_ATM_QOS_CLASS_1;

	popt = T_OPT_NEXTHDR(buf, info.options, popt);
    }

    if (user_optionsP->aal != T_ATM_ABSENT) {
	struct t_atm_aal *aal;

	/* T_ATM_AAL:  ATM Adaptation Layer */
	popt->len	= sizeof(struct t_opthdr) + sizeof(struct t_atm_aal);
	popt->level	= T_ATM_SIGNALING;
	popt->name	= T_ATM_AAL;
	popt->status	= 0;

	aal = (struct t_atm_aal *)((char *)popt + sizeof(struct t_opthdr));
	aal->aal_type = user_optionsP->aal;

	switch (user_optionsP->aal) {
	  case T_ATM_AAL_TYPE_NULL: {
	      u_int gfc = 0x0;	/* gfc bits (32-29) */
	      u_int vpi = 0x0;	/* vpi bits (28-21) */
	      u_int vci = 0x0;	/* vci bits (20-5)  */
	      u_int pti = 0x0;	/* pti bits (4-2)   */
	      u_int clp = 0x0;	/* clp bit  (1)	    */

				/* for signalling sample, code any number */
	      aal->uniaalu.user_aal_info = 99;

				/* Sample hdr for Transmit	   */
	      aal->uniaalu.AtmT_hdr= gfc | vpi | vci | pti | clp;

	      aal->aal_type = T_ATM_AAL_TYPE_USER;
				/* max_SDU_size that is a multiple of 48 */
	      aal->uniaalu.backward_max_SDU_size = 65520;
	      aal->uniaalu.forward_max_SDU_size	 = 65520;
				/* VC to be setup as AAL NULL */
	      aal->uniaalu.vc_aal_type = T_ATM_AAL_TYPE_NULL;
	      break;
	  }
	  case T_ATM_AAL_TYPE_5:
	      aal->uniaal5.backward_max_SDU_size = 65535;
	      aal->uniaal5.forward_max_SDU_size	 = 65535;
	      break;
	  default:
	      fprintf(stderr," Unsupported aal type %d\n", user_optionsP->aal);
	      break;
	}
	popt = T_OPT_NEXTHDR(buf, info.options, popt);
    }

    /*
     * return actual size of options and option buffer to user
     */
    user_optionsP->optlen = (char *)popt - buf;
    return buf;
}


/*
 * construct_pvc_options(fd, user_optionsP);
 *
 * Provides the same function as construct_options(), above, for
 * PVC endpoints (which use different options structures than SVCs).
 */
char *
construct_pvc_options(int fd, usercfg_opts_t *user_optionsP)
{
    struct t_opthdr *popt;
    char *buf;
    int qos_cells;
    struct t_info info;

    if (t_getinfo(fd, &info) < 0) {
	t_error("t_getinfo");
	return (char *) NULL;
    }

    buf = (char *) malloc(info.options);
    if(buf == (char *) NULL) {
	fprintf(stderr,"Unable to allocate %ld bytes for options\n",
		info.options);
	return (char *) NULL;
    }

    memset(buf, 0, info.options);

    popt = (struct t_opthdr *) buf;

    if ((user_optionsP->batchsize != T_ATM_ABSENT) || 
	(user_optionsP->vc_direction != T_ATM_ABSENT)) {
	/*
	 * batchsize is an option only applicable on receiving cell side
	 * and only applies to AAL NULL cells.	Default batchsize is 1.
	 *
	 * a virtual channel can be opened as duplex or in a single 
	 * direction.  Default is duplex. 
	 */
	t_atm_conn_prop *connprop;
	popt->len = sizeof(struct t_opthdr) + sizeof(t_atm_conn_prop);
	popt->level    = T_ATM_PVC;
	popt->name     = T_ATM_CONN_PROP;
	popt->status   = 0;

	connprop = (t_atm_conn_prop *)((char *)popt + sizeof(struct t_opthdr));

	connprop->recv_batchsize = user_optionsP->batchsize;

	connprop->vc_direction	 = user_optionsP->vc_direction;

	popt = T_OPT_NEXTHDR(buf, info.options , popt);
    }

    qos_cells = user_optionsP->vc_qos.pcr;

    if ((qos_cells <= 0) &&
	(user_optionsP->vc_qos.qosclass == T_ATM_NULL)) {
	qos_cells = OC3_LINE_RATE;
    }

    if (qos_cells > 0) {
	/*
	 * this constructs all options necessary (QoS, and
	 * Traffic Descriptor) to signal for a connection with the
	 * specified QoS.
	 *
	 * for FORE 200e cards, the adapter shapes traffic to CBR with
	 * rate equal to PCR CLP=0+1 (traffic->forward.PCR_all_traffic)
	 */

	struct t_atm_traffic *traffic;
	struct t_atm_bearer  *bearer;

	/* T_ATM_TRAFFIC: traffic descriptor */
	popt->len	= sizeof(struct t_opthdr) +
	    sizeof(struct t_atm_traffic);
	popt->level	= T_ATM_PVC;
	popt->name	= T_ATM_TRAFFIC;
	popt->status	= 0;

	traffic=(struct t_atm_traffic *)
	    ((char *)popt+sizeof(struct t_opthdr));

	/*
	 * Forward traffic is applicable for a PVC,
	 * but, backward traffic does not apply.
	 * So, backward traffic is never coded.
	 */

	traffic->forward.PCR_high_priority	= T_ATM_ABSENT;
	traffic->forward.SCR_high_priority	= T_ATM_ABSENT;
	traffic->forward.MBS_high_priority	= T_ATM_ABSENT;

	traffic->forward.PCR_all_traffic	= qos_cells;
	traffic->forward.SCR_all_traffic	= user_optionsP->vc_qos.scr;
	traffic->forward.MBS_all_traffic	= user_optionsP->vc_qos.mbs;
	traffic->forward.tagging		= T_NO;

	if (user_optionsP->vc_qos.qosclass != T_ATM_NULL) {
	    traffic->best_effort		= T_NO;
	} else {
	    traffic->best_effort		= T_YES;
	}

	popt = T_OPT_NEXTHDR(buf, info.options, popt);

	/*
	 * Let's now fill in traffic type in t_atm_bearer structure
	 * For a PVC, bearer_class, timing_requirements, clipping and
	 * connection configuration do not apply.
	 */

	popt->len	= sizeof(struct t_opthdr) +
			  sizeof(struct t_atm_bearer);
	popt->level	= T_ATM_PVC;
	popt->name	= T_ATM_BEARER_CAP;
	popt->status	= 0;

	bearer = (struct t_atm_bearer *)((char *)popt +
					 sizeof(struct t_opthdr));

	bearer->traffic_type = user_optionsP->vc_qos.qosclass; 
	
	
	popt = T_OPT_NEXTHDR(buf, info.options, popt);
    }

    if ((user_optionsP->vc_qos.icr != T_ATM_ABSENT) ||
	(user_optionsP->vc_qos.mcr != T_ATM_ABSENT)) {
	struct t_atm_abr_setup *abrsetup;

	/* T_ATM_ABR_SETUP:  abrsetup parameters */
	popt->len	= sizeof(struct t_opthdr) +
			  sizeof(struct t_atm_abr_setup);
	popt->level	= T_ATM_PVC;
	popt->name	= T_ATM_ABR_SETUP;
	popt->status	= 0;

	abrsetup = (struct t_atm_abr_setup *)((char *)popt +
					      sizeof(struct t_opthdr));

	/*
	 * Forward abr_setup is applicable for a PVC,
	 * but, backward abr_setup does not apply.
	 * So, there is no need to code backward abr_setup
	 */

	abrsetup->forward.MCR_high_priority    = T_ATM_ABSENT;
	abrsetup->forward.MCR_all_traffic      = user_optionsP->vc_qos.mcr;
	abrsetup->forward.icr_all_traffic      = user_optionsP->vc_qos.icr;
	abrsetup->forward.trans_buff_exp       = T_ATM_ABSENT;
	abrsetup->forward.rate_incr_factor     = T_ATM_ABSENT;
	abrsetup->forward.rate_decr_factor     = T_ATM_ABSENT;

	popt = T_OPT_NEXTHDR(buf, info.options, popt);
    }


    if (user_optionsP->aal != T_ATM_ABSENT) {
	struct t_atm_pvc_aal *pvc_aal_info;

	/* T_ATM_PVC_AAL: aal descriptor */
	popt->len	= sizeof(struct t_opthdr) +
			  sizeof(struct t_atm_pvc_aal);
	popt->level	= T_ATM_PVC;
	popt->name	= T_ATM_PVC_AAL;
	popt->status	= 0;

	pvc_aal_info = (struct t_atm_pvc_aal *)((char *)popt +
						sizeof(struct t_opthdr));
	pvc_aal_info->aal_type = user_optionsP->aal;
	switch (user_optionsP->aal) {
	  case T_ATM_AAL_TYPE_NULL : {
	      u_int gfc = 0x0;			    /* gfc bits (32-29) */
	      u_int vpi = user_optionsP->vpi<<20;   /* vpi bits (28-21) */
	      u_int vci = user_optionsP->vci<<4;    /* vci bits (20-5)	*/
	      u_int pti = 0x0;			    /* pti bits (4-2)	*/
	      u_int clp = 0x0;			    /* clp bit	(1)	*/

	      /* AtmT_hdr is used for transmitting cells */
	      pvc_aal_info->pvcaal0.AtmT_hdr = gfc | vpi | vci | pti | clp;
	      break;
	  }
	  case T_ATM_AAL_TYPE_5 :
	      pvc_aal_info->pvcaal5.max_sdu_size[0] = 65535;
	      pvc_aal_info->pvcaal5.max_sdu_size[1] = 65535;
	      break;
	  default :
	      fprintf(stderr, "Unknown aal %d\n", user_optionsP->aal);
	}

	popt = T_OPT_NEXTHDR(buf, info.options, popt);
    }

    /*
     * return actual size of options and option buffer to user
     */
    user_optionsP->optlen  = (char *)popt - buf;
    return buf;
}

/*
 * get_local_address(fd, addr)
 *
 * Obtains the local ATM address
 *
 * In:
 *	int fd		bound fd of transport endpoint to obtain address for
 *
 * Out:
 *	u_char *addr	local address
 *
 * Returns:
 *	int		0	on success
 *			-1	on failure
 */
/* Some OSes (namely, AIX) have implemented the v5 t_getprotaddr() command.
 * Others, like Irix, still use the v4 t_getname().  Both are offered here.
 */
#ifdef aix
#define T_GETPROTADDR(fd, laddr, raddr) t_getprotaddr(fd, laddr, raddr)
#else
#define T_GETPROTADDR(fd, laddr, raddr) t_getname(fd, laddr.addr, LOCALNAME)
#endif /* aix */

static int
get_local_address(int fd, unsigned char addr[])
{
    ATMSAPAddress local_addr;
    struct t_bind boundaddr;

    boundaddr.addr.maxlen = sizeof(local_addr);
    boundaddr.addr.buf = (char *) &local_addr;

    if (T_GETPROTADDR(fd, &boundaddr, NULL) < 0) {
	t_error("T_GETPROTADDR (local_address)");
	return -1;
    }

    memcpy(addr, local_addr.sap.t_atm_sap_addr.address, 20);

    return 0;
}

/*
 * sap_to_string(addr)
 *
 * Converts an ATMSAPAddress to a string.
 *
 * In:
 *	addr		address to convert
 *
 * Out:
 *	pointer to a string containing the address or a diagnostic message.
 *
 *	this storage is not thread-safe and should not be freed by the caller.
 */
static char *sap_to_string(ATMSAPAddress addr)
{
    static char buf[256];

    if (addr.addressType != (uint16_t)AF_ATM) {
	sprintf(buf, "[address type (%#x) != AF_ATM]", addr.addressType);
    } else if (addr.sap.t_atm_sap_addr.SVE_tag_addr != (int8_t)T_ATM_PRESENT) {
	sprintf(buf, "[address is not present]");
    } else if (addr.sap.t_atm_sap_addr.address_format !=
	      (uint8_t)T_ATM_ENDSYS_ADDR) {
	sprintf(buf, "[address format (%#x) != T_ATM_ENDSYS_ADDR",
		addr.sap.t_atm_sap_addr.address_format);
    } else if (addr.sap.t_atm_sap_addr.address_length != 20) {
	sprintf(buf, "[address length (%d) != 20]",
		addr.sap.t_atm_sap_addr.address_length);
    } else {
	struct atmnsap_addr nsap;
	memcpy(nsap.atmnsap, addr.sap.t_atm_sap_addr.address, 20);
	sprintf(buf, "%s",atmnsap_ntoa(nsap));
    }

    return buf;
}


/*
 * handle_error(fd, func)
 *
 * In:
 *	int fd		fd where error occurred
 *	char *func	name of function that caused the error
 *
 * Returns:
 *	int		0	if the error was that a disconnect was received
 *			-1	if any other error occurred
 *
 * Notes:
 *	also handles disconnect indications by calling t_rcvdis().
 */
static int
handle_error(int fd, char *func)
{
    printf("%s returns negative; ", func);

    if (t_errno == TLOOK) {
	int code;
	code = t_look(fd);
	if (code == T_DISCONNECT) {
	    printf("t_look returns T_DISCONNECT, calling t_rcvdis()\n");
	    if (t_rcvdis(fd, (struct t_discon *) NULL) < 0) {
		t_error("t_rcvdis");
	    } else {
		return 0;
	    }
	} else {
	    printf("t_look returns %x (unexpected)\n", code);
	}
    } else {
	t_error(func);
    }

    return -1;
}

char *
causemsg(int causenumber)
{
    char *msg;
    switch (causenumber) {
      case (T_ATM_CAUSE_UNALLOCATED_NUMBER) :
	  msg = "Unallocated (unassigned) number";
	  break;
      case (T_ATM_CAUSE_NO_ROUTE_TO_TRANSIT_NETWORK) :
	  msg = "No route to specified transit network";
	  break;
      case (T_ATM_CAUSE_NO_ROUTE_TO_DESTINATION) :
	  msg = "No route to destination";
	  break;
      case (T_ATM_CAUSE_NORMAL_CALL_CLEARING) :
	  msg = "Normal call clearing";
	  break;
      case (T_ATM_CAUSE_USER_BUSY) :
	  msg = "User busy";
	  break;
      case (T_ATM_CAUSE_NO_USER_RESPONDING) :
	  msg = "No user responding";
	  break;
      case (T_ATM_CAUSE_CALL_REJECTED) :
	  msg = "Call rejected";
	  break;
      case (T_ATM_CAUSE_NUMBER_CHANGED) :
	  msg = "Number changed";
	  break;
      case (T_ATM_CAUSE_ALL_CALLS_WITHOUT_CALLER_ID_REJECTED) :
	  msg = "User rejects all calls with calling line identification "
		"restriction";
	  break;
      case (T_ATM_CAUSE_DESTINATION_OUT_OF_ORDER) :
	  msg = "Destination out of order";
	  break;
      case (T_ATM_CAUSE_INVALID_NUMBER_FORMAT) :
	  msg = "Invalid number format (address incomplete)";
	  break;
      case (T_ATM_CAUSE_RESPONSE_TO_STATUS_INQUIRY) :
	  msg = "Response to STATUS INQUIRY";
	  break;
      case (T_ATM_CAUSE_UNSPECIFIED_NORMAL) :
	  msg = "Normal Unspecified";
	  break;
      case (T_ATM_CAUSE_REQUESTED_VPCI_VCI_NOT_AVAILABLE) :
	  msg = "Requested VPCI/VCI not available";
	  break;
      case (T_ATM_CAUSE_VPCI_VCI_ASSIGNMENT_FAILURE) :
	  msg = "VPCI/VCI assignment failure";
	  break;
      case (T_ATM_CAUSE_USER_CELL_RATE_NOT_AVAILABLE) :
	  msg = "User cell rate not available";
	  break;
      case (T_ATM_CAUSE_NETWORK_OUT_OF_ORDER) :
	  msg = "Network out of order";
	  break;
      case (T_ATM_CAUSE_TEMPORARY_FAILURE) :
	  msg = "Temporary failure";
	  break;
      case (T_ATM_CAUSE_ACCESS_INFO_DISCARDED) :
	  msg = "Access information discarded";
	  break;
      case (T_ATM_CAUSE_NO_VPCI_VCI_AVAILABLE) :
	  msg = "No VPCI/VCI available";
	  break;
      case (T_ATM_CAUSE_UNSPECIFIED_RESOURCE_UNAVAILABLE) :
	  msg = "Unspecified resource unavailable";
	  break;
      case (T_ATM_CAUSE_QUALITY_OF_SERVICE_UNAVAILABLE) :
	  msg = "Quality of Service unavailable";
	  break;
      case (T_ATM_CAUSE_BEARER_CAPABILITY_NOT_AUTHORIZED) :
	  msg = "Bearer capability not authorized";
	  break;
      case (T_ATM_CAUSE_BEARER_CAPABILITY_UNAVAILABLE) :
	  msg = "Bearer capability not presently available";
	  break;
      case (T_ATM_CAUSE_SERVICE_OR_OPTION_UNAVAILABLE) :
	  msg = "Service or Option unavailable";
	  break;
      case (T_ATM_CAUSE_BEARER_CAPABILITY_NOT_IMPLEMENTED) :
	  msg = "Bearer capability not implemented";
	  break;
      case (T_ATM_CAUSE_INVALID_TRAFFIC_PARAMETERS) :
	  msg = "Unsupported combination of traffic parameters";
	  break;
      case (T_ATM_CAUSE_AAL_PARAMETERS_NOT_SUPPORTED) :
	  msg = "AAL Parameters cannot be supported";
	  break;
      case (T_ATM_CAUSE_INVALID_CALL_REFERENCE_VALUE) :
	  msg = "Invalid call reference value";
	  break;
      case (T_ATM_CAUSE_IDENTIFIED_CHANNEL_DOES_NOT_EXIST) :
	  msg = "Identified channel does not exist";
	  break;
      case (T_ATM_CAUSE_INCOMPATIBLE_DESTINATION) :
	  msg = "Incompatible destination";
	  break;
      case (T_ATM_CAUSE_INVALID_ENDPOINT_REFERENCE) :
	  msg = "Invalid endpoint reference";
	  break;
      case (T_ATM_CAUSE_INVALID_TRANSIT_NETWORK_SELECTION) :
	  msg = "Invalid transit network selection";
	  break;
      case (T_ATM_CAUSE_TOO_MANY_PENDING_ADD_PARTY_REQUESTS) :
	  msg = "Too many pending add party requests";
	  break;
      case (T_ATM_CAUSE_MANDITORY_INFO_ELEMENT_MISSING) :
	  msg = "Mandatory information element is missing";
	  break;
      case (T_ATM_CAUSE_MESSAGE_TYPE_NOT_IMPLEMENTED) :
	  msg = "Message type non-existent or not implemented";
	  break;
      case (T_ATM_CAUSE_INFO_ELEMENT_NOT_IMPLEMENTED) :
	  msg = "Information element non-existent or not implemented";
	  break;
      case (T_ATM_CAUSE_INVALID_INFO_ELEMENT_CONTENTS) :
	  msg = "Invalid information element contents";
	  break;
      case (T_ATM_CAUSE_MESSAGE_INCOMPATIBLE_WITH_CALL_STATE) :
	  msg = "Message is incompatible with call state";
	  break;
      case (T_ATM_CAUSE_RECOVERY_ON_TIMER_EXPIRY) :
	  msg = "Recovery on timer expiry";
	  break;
      case (T_ATM_CAUSE_INCORRECT_MESSAGE_LENGTH) :
	  msg = "Incorrect message length";
	  break;
      case (T_ATM_CAUSE_UNSPECIFIED_PROTOCOL_ERROR) :
	  msg = "Unspecified protocol error";
	  break;
      default :
	  msg = "UNKNOWN Cause message";
	  break;
    }

    return msg;
}
