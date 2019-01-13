/*
********************************************************************************
* FILE        : sort.C
* DESCRIPTION : Implementation file for sort.h
* AUTHOR      : M H Khaliq
* LICENSE     : MIT
********************************************************************************
*/




/*
 *******************************************************************************
 *                              HEADER FILES                                   *
 *******************************************************************************
*/


/* ************************************************************************** */
/* *                        SYSTEM HEADER FILES                             * */
/* ************************************************************************** */


#include <stdlib.h>



/* ************************************************************************** */
/* *                        LOCAL HEADER FILES                              * */
/* ************************************************************************** */


#include "sort.h"



/*
 *******************************************************************************
 *                           LOCAL PROTOTYPES                                  *
 *******************************************************************************
*/


extern void downheap (long **, long, long);
/* pre  : We are given a pointer to pointer to a valid
 *        array of long integers, and we employ this procedure
 *        via the 2 long arguments, both different and not
 *        negative.
 * post : The procedure effects a pushing down of the intitial
 *        parent node down to the bottom of the heap, then imposes
 *        the heap comdition that a parent node has a value greater
 *        than its siblings throughout the heap.
 */



/*
 *******************************************************************************
 *                               IMPLEMENTATIONS                               *
 *******************************************************************************
*/


/* ************************************************************************** */
/* *                                DOWNHEAP                                * */
/* ************************************************************************** */


void downheap (long **array, long start, long end)
/* pre  : We are given a pointer to pointer to a valid
 *        array of long integers, and we employ this procedure
 *        via the 2 long arguments, both different and not
 *        negative.
 * post : The procedure effects a pushing down of the intitial
 *        parent node down to the bottom of the heap, then imposes
 *        the heap comdition that a parent node has a value greater
 *        than its siblings throughout the heap, between 'start' and
 *        'end' in the array pointed to by the pointer pointer 'array'.
 *
 * NOTE : The comparison function is integrated into this code.
 */
{
   register long i,      /* general counter, indexer                  */
                 child,  /* used for heap movement of data            */
                 parent, /* data movement in optimisation loop at end */
                 temp1,  /* temporary values for swapping             */
                 temp2,  /*                    "                      */
                 first,  /* sentinel for start of array               */
                 last;   /* sentinel for end of array                 */
   long *tempptr1 = NULL, /* used to avoid too many derefernces of array */
        *tempptr2 = NULL; /*                     "                       */


   /* strictly the compiler should do this */
   first = start;
   last  = end;


   /*
    * make use of the fact that (2*i) = (i+i), additions are faster
    * than multiplications
    */
   for (i = first; 
	 (child = ((i + i) + 1L)) <= last; 
	  i = child)
   {
      temp1 = child + 1L;

      if ((temp1 <= last) && (*((*array) + temp1) > *((*array) + child)))
      {
         child++;
      }/* end if (1) */


      tempptr1 = ((*array) + i);
      tempptr2 = ((*array) + child);

      temp1 = *tempptr1;

      *tempptr1 = *tempptr2;
      *tempptr2 = temp1;

   }/* end for i */


   /* optimisation loop working on the parent of the heap */
   for ( ; 
	  ; 
	   i = parent)
   {
      parent = (i - 1L) / 2L;

      tempptr1 = ((*array) + parent);
      tempptr2 = ((*array) + i);

      temp1 = *tempptr1;
      temp2 = *tempptr2;

      if ((parent < first) || (parent == i) || (temp1 > temp2))
      {
         break;
      }/* end if (1) */


      *tempptr1 = temp2;
      *tempptr2 = temp1;

   }/* end for i */


   return ;

}



/* ************************************************************************** */
/* *                               HEAPSORT                                 * */
/* ************************************************************************** */


void heapsort (long **array, long numels)
/* pre  : We have 'array' as a valid pointer to a long pointer, and
 *        'numels' is a valid long.
 * post : Implementation of heapsort to order (nlog2(n)) is employed
 *        to sort the first numels elements contained in array into
 *        ascending order.
 */
{
   long *tempptr1 = NULL, /* helps minimize time spent dereferencing array */
        *tempptr2 = NULL; /*                       "                       */
   register long i,    /* general counter, indexer          */
                 end,  /* sentinel                          */
                 temp; /* holds temporary values for a swap */


   end = numels - 1L;

   /* heap-building */
   for (i = ((end - 1L) / 2L); 
	 i >= 0L; 
	  i--)
   {
      downheap (array, i, end);
   }/* end for i */


   for (i = end; 
	 i > 0L; 
	  )
   {
      tempptr1 = ((*array) + i); /* point to ith element of array */
      tempptr2 = *array;         /* point to head of array        */

      temp = *tempptr1; /* save the value of the ith element */

      *tempptr1 = *tempptr2; /* first stage of swap */

      *tempptr2 = temp; /* second stage of swap */

      i--;

      downheap (array, 0L, i);

   }/* end for i */


   return ;

}



/* ************************************************************************** */
/* *                         END OF IMPLEMENTATIONS                         * */
/* ************************************************************************** */




/* ********************************** END *********************************** */
