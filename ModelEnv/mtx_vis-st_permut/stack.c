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
 *	Stack routines
 *
 *****************************************************************************/
 

#include <stdlib.h>
#include <stdio.h>

struct windowBlock {

	int x, y;
	int size;

};

typedef struct windowBlock WindowBlock;



struct node{
	struct windowBlock info;
	struct node *next;
};


typedef struct node Node;

struct stack {
	struct node *pttop;
	int nelem;
};

typedef struct stack Stack;


void Stack_InitStack( Stack *S );

int Stack_IsEmpty(Stack *S);

int Stack_Pop(Stack *S, WindowBlock *wb);

int Stack_GetFirst(Stack *S, WindowBlock *wb);

void Stack_Push(Stack *S, WindowBlock *wb);

void Stack_Flush(Stack *S);

void Stack_InitStack(Stack *S)
{
	S->pttop = NULL;
	S->nelem = 0;
}

int Stack_IsEmpty(Stack *S) {

	if(S->nelem==0)
		return 1;
	else
		return 0;
}

int Stack_Pop(Stack *S, WindowBlock *wb){
	
	Node *aux;
	
	if(!Stack_IsEmpty(S)){
		aux = S->pttop;
		if(S->pttop->next!=NULL)
			S->pttop=S->pttop->next;
		else
			S->pttop=NULL;
		(S->nelem)--;	
		if (wb!=NULL) *wb=(aux->info);
		free (aux);
		return 0;
	
	}
	else
		return -1;
}	

int Stack_GetFirst(Stack *S, WindowBlock *wb) {

	if (!Stack_IsEmpty(S)) {
		*wb = (S->pttop->info);
		return 0;
	}
	else
		return -1;
}


void Stack_Push(Stack *S, WindowBlock *wb){

	Node *new;
        if((new=(Node *)
		    malloc(sizeof(Node)))==NULL){
		fprintf(stderr,"Couldn't allocate new stack node.");
		exit(1);
	}

	new->info=*wb;
	new->next=S->pttop;
	S->pttop=new;
	(S->nelem)++;
		
}

void Stack_Flush(Stack *S)
{
	while (Stack_Pop(S,NULL)!=-1) {}
}


void WindowBlock_Show(WindowBlock *wb)
{
	fprintf(stderr, "Window Block - x: %d y: %d size: %d\n", 
		wb->x, wb->y, wb->size);
}

