/*
********************************************************************************
* FILE        : readmps.h
* DESCRIPTION : Header file for all MPS file reading functionality
* AUTHOR      : M H Khaliq
* LICENSE     : MIT
********************************************************************************
*/



#ifndef __READMPS_H
#define __READMPS_H



/*
 *******************************************************************************
 *                               HEADER FILES                                  *
 *******************************************************************************
*/


/* ************************************************************************** */
/* *	                      ANSI C HEADER FILES			    * */
/* ************************************************************************** */


#ifdef __ANSI__C
#include <float.h>
#else
#include <values.h>
#endif /* __ANSI__C */



/* ************************************************************************** */
/* *                 	      LOCAL HEADER FILES			    * */
/* ************************************************************************** */


#include "hash.h"



/*
 *******************************************************************************
 *			           DEFINES			               *
 *******************************************************************************
*/


/* representing +/- infinity */
#ifdef __ANSI__C
#define PLUS_INF	DBL_MAX
#define MINUS_INF	-(DBL_MAX)
#else
#define PLUS_INF	MAXDOUBLE
#define MINUS_INF	-(MAXDOUBLE)
#endif /* __ANSI__C */


/* minimum file size for checking to be done based on the naive file:
 * "
 *	NAME
 *	ROWS
 *	  N	R
 *	COLUMNS
 *	  C	R	1
 *	RHS
 *	ENDATA
 * "
 */

#define MPSFILESIZECHECKLIMIT	41UL


/*
 * maximum values for strings to reduce run-time calculation,
 * including 1 space for their NULL character
 */

#define MAX_LPNAMESIZE	   256
#define MAX_ROWNAMESIZE	   9
#define MAX_COLNAMESIZE	   9
#define MAX_RHSNAMESIZE	   9
#define MAX_RANNAMESIZE	   9
#define MAX_BNDNAMESIZE	   9


/* a series of default values for the rim vectors */

#define DEFAULTRHS	0.0L
#define DEFAULTRNG	0.0L
#define DEFAULTUBD	PLUS_INF
#define DEFAULTLBD	0.0L


/*
 * characters to skip from the start sentinel pointer for the "GetX"
 * series of functions
 */

#define SKIPNAMECHARS	   4
#define SKIPROWSCHARS	   4
#define SKIPCOLSCHARS	   7
#define SKIPRHSCHARS	   3
#define SKIPRNGCHARS	   6
#define SKIPBNDCHARS	   6


/*
 * values added to counter in SetSectionPtrs as it finds the MPS sections
 */

#define JUMPNAMECHARS	   4UL
#define JUMPROWCHARS	   4UL
#define JUMPCOLCHARS	   7UL
#define JUMPRHSCHARS	   3UL
#define JUMPENDCHARS	   6UL


/*
 * an id of -1L is set for non-valid rows/columns, based on output
 * from hash.h
 */

#define BADROW	-1L
#define BADCOL	-1L


/*
 * the maximum number of warnings we state for each of the "GetX"
 * functions that employ "atof"
 */

#define MAXCOLWARNINGS	2UL
#define MAXRHSWARNINGS	2L
#define MAXRNGWARNINGS	2L
#define MAXBNDWARNINGS	2L


/* define the number of standard ASCII characters available for GetROWS */

#define ASCII_LIMIT	256



/*
 *******************************************************************************
 *			            STRUCTURES                                 *
 *******************************************************************************
*/


struct rowdata {
   long	  rowid; /* the unique id for row                 */
   double value; /* the value associated with the element */
};


struct MPSstruct {
   char		  lpname[MAX_LPNAMESIZE]; /* the name of the LP problem */

   long		  numberrows,             /* total rows                 */
		  numbercols;             /* total columns              */

   unsigned long  numberelements,         /* total elements > 0.0L      */
		  numberzeroelements;     /* total elements = 0.0L      */

   char		  **rownames, /* points to array of row names    */
		  **colnames, /* points to array of column names */

                  /* preserved rhs, ranges, bounds names */
                  rhsname[MAX_RHSNAMESIZE], /* for RHS    */
                  ranname[MAX_RANNAMESIZE], /* for RANGES */
                  bndname[MAX_BNDNAMESIZE], /* for BOUNDS */

                  /* the rim vectors */
		  *relationalcodesvector; /* char array   */

   double	  *rhsvector,		   /* double array */
		  *rangesvector,           /*      "       */
	          *upperboundsvector,	   /*      "       */
		  *lowerboundsvector;	   /*      "       */

   /* the kernel */
   struct rowdata *kerneldata_row,  /* the actual kernel data             */
		  **kerneldata_col; /* columnwise pointers to kernel data */
};



/*
 *******************************************************************************
 *	                           PROTOTYPES                                  *
 *******************************************************************************
*/


#ifdef __C__PLUS__PLUS
extern "C" {
#else
extern 
#endif /* __C__PLUS__PLUS */
struct MPSstruct *InitMPSstruct (long, long);
/* pre  : We are given two valid long integers.
 * post : Returns a pointer to a generated MPSstruct
 *	  whose row number corresponds to the first
 *	  argument, and whose column number initially
 *	  matches the second argument.
 */


#ifndef __C__PLUS__PLUS
extern 
#endif
void DeleteMPSstruct (struct MPSstruct **);
/* pre  : We are given a pointer to a pointer to a valid MPSstruct.
 * post : Safely removes all "malloc"-ed items in the MPSstruct prior
 *	  to deleting the structure itself.
 */


#ifndef __C__PLUS__PLUS
extern
#endif
int SetSectionPtrs (char *, 
                     char **, 
                      char **, 
                       char **, 
                        char **,
 	                 char **, 
                          char **, 
                           char **, 
                            long *, 
                             long *,
			      unsigned long);
/* pre  : We are given a pointer to an array of characters, 7 character
 *	  pointer pointers, 2 long integer pointers, and an unsigned long
 *	  value.
 * post : Assigns the character pointer pointers to each possible section
 *	  that can be found in an MPS file.  Since "RANGES" and "BOUNDS" are
 *	  optional MPS sections, their corresponding pointer pointers to char
 *	  are preset to NULL before being passed as arguments to this function.
 *	  
 *	  The function performs many tests for validity of an MPS file.
 *	  It incorporates the ability to ignore comment lines in an MPS file.
 *	  The range of tests catered for are : file too small to be MPS,
 *	  file not MPS file because some compulsory section is missing,
 *	  file not MPS because any one of the possible section names is
 *	  misspelt, file has no row data, file has no column data, and if the
 *	  file has an empty "RHS" section we issue a warning but continue
 *	  processing.
 *
 *	  Returns 1 on success, and 0 on failure.
 */


#ifndef __C__PLUS__PLUS
extern 
#endif
int GetNAME	(char **, struct MPSstruct **, char *, long *);
/* pre  : We are given a valid pointer pointer to an array of char, a
 *	  pointer pointer to a valid MPSstruct, character pointer, and a 
 *	  long integer pointer.
 * post : Isolates the "NAME" section from the array, making sure it does
 *	  not overflow the sentinel value set in the char * argument.
 *	  Updates the MPSstruct's name field to hold the MPS file's name if
 *	  it is given, else that name field holds NULL.  The number of lines
 *	  that the code had to traverse is recorded to help with the final
 *	  update of the long integer addressed by the long * argument.
 *
 *	  The array of characters is updated to create strings, through adding
 *	  NULL delimiters.
 *
 *	  Returns 1 on success, 0 on failure.
 */


#ifndef __C__PLUS__PLUS
extern 
#endif
int GetROWS	(char **, 
                  struct MPSstruct **,
	           rowhashtable *, 
                    char *, 
                     long *);
/* pre  : We are given a pointer to a pointer to an array of characters,
 *	  a pointer to a pointer to a valid MPSstruct, a pointer to a valid
 *	  rowhashtable, and character pointer, and a pointer to a long
 *	  integer.
 * post : Skipping over all comment lines with updating of the line number
 *	  nevertheless, this function intakes the whole of the "ROWS"
 *	  section of the input file character array, placing the data into
 *	  the row hash table.  The MPSstruct is updated to contain the
 *	  row names and the row types and these can be isolated by noting
 *	  that their unique id is determined by the position of the row
 *	  in the row-related arrays, which start at zero.  The data is
 *	  taken as stretching from the first argument to the sentinel
 *	  value held by the fourth argument.  The long integer is updated at
 *	  the end to reflect the number of lines read.
 *
 *	  The function checks that a row is not defined twice, and that the
 *	  type of a row is only 'N', 'L', 'G', or 'E'.
 *
 *	  Returns 1 on success, 0 on failure.
 */


#ifndef __C__PLUS__PLUS
extern 
#endif
int GetCOLUMNS	(char **, 
                  struct MPSstruct **, 
                   elhashtable *,
		    colhashtable *, 
                     rowhashtable *, 
                      char *, 
                       long *);
/* pre  : We are given a pointer to a pointer to an array of characters, a
 *	  pointer to a pointer to a valid MPSstruct, a pointer to a valid
 *	  element hash table, a pointer to a valid column hash table, a
 *	  pointer to a valid row hash table, a character pointer, and a
 *	  pointer to a long integer.
 * post : The scanned array of characters stretches from the first argument
 *	  to the sentinel limit held in the sixth argument; this is the
 *	  "COLUMNS" section of the MPS input file.  The element hash table and
 *	  the column hash table are updated with element and column data,
 *	  helped by referencing row data held in the unchanging rowhashtable.
 *	  The MPSstruct is updated with the column names and the kernel data
 *	  and then a truncation operation is performed to shrink the column-
 *	  related arrays since the number of columns and elements is
 *	  overspecified.  The long integer is updated at the end to reflect
 *	  the number of lines read.
 *
 *	  The function checks for the following: duplicate column names,
 *	  duplicate elements in the same section, elements defined in
 *	  non-contiguous column sections, elements defined with an incorrect
 *	  rowname or a name that does not exist.
 *	  
 *	  If the 'atof' function should give a value of a double as '0.0',
 *	  then, up to a limit, a warning is issued, but processing continues.
 *
 *	  This is the only location for the sort function, used to sort the
 *	  element data of a column rowwise before entry into the kernel.  The
 *	  kernel is sealed by a special element item of row id -1 and value
 *	  0.0 and the last colptr item points to this seal.  Columns can be
 *	  addressed via their location in the "colptrs" array of the 
 *	  MPSstruct.
 *
 *	  The array of characters is updated to create strings, through adding
 *	  NULL delimiters.
 *
 *	  Returns 1 on success, 0 on failure.
 */


#ifndef __C__PLUS__PLUS
extern 
#endif
int GetRHS	(char **, 
                  struct MPSstruct **, 
                   rowhashtable *,
		    char *, 
                     long *);
/* pre  : We are given a pointer to a pointer to an array of characters,
 *	  a pointer to a pointer to a valid MPSstruct, a pointer to a valid
 *	  row hash table, a character pointer, and a pointer to a long integer.
 * post : The scanned array of characters stretches from the first argument
 *	  to the sentinel limit held in the fourth argument; this is the "RHS"
 *	  section of the MPS input file.  The row hash table is not altered,
 *	  but is used to check that rows specified in the section are valid.
 *
 *	  The RHS name is saved, and the dense RHS vector field of the MPS
 *	  structure is updated to reflect the values provided.
 *
 *	  If the 'atof' function should give a value of a double as '0.0',
 *	  then, up to a limit, a warning is issued, but processing continues.
 *
 *	  The array of characters is updated to create strings, through adding
 *	  NULL delimiters.
 *
 *	  The long integer is updated at the end to reflect the lines processed
 *	  from the MPS data.
 *
 *	  Returns 1 on success, 0 on failure.
 */


#ifndef __C__PLUS__PLUS
extern 
#endif
int GetRANGES	(char **, 
                  struct MPSstruct **, 
                   rowhashtable *,
	            char *, 
                     long *);
/* pre  : We are given a pointer to a pointer to an array of characters,
 *	  a pointer to a pointer to a valid MPSstruct, a pointer to a valid
 *	  row hash table, a character pointer, and a pointer to a long integer.
 * post : The scanned array of characters stretches from the first argument
 *	  to the sentinel limit held in the fourth argument; this is the 
 *	  "RANGES" section of the MPS input file.  The row hash table is not
 *	  altered, but is used to check that rows specified in the section are
 *	  valid.
 *
 *	  The RANGES name is saved, and the dense RANGES vector field of the
 *	  MPS structure is updated to reflect the values provided.
 *
 *	  If the 'atof' function should give a value of a double as '0.0',
 *	  then, up to a limit, a warning is issued, but processing continues.
 *
 *	  The array of characters is updated to create strings, through adding
 *	  NULL delimiters.
 *
 *	  The long integer is updated at the end to reflect the lines processed
 *	  from the MPS data.
 *
 *	  Returns 1 on success, 0 on failure.
 */


#ifndef __C__PLUS__PLUS
extern 
#endif
int GetBOUNDS 	(char **, 
                 struct MPSstruct **, 
                  colhashtable *,
		   char *, 
                    long *);
/* pre  : We are given a pointer to a pointer to an array of characters,
 *	  a pointer to a pointer to a valid MPSstruct, a pointer to a valid
 *	  column hash table, a character pointer, and a pointer to a long
 *	  integer.
 * post : The scanned array of characters stretches from the first argument
 *	  to the sentinel limit held in the fourth argument; this is the 
 *	  "BOUNDS" section of the MPS input file.  The column hash table is not
 *	  altered, but is used to check that columns specified in the section
 *	  are valid.
 *
 *	  The BOUNDS name is saved, and the dense upper BOUNDS vector field of
 *	  the MPS structure is updated, as well as the lower BOUNDS vector
 *	  to reflect the values provided.  The exact values are preset to
 *	  +(infinity) and zero respectively first of all.
 *
 *	  If the 'atof' function should give a value of a double as '0.0',
 *	  then, up to a limit, a warning is issued, but processing continues.
 *
 *	  The array of characters is updated to create strings, through adding
 *	  NULL delimiters.
 *
 *	  The long integer is updated at the end to reflect the lines processed
 *	  from the MPS data.
 *
 *	  Returns 1 on success, 0 on failure.
 */


#ifndef __C__PLUS__PLUS
extern 
#endif
struct MPSstruct *GetMPSdata (char *);
/* pre  : We are given a valid pointer to a string of characters.
 * post : This is the overall function responsible for transforming the
 *	  MPS input file located by its argument into the final MPS structure.
 *
 *	  In any error situation, it returns NULL, otherwise it returns a
 *	  pointer to an internally allocated MPS structure with the MPS
 *	  data residing within it.
 */

#ifdef __C__PLUS__PLUS
}
#endif /* __C__PLUS__PLUS */


/* ************************************************************************** */
/* *                    	END OF PROTOTYPES                           * */
/* ************************************************************************** */



#endif /* __READMPS_H */


/* *********************************** END ********************************** */
