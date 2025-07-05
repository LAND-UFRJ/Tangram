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



#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <pvm3.h>

#define MAX_SPMD_SPAWNS 1024
#define _TASKASSIGNMENT_ 3
#define _TASKEXIT_ 200
#define _HOSTCRASH_ 201
#define min(n,m) ((n<m)?n:m)

main(int argc, char *argv[])
{
    int mytid;                  /* my task id */
    int tids[MAX_SPMD_SPAWNS];				/* slave task ids */
    int n, nproc, numt, run, i, k, j, who, msgtype, nhost, narch, njobs, assgjobs, jobsinit;
    struct pvmhostinfo *hostp;
    struct timeval tmout = {1,0}; // ten seconds to timeout

    if ((argc < 3) || ((njobs = atoi(argv[1])) == 0))
    {
        fprintf(stderr,"Invalid parameter\n   Usage masternode jobstodo $(SLAVEDIR)/slavename <slave parameters>\n\n");
        exit(1);
    }

    /* enroll in pvm */
    mytid = pvm_mytid();

    /* Set number of slaves to start */
    pvm_config( &nhost, &narch, &hostp );
    nproc = min(nhost,njobs);
    if( nproc > MAX_SPMD_SPAWNS ) nproc = MAX_SPMD_SPAWNS ;
	fprintf(stderr,"Spawning %d worker tasks with %d jobs... " , nproc, njobs);

    /* start up slave tasks */
    pvm_catchout(stdout);

    argv[argc]=NULL;
    numt=pvm_spawn(argv[2], &argv[3], PvmTaskDefault, "", nproc, tids);
    if( numt < nproc ){
       fprintf(stderr,"\n Trouble spawning slaves. Aborting. Error codes are:\n");
       for( i=numt ; i<nproc ; i++ ) {
          fprintf(stderr,"TID %d >>>>> ",i);
          switch (tids[i])
          {
          	case PvmBadParam:
              fprintf(stderr,"giving an invalid argument value.\n");
              break;

       		case PvmNoHost:
              fprintf(stderr,"Specified host is not in the virtual machine.\n");
              break;

       		case PvmNoFile:
              fprintf(stderr,"Specified  executable  cannot be found. The default"
              " location PVM looks  in  is  ~/pvm3/bin/ARCH,  where"
              " ARCH is a PVM architecture name.\n");
              break;

       		case PvmNoMem:
              fprintf(stderr,"Malloc failed. Not enough memory on host.\n");
              break;

       		case PvmSysErr:
              fprintf(stderr,"pvmd not responding.\n");
              break;

       		case PvmOutOfRes:
              fprintf(stderr,"out of resources.\n");
              break;
         }
       }

    }

    if( numt > 0 ){
		fprintf(stderr,"SUCCESSFUL\n");
        for( i=0 ; i<numt ; i++ )
            fprintf(stderr,"[%d]" , tids[i]);
    }
    else
    {
       fprintf(stderr,"Could not start tasks!\n");
       exit(1);
    }
    fprintf(stderr,"END\n");
    pvm_notify(  PvmTaskExit, _TASKEXIT_, numt, tids );
    pvm_notify(  PvmHostDelete, _HOSTCRASH_, numt, tids );

    /* Broadcast initial tasks to slave tasks */
    pvm_initsend(PvmDataDefault);
    assgjobs = 1;
    pvm_pkint(&assgjobs, 1, 1);
    run=-1;
    pvm_pkint(&run, 1, 1); // sends the run number
    pvm_mcast(tids, numt, _TASKASSIGNMENT_);

    run = numt;
    jobsinit = 0;
		/* Wait from slaves responses */
    for( i=jobsinit ; i<njobs-numt; i++ )
    {
        if (pvm_trecv( -1, _TASKASSIGNMENT_ , &tmout) > 0)
        {
                pvm_upkint( &who, 1, 1 );
                pvm_initsend(PvmDataDefault);
                assgjobs = 1;
                pvm_pkint(&assgjobs, 1, 1);
                pvm_pkint(&run, 1, 1); // sends the run number
                run++;
                fprintf(stderr,"\n Task [0x%x] received run #%d...",who,run);
                pvm_send(tids[who],_TASKASSIGNMENT_);
        }
        else i--; // will not count this timeout
        while (pvm_nrecv(-1, _TASKEXIT_) > 0)
        {
            pvm_upkint(&who,1,1);
            fprintf(stderr,"\n Task [0x%x] has exited...",who);
            for (j=0; j<numt; j++)
                if (tids[j] == who) break;
            fprintf(stderr,"task id=%d removed from tasks list\n",j);
            for (k=j; k<numt-1; k++)
                tids[k] = tids[k+1];
            numt--;
            pvm_initsend(PvmDataDefault);
            pvm_pkint(tids, numt, 1);
            pvm_send(tids[0],_TASKEXIT_);
            njobs++;
        }
        while (pvm_nrecv(-1, _HOSTCRASH_) > 0)
        {
            pvm_upkint( &who, 1, 1 );
            fprintf(stderr,"\n Host which had task [0x%x] has died!\n",who);
            for (j=0; j<numt; j++)
                if (tids[j] == who) break;
            fprintf(stderr,"Task id=%d removed from tasks list... trying to redistribute last job...\n The simulation program may freeze!!!!\n",j);
            for (k=j; k<numt-1; k++)
                tids[k] = tids[k+1];
            numt--;
            pvm_initsend(PvmDataDefault);
            pvm_pkint(tids, numt, 1);
            pvm_send(tids[0],_TASKEXIT_);
            njobs++;
        }
    }
    /* Broadcast that tasks should finish when the last job finishes */
    pvm_initsend(PvmDataDefault);
    assgjobs = 0;
    pvm_pkint(&assgjobs, 1, 1);
    run=njobs-numt;
    pvm_pkint(&run, 1, 1); // sends the run number
    pvm_mcast(tids, numt, _TASKASSIGNMENT_);

    fprintf(stderr,"\n\n Waiting %d tasks to die...",numt);
    if (numt > 0)
        while (pvm_recv( -1, _TASKEXIT_ ) > 0)
            if (--numt == 0) break;
    /* Program Finished exit PVM before stopping */
    fprintf(stderr,"\n\n\n");
    pvm_exit();
}

/*
 *         PVM version 3.4:  Parallel Virtual Machine System
 *               University of Tennessee, Knoxville TN.
 *           Oak Ridge National Laboratory, Oak Ridge TN.
 *                   Emory University, Atlanta GA.
 *      Authors:  J. J. Dongarra, G. E. Fagg, M. Fischer
 *          G. A. Geist, J. A. Kohl, R. J. Manchek, P. Mucci,
 *         P. M. Papadopoulos, S. L. Scott, and V. S. Sunderam
 *                   (C) 1997 All Rights Reserved
 *
 *                              NOTICE
 *
 * Permission to use, copy, modify, and distribute this software and
 * its documentation for any purpose and without fee is hereby granted
 * provided that the above copyright notice appear in all copies and
 * that both the copyright notice and this permission notice appear in
 * supporting documentation.
 *
 * Neither the Institutions (Emory University, Oak Ridge National
 * Laboratory, and University of Tennessee) nor the Authors make any
 * representations about the suitability of this software for any
 * purpose.  This software is provided ``as is'' without express or
 * implied warranty.
 *
 * PVM version 3 was funded in part by the U.S. Department of Energy,
 * the National Science Foundation and the State of Tennessee.
 */
