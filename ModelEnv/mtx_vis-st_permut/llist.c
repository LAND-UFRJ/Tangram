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

/*****************************************************************************
 *
 *    Linked List (with head LLNode) routines
 *
 *****************************************************************************/

#include <stdlib.h>
#include <stdio.h>

struct matrixBlock
{
    int initialState;
    int size;
};

typedef struct matrixBlock MatrixBlock;

struct LLNode
{
    struct matrixBlock info;
    struct LLNode *next;
};


typedef struct LLNode LLNode;

struct llist
{
    struct LLNode *ptfirst;
    int nelem;
};

typedef struct llist LList;

void LList_InitLList(LList *LL);
int LList_IsEmpty(LList *LL);
int LList_Search(LList *LL, int fs, LLNode **ptant, LLNode **pt);
int LList_SearchGreater(LList *LL, int fs, LLNode **pt);
int LList_Insert(LList *LL, MatrixBlock *mb);
int LList_Remove(LList *LL, int fs, MatrixBlock *mb);
int LList_RemovePos(LList *LL, int pos);
int LList_GetFirst(LList *LL, MatrixBlock *mb);
void LList_Flush(LList *LL);
String* LList_GetStringArray(LList *LL);


void MatrixBlock_Show(MatrixBlock *mb);

void LList_InitLList(LList *LL)
{
    LL->ptfirst = (LLNode*)malloc(sizeof(LLNode));  /* list header */
    LL->ptfirst->next = NULL;
    LL->nelem = 0;
    LL->ptfirst->info.initialState = 0;
    LL->ptfirst->info.size = 0;
}

int LList_IsEmpty(LList *LL)
{
    if(LL->nelem==0)
        return 1;
    else
        return 0;
}

int LList_SearchGreater(LList *LL, int is, LLNode **pt)
{
    LLNode *ptant;

    *pt = LL->ptfirst->next;
    ptant = LL->ptfirst;

    while(*pt != NULL)
    {
        if((*pt)->info.initialState < is)
        {
            *pt = (*pt)->next;
            ptant = ptant->next;
        }
        else
        {
            if(ptant != LL->ptfirst)
                *pt = ptant;
            return 1;
        }
    }
    
    if ( ( ptant != LL->ptfirst ) && ( ((ptant->info.initialState + ptant->info.size)-1) >= is ) )
    {
        *pt = ptant;
        return 1;
    }

    return -1;
}

int LList_Search(LList *LL, int is, LLNode **ptant, LLNode **pt)
{
    LLNode *ptaux;
    *ptant = LL->ptfirst;
    *pt = NULL;
    ptaux = LL->ptfirst->next;
    while (ptaux!=NULL)
    {
        if (ptaux->info.initialState < is)
        {
            *ptant = ptaux;
            ptaux = ptaux->next;
        }
        else
        {
            if (ptaux->info.initialState == is)
            {
                *pt = ptaux;
            }
            ptaux = NULL;
        }
    }
    if (*pt==ptaux) 
        return 1; 
    else 
        return -1;
}

int LList_Insert(LList *LL, MatrixBlock *mb)
{
    LLNode *ptprev, *pt, *ptprox;

    LList_Search(LL, mb->initialState, &ptprev, &pt);

    if (pt==NULL) 
    {    
        if (mb->initialState <= (ptprev->info.initialState + ptprev->info.size -1))
            return -1; /* overlapping with the previous block */

        ptprox = ptprev->next;
        if (ptprox != NULL)
        {
            if ((mb->initialState + mb->size -1) >= ptprox->info.initialState)
            {
                /* mb->initialState doesn't change */
                mb->size = ptprox->info.initialState - mb->initialState;

                if((mb->initialState > 0) && (mb->size > 0))
                {
                    pt = (LLNode*)malloc(sizeof(LLNode));
                    pt->info = *mb;
                    pt->next = ptprev->next;
                    ptprev->next = pt;
                    LL->nelem++;
                }

                return -1; /* overlapping with the next block, but may have added a smaller block */
            }
        }    

        pt = (LLNode*)malloc(sizeof(LLNode));
        pt->info = *mb;
        pt->next = ptprev->next;
        ptprev->next = pt;
        LL->nelem++;

        return 1; /* Added a complete block */
    }
    else
    {
        return -1;  /* duplicated initial states */
    }
}    

int LList_Remove(LList *LL, int is, MatrixBlock *mb)
{
    
    LLNode *ptant, *pt;
    
    if (is != (int)NULL)  /* if a key was defined...*/
        LList_Search(LL, is, &ptant, &pt); /* search for the LLNode to be deleted */
    else    /* else, removes the first one of the list */
    {
        if (!LList_IsEmpty(LL)) 
        {
            ptant = LL->ptfirst;
            pt = LL->ptfirst->next;
        }
        else
            return -1;
    }

    if (pt!=NULL)
    {
        ptant->next = pt->next;

        if (mb!=NULL)
            *mb = pt->info;

        free(pt);
        LL->nelem--;
        return 1;
    }
    else
        return -1;
}

int LList_RemovePos(LList *LL, int pos)
{
    LLNode *ptant, *pt;

    if (LList_IsEmpty(LL))
        return -1;

    ptant = LL->ptfirst;

    while ((ptant->next != NULL) && (pos!=0))
    {
        pos--;
        ptant = ptant->next;
    }

    if(pos == 0)
    {
        pt = ptant->next;
        ptant->next = pt->next;
        free(pt);
        LL->nelem--;
        return 1;
    }
    return -1;
}


int LList_GetFirst(LList *LL, MatrixBlock *mb)
{
    if (!LList_IsEmpty(LL))
    {
        *mb = (LL->ptfirst->next->info);
        return 0;
    }
    else
        return -1;
}

void LList_Flush(LList *LL)
{
    while (LList_Remove(LL,(int)NULL,NULL)!=-1);

    free(LL->ptfirst);
}

#ifdef DEBUG
void LList_Show(LList *LL)
{
	LLNode *pt;
	for (pt=LL->ptfirst->next;pt!=NULL;pt=pt->next)
	{
		MatrixBlock_Show(&pt->info);
	}
	
}

void MatrixBlock_Show(MatrixBlock *mb)
{
	fprintf(stderr, "Matrix Block - finalState: %d size: %d\n", 
		mb->initialState, mb->size);
}
#endif
