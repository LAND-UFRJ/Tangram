/*
 * Copyright (C) 1999-2009, Edmundo Albuquerque de Souza e Silva.
 *
 * This file may be distributed under the terms of the Q Public License
 * as defined by Trolltech AS of Norway and appearing in the file
 * LICENSE.QPL included in the packaging of this file.
 *
 * THIS FILE IS PROVIDED AS IS WITH NO WARRANTY OF ANY KIND, INCLUDING
 * THE WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE.  IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL,
 * INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING
 * FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT,
 * NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION
 * WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 */

/******************************************************************************
	
	Magnos Martinello
	Traffic Generator with ATM support
	LAND - Labor. de Analise e Desempenho - UFRJ/COPPE/NCE
        Last Update: 06/06/2000

******************************************************************************/

#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <atm.h>


#include "common_pvc.h"

int open_device() {
  int		fd;

  if ((fd = socket(PF_ATMPVC,SOCK_DGRAM,0)) < 0) {
    perror("socket");
    return -1;
  }
    
  return fd;
}

void clean_up(int fd) {
  
  close(fd);
}

/* pcr is peak cell rate in cells/s */
/* If pcr = 0, traffic is UBR */
void construct_pvc_options(int fd,int pcr) {
  struct 	atm_qos qos;

  memset(&qos,0,sizeof(qos));
  if (pcr > 0) {
    qos.txtp.traffic_class = ATM_CBR;
    qos.txtp.max_pcr       = pcr;
  }
  else
    qos.txtp.traffic_class = ATM_UBR;
 
  qos.txtp.max_sdu = 8192;
  qos.rxtp 	   = qos.txtp;
  qos.aal  	   = ATM_AAL5;

  if (setsockopt(fd,SOL_ATM,SO_ATMQOS,&qos,sizeof(qos)) < 0) {
      perror("setsockopt SO_ATMQOS");
      exit(1);
  }
  
  
  return;
}

int pvc_connect(int fd, int vpi, int vci) {
  struct sockaddr_atmpvc addr;

  memset(&addr,0,sizeof(addr));
  addr.sap_family = AF_ATMPVC;
  
  addr.sap_addr.vpi = vpi;
  addr.sap_addr.vci = vci;
  if (connect(fd,(struct sockaddr *) &addr,sizeof(addr)) < 0) {
    perror("connect");
    return -1;
  }

  return 0;
}

int pvc_disconnect(int fd) {

  return 0;
}

int pvc_read(int fd, char *buff, int len) {

  return read(fd,buff,len);
}

int pvc_write(int fd, char *buff, int len) {

  return write(fd,buff,len);
}
