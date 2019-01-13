/*
********************************************************************************
* FILE        : timefunc.h
* DESCRIPTION : Header file for all timing functions
* AUTHOR      : M H Khaliq
* LICENSE     : MIT
********************************************************************************
*/



#ifndef __TIMEFUNC_H
#define __TIMEFUNC_H


/*
 *******************************************************************************
 *                                HEADER FILES                                 *
 *******************************************************************************
*/


/* ************************************************************************** */
/* *                         SYSTEM HEADER FILES                            * */
/* ************************************************************************** */


#include <time.h>
#ifndef __ANSI__C
#include <sys/times.h>
#ifndef CLK_TCK
#include <unistd.h>
#define CLK_TCK    sysconf(_SC_CLK_TCK)
#endif /* CLK_TCK */
#endif /* __ANSI__C */



/*
 *******************************************************************************
 *                                   DEFINES                                   *
 *******************************************************************************
*/


#ifdef __ANSI__C
#define TIMESTRUCTURE clock_t
#else
#define TIMESTRUCTURE struct tms
#endif /* __ANSI__C */



/*
 *******************************************************************************
 *                                   MACROS                                    *
 *******************************************************************************
*/


#ifdef __ANSI__C
#define gettimedata(tms_ptr)   (*(tms_ptr) = clock ())
#else  
#define gettimedata(tms_ptr)   ((void)times((tms_ptr)))
/* pre  : We are given a valid pointer to "struct tms".
 * post : Sets up the data for "tms_ptr".
 */
#endif /* __ANSI__C */

#ifdef __ANSI__C
#define gettotaltime(start,end)   \
       ((double)((end) - (start)) / (double)CLOCKS_PER_SEC)
#else
#define gettotaltime(start,end)  \
        ((double)(((end).tms_stime + (end).tms_utime) - \
	         ((start).tms_stime + (start).tms_utime))/(double)CLK_TCK)
/* pre  : We are given two valid "tms" structures.
 * post : Returns the floating-point difference to 3 decimal places of
 *        the time difference in seconds between the first argument, and
 *        the second.
 */
#endif /* __ANSI__C */



/* ************************************************************************** */
/* *                               END OF MACROS                            * */
/* ************************************************************************** */



#endif /* __TIMEFUNC_H */


/* ********************************* END *********************************** */
