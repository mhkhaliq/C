/*
********************************************************************************
* FILE        : sort.h
* DESCRIPTION : Header file for all sort functionality
* AUTHOR      : M H Khaliq
* LICENSE     : MIT
********************************************************************************
*/



#ifndef __SORT_H
#define __SORT_H



/*
 *******************************************************************************
 *                                  MACROS                                     *
 *******************************************************************************
*/


#define sort(ptr_to_array,numitems) (heapsort((ptr_to_array),(numitems)))
/* pre  : We are given a pointer to a valid array of long integers,
 *        with preferably greater than 3 elements, and a long integer
 *        giving the number of elements to consider.
 * post : Calls the implemented sort function to generate an array
 *        of the same elements sorted to ascending order.
 */


/* ************************************************************************** */
/* *                            END OF MACROS                               * */
/* ************************************************************************** */



/*
 *******************************************************************************
 *                               PROTOTYPES                                    *
 *******************************************************************************
*/

#ifdef __C__PLUS__PLUS
extern "C" {
#else
extern 
#endif /* __C__PLUS__PLUS */
void heapsort (long **, long);
/* pre  : We are given a valid pointer to a long pointer and a long
 *        number.
 * post : The array provided is sorted from the start to the nth
 *        element defined by the provided long.  The order is ascending.
 */

#ifdef __C__PLUS__PLUS
}
#endif /* __C__PLUS__PLUS */


/* ************************************************************************** */
/* *                          END OF PROTOTYPES                             * */
/* ************************************************************************** */



#endif /* __SORT_H */



/* ******************************** END ************************************* */
