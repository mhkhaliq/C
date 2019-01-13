/*
********************************************************************************
* FILE        : fileio.h
* DESCRIPTION : Header file for all input-output routines
* AUTHOR      : M H Khaliq
* LICENSE     : MIT
********************************************************************************
*/



#ifndef __FILEIO_H
#define __FILEIO_H



/*
 *******************************************************************************
 *                                 DEFINES                                     *
 *******************************************************************************
*/


#ifdef __ANSI__C
/* 1 megabyte buffer */
#define READBUFFERSIZE	1048576
#else
/* POSIX defines the error return value of "stat" as -1 */
#define BADSTAT		-1
#endif /* __ANSI__C */



/*
 *******************************************************************************
 *                      	STRUCTURES				       *
 *******************************************************************************
*/


#ifdef __ANSI__C
/* we will input file data temporarily to a linked list of char arrays */
struct datanode {
   char            data[READBUFFERSIZE];
   struct datanode *next;
};
#endif /* __ANSI__C */



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
char * Readfileintomemory (char *, unsigned long *);
/* pre  : We are given a valid character pointer, unsigned long pointer.
 * post : Returns a pointer to an array of characters holding
 *        the whole of the data from the file identified by the
 *        (char *) argument, and the unsigned long pointer addresses the
 *        the file size.
 *
 *        NULL is returned on any error condition.
 */
#ifdef __C__PLUS__PLUS
}
#endif /* __C__PLUS__PLUS */


/* ************************************************************************** */
/* *	                     END OF PROTOTYPES			            * */
/* ************************************************************************** */


#endif /* __FILEIO_H */


/* ********************************** END *********************************** */
