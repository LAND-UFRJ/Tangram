/******************************************************************************
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
 ******************************************************************************/

#ifdef _SOLARIS_
// typedef long    int64_t;
 #include <sys/types.h>
#endif

#include <jni.h>
#include "Processo.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>


JNIEXPORT void JNICALL
  Java_Tangram2_Processo_Execute( JNIEnv *env, jobject obj, jstring jcmd, jstring jarg )
{
    const char *cmd = (*env)->GetStringUTFChars( env, jcmd, 0 );
    const char *arg = (*env)->GetStringUTFChars( env, jarg, 0 );

    /*fprintf( stderr, "Vou criar o process \"%s %s\"\n", cbuf, abuf );*/

    if( fork() == 0 )
        execlp( cmd, cmd, arg, NULL );
    
    (*env)->ReleaseStringUTFChars( env, jcmd, cmd );
    (*env)->ReleaseStringUTFChars( env, jarg, arg );

    return;
}

JNIEXPORT void JNICALL
  Java_Tangram2_Processo_SetDir( JNIEnv *env, jobject obj, jstring newDir )
{
    const char *jpath = (*env)->GetStringUTFChars( env, newDir, 0 );

    chdir( jpath );

    (*env)->ReleaseStringUTFChars( env, newDir, jpath );

    
}
