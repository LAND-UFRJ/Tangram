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

   This class inheritances from the template class for generic binary trees, 
adding the balancing characteristics to these trees.
   
******************************************************************************/

#ifndef __TREEAVL_H__
#define __TREEAVL_H__

#include <iostream>
#include "tree.h"

template <class T>
class TreeAVL : public Tree<T>
{
  private:
    int balance;
    int need_rotate;

    TreeAVL* rotationleft( TreeAVL *pt )
    {
        TreeAVL *ptu;

        ptu = (TreeAVL<T> *) pt->right;
        if( ptu->balance == 1 )
        {
            pt->right = ptu->left;
            ptu->left = pt;
            pt->balance = 0;
            pt = ptu;
        }
        else
        {
            TreeAVL *ptv;

            ptv = (TreeAVL<T> * ) ptu->left;
            ptu->left = ptv->right;
            ptv->right = ptu;
            pt->right = ptv->left;
            ptv->left = pt;

            if( ptv->balance == 1 )
                pt->balance = -1;
            else
                pt->balance = 0;

            if ( ptv->balance == -1 )
                ptu->balance = 1;
            else
                ptu->balance = 0;

            pt = ptv;
        } 
        pt->balance = 0;
        need_rotate = FALSE;
        return pt;
    }



    TreeAVL* rotationright( TreeAVL *pt )
    {
      TreeAVL *ptu;
      
      ptu = (TreeAVL<T> * ) pt->left;
      if ( ptu->balance == -1 )
      {
          pt->left    = ptu->right;
          ptu->right  = pt;
          pt->balance = 0;
          pt          = ptu;
      }
      else
      {
          TreeAVL *ptv;

          ptv = (TreeAVL<T> * ) ptu->right;
          ptu->right = ptv->left;
          ptv->left  = ptu;
          pt->left   = ptv->right;
          ptv->right = pt;
          if( ptv->balance == -1 )
              pt->balance = 1;
          else
              pt->balance = 0;
          if( ptv->balance == 1 )
              ptu->balance = -1;
          else
              ptu->balance = 0;

          pt = ptv;
      }
      pt->balance = 0;
      need_rotate = FALSE;
      return pt;
    }
  
    TreeAVL* set_bal_left( void )
    {
        switch( balance )
        {
            case 1: 
                balance     = 0 ;
                need_rotate = FALSE;
                return this;
            case 0:
                balance = -1;
                return this;
            case -1:
                return rotationright( this );
            default:
                return this;
        }
    }


    TreeAVL* set_bal_right( void )
    {
        switch( balance )
        {
            case -1: 
                balance = 0 ;
                need_rotate = FALSE;
                return this;
            case 0:
                balance = 1;
                return this;
            case 1:
                return rotationleft( this );
            default:
                return this;
        }
    }

  
  public:

    TreeAVL()
    { 
        balance = 0;
        need_rotate = FALSE;
    }

    ~TreeAVL()  
    {
        if( this->nod )
            delete this->nod;
        if( this->left )
            delete (TreeAVL<T>*)this->left;
        if( this->right )
            delete (TreeAVL<T>*)this->right;
    }

    TreeAVL* insert( T* value )
    {
        if( this->nod == NULL )
        {
            this->nod   = value;
            need_rotate = TRUE;
            return this;
        }
        else
        {
            if( *value == *this->nod )
            {
                need_rotate = FALSE;
                return this;
            }
            else
                if ( *value < *this->nod )
                    if( this->left == NULL )
                    {
                        this->left  = new TreeAVL<T>;
                        this->left->setnod( value );
                        need_rotate = TRUE;
                        return set_bal_left();
                    }    
                    else
                    {
                        this->left = ( ( TreeAVL<T> *)this->left)->insert(value);
                        if( need_rotate )
                            return set_bal_left();
                        else
                            return this;
                    }
                else
                    if( this->right == NULL)
                    {
                        this->right = new TreeAVL<T>;
                        this->right->setnod( value );
                        need_rotate = TRUE;
                        return set_bal_right();
                    }
                    else
                    {
                        this->right = ( (TreeAVL<T> *)this->right)->insert( value );
                        if( need_rotate )
                            return set_bal_right();
                        else
                            return this;
                    }
        }
    }

    void print()
    {
        std::cout << "Balance " << balance << "\t";
        if( this->nod )
            this->nod->print();

        if( this->left )
            ( (TreeAVL<T> * ) this->left)->print();

        if( this->right )
            ( ( TreeAVL<T> * ) this->right)->print();
    }
};

#endif
