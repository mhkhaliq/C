/*
********************************************************************************
* FILE        : readmps.C
* DESCRIPTION : Implementation file for readmps.h
* AUTHOR      : M H Khaliq
* LICENSE     : MIT
********************************************************************************
*/



/*
 *******************************************************************************
 *                               HEADER FILES                                  *
 *******************************************************************************
*/


/* ************************************************************************** */
/* *                         ANSI C HEADER FILES                            * */
/* ************************************************************************** */


#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <ctype.h>
#include <string.h>



/* ************************************************************************** */
/* *                         LOCAL HEADER FILES                             * */
/* ************************************************************************** */


#include "readmps.h"
#ifdef DRIVER
#include "timefunc.h"
#endif /* DRIVER */
#include "fileio.h"
#include "sort.h"



/* ************************************************************************** */
/* *                            LOCAL DEFINES                               * */
/* ************************************************************************** */


/* lookup define for sort, EXTREMELY INADVISABLE TO ALTER */
#define LOOKUPTABLESIZE	150001L


#ifdef DRIVER
/* local defines for presentation */

#define NUMBUCKETS1	11L
#define NUMBUCKETS2	21L
#define MAXSTARS	50

/* a 32kb I/O buffer size for the output file */
#define FILEBUFFSIZE    32768

/* end of presentation defines */
#endif /* DRIVER */



/* ************************************************************************** */
/* *                          LOCAL CONSTANTS                               * */
/* ************************************************************************** */


/* 
 * speed increasing lookup table for data purging
 * 
 *
 * const long nlogntable[LOOKUPTABLESIZE] = {
 *	0L, 0L, 2L, 5L, 8L, 
 *	12L, 16L, 20L, 24L, 29L, 
 *	34L, 39L, 44L, 49L, 54L, 
 *		.
 *		.
 *	2579005L, 2579023L, 2579042L, 2579060L, 2579079L, 
 *	2579098L, 2579116L, 2579135L, 2579154L, 2579172L, 
 *	2579191L 
 * };
 *
 */

#include "nlogn.tbl"

       
/*
 * A special lookup table of flags for GetROWS so that checks for row 
 * type are constant time.
 * Only the characters 'N', 'L', 'G', 'E' have flag values of 1.
 *
 * NOTE : "GetROWS" is the only function to employ this constant array.
 *        However, doing the "correct" action of placing the constant
 *        within the function would lead to stack frame inefficiency
 *        as the whole of this 256-byte data will be placed at run-time.
 *        It is far better to have the data in the stack at the start,
 *        i.e. have the compiler be done with this data at compile time.
 *
 */
const unsigned char row_flags[ASCII_LIMIT] = {
   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
   0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 1, 0, 1, 0, 
   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 
};


#ifdef DRIVER
/* presentation constants */

const double buckets[NUMBUCKETS1] = {
   0.00001,
   0.0001,
   0.001,
   0.01,
   0.1,
   1.0,
   10.0,
   100.0,
   1000.0,
   10000.0,
   100000.0
};

/* end of presentation constants */
#endif /* DRIVER */



/*
 *******************************************************************************
 *                            LOCAL PROTOTYPES                                 *
 *******************************************************************************
*/


extern long fastlog2 (long);
/* pre  : We are given a valid long integer >= 0L.
 * post : Returns the ceiling whole number value of the argument
 *	       to log base 2
 */



#ifdef DRIVER
/* **************************** PRESENTATION ******************************** */


extern void menu ( void );
/* pre  : None.
 * post : Responsible for the display of menu functions to stdout.
 */


extern void rowbreakdown (struct MPSstruct *, FILE *);
/* pre  : We are given a valid pointer to an MPS structure, and a valid
 *        pointer to FILE.
 * post : Prints all row-related data into the specified file from the
 *        MPS structure.
 */


extern void colbreakdown (struct MPSstruct *, FILE *);
/* pre  : We are given a valid pointer to an MPS structure, and a valid
 *        pointer to FILE.
 * post : Prints all row-related data into the specified file from the
 *        MPS structure.
 */


extern void histogram1 (struct MPSstruct *, FILE *);
/* pre  : We are given a valid pointer to an MPS structure, and a valid
 *	  pointer to FILE.
 * post : Prints a sidelong histogram detailing the absolute values
 *	  of elements stated in the MPS file into the specified file,
 *	  taking the data from the MPS structure.
 */


extern void histogram2 (struct MPSstruct *, FILE *);
/* pre  : We are given a valid pointer to an MPS structure, and a valid
 *	  pointer to FILE.
 * post : Prints a sidelong histogram detailing the breakdown of rows with 'x'
 *	  number of elements stated in the MPS file into the specified file,
 *	  taking the data from the MPS structure.
 */


extern void histogram3 (struct MPSstruct *, FILE *);
/* pre  : We are given a valid pointer to an MPS structure, and a valid
 *	  pointer to FILE.
 * post : Prints a sidelong histogram detailing the breakdown of columns with
 *	  'x' number of elements stated in the MPS file into the specified 
 *        file, taking the data from the MPS structure.
 */


extern void printRHSvec (struct MPSstruct *, FILE *);
/* pre  : We are given a valid pointer to an MPS structure, and a valid
 *	  pointer to a FILE.
 * post : Prints out in dense format the entire contents of the RHS vector
 *	  to the file from the MPS structure.
 */


extern void printRNGvec (struct MPSstruct *, FILE *);
/* pre  : We are given a valid pointer to an MPS structure, and a valid
 *	  pointer to a FILE.
 * post : Prints out in dense format the entire contents of the RANGES vector
 *	  to the file from the MPS structure.
 */


extern void printLBDvec (struct MPSstruct *, FILE *);
/* pre  : We are given a valid pointer to an MPS structure, and a valid
 *	  pointer to a FILE.
 * post : Prints out in dense format the entire contents of the LOWER BOUNDS
 *	  VECTOR to the file from the MPS structure.
 */


extern void printUBDvec (struct MPSstruct *, FILE *);
/* pre  : We are given a valid pointer to an MPS structure, and a valid
 *	  pointer to a FILE.
 * post : Prints out in dense format the entire contents of the UPPER BOUNDS
 *	  VECTOR to the file from the MPS structure.
 */



/* **************************** END PRESENTATION **************************** */
#endif /* DRIVER */



/*
 *******************************************************************************
 *                             LOCAL STRUCTURES                                *
 *******************************************************************************
*/


struct rdstruct {
   short valid;		/* flag to test if the structure "exists" */
   long  linenumber;	/* linenumber of the row                  */
   char  *rowname;	/* string name of the row                 */
   char  *value;	/* string ASCII value of the element      */
};



/*
 *******************************************************************************
 *                               IMPLEMENTATIONS                               *
 *******************************************************************************
*/



/* ************************************************************************** */
/* *                                FASTLOG2                                * */
/* ************************************************************************** */

long fastlog2 (long number)
/* pre  : We are given a valid long integer >= 0L.
 * post : Returns the ceiling whole number value of the argument
 *	  to log base 2
 */
{
   register unsigned long num = (unsigned long)number,
    			  temp;
   register long log2,     /* returns normal log2 value */
		 templog2; /* returns correct log2 value if we overrun */


   /* monitor number of bit-shifts needed to move temp to zero */
   for (log2 = 0L, 
         temp = num; 
          temp > 0UL; 
           log2++, 
            temp >>= 1UL);

   /* if the argument was a power of 2, XOR will give zero here */
   templog2 = log2 - 1L;
   if (((1UL << templog2) ^ num) == 0UL)
   {
      /* the argument was a power of two, so we overcalculated log2 */
      return (templog2);
   }/* end if */

   /* we did not overcalculate, so return log2 */
   return (log2);

}



/* ************************************************************************** */
/* *			         INITMPSSTRUCT				    * */
/* ************************************************************************** */

struct MPSstruct *InitMPSstruct (long numrow, long guesscol)
/* pre  : We are given two valid long integers.
 * post : Returns a pointer to a generated MPSstruct
 *	  whose row number corresponds to the first
 *	  argument, and whose column number initially
 *	  matches the second argument.
 */
{
   struct MPSstruct *tempstruct = NULL;
   char **chptrptr = NULL,
        *chptr     = NULL;
   double *dptr1 = NULL, 
          *dptr2 = NULL;
   register long numrows, 
	         guesscols, 
	         i;


   /*
    * strictly, a compiler should do this anyway, but for safety
    * set up local registers for speed
    */
   numrows   = numrow;
   guesscols = guesscol;


   if ((tempstruct = 
	 (struct MPSstruct *) malloc (sizeof(struct MPSstruct))) == NULL)
   {
      return (NULL);
   }/* end if */

   /* we can set the number of rows already */
   tempstruct->numberrows = numrows;
   if (((tempstruct->rownames) = 
	 (char **) malloc (numrows * sizeof (char *))) == NULL)
   {
      free (tempstruct);
      
      return (NULL);
   }/* end if */


   /* we use chptr and chptrptr to simplify assignment of string space */
   if ((chptr = 
	 (char *) malloc (numrows * MAX_ROWNAMESIZE * sizeof(char))) == NULL)
   {
      free (tempstruct->rownames);
      free (tempstruct);
      
      return (NULL);
   }/* end if */

   /* initialise the setting of the character arrays */
   for (i = 0L, 
	 chptrptr = tempstruct->rownames; 
	  i < numrows; 
	   i++,
	    chptrptr++, 
	     chptr += MAX_ROWNAMESIZE)
   {
      *chptrptr = chptr;
   }/* end for i */


   /* repeat the procedure for columns */
   if (((tempstruct->colnames) = 
	 (char **) malloc (guesscols * sizeof (char *))) == NULL)
   {
      free (*(tempstruct->rownames));
      free (tempstruct->rownames);
      free (tempstruct);
      
      return (NULL);
   }/* end if */

   /* once again, we use chptr and chptrptr for simple space assignment */
   if ((chptr = 
	 (char *) malloc (guesscols * MAX_COLNAMESIZE * sizeof(char))) == NULL)
   {
      free (tempstruct->colnames);
      free (*(tempstruct->rownames));
      free (tempstruct->rownames);
      free (tempstruct);
      
      return (NULL);
   }/* end if */

   /* initialise the setting of the character arrays */
   for (i = 0L, 
	 chptrptr = tempstruct->colnames; 
	  i < guesscols; 
	   i++,
	    chptrptr++, 
	     chptr += MAX_COLNAMESIZE)
   {
      *chptrptr = chptr;
   }/* end for i */


   /* get space for the relational codes vector for rows */
   if (((tempstruct->relationalcodesvector) = 
	 (char *) calloc ((numrows * sizeof(char)), sizeof(char))) == NULL)
   {
      free (*(tempstruct->colnames));
      free (tempstruct->colnames);
      free (*(tempstruct->rownames));
      free (tempstruct->rownames);
      free (tempstruct);
      
      return (NULL);
   }/* end if */

   
   /* get space for the RHS vector */
   if (((tempstruct->rhsvector) = 
	 (double *) malloc (numrows * sizeof(double))) == NULL)
   {
      free (tempstruct->relationalcodesvector);
      free (*(tempstruct->colnames));
      free (tempstruct->colnames);
      free (*(tempstruct->rownames));
      free (tempstruct->rownames);
      free (tempstruct);
      
      return (NULL);
   }/* end if */


   /* get space for the RANGES vector */		
   if (((tempstruct->rangesvector) = 
	 (double *) malloc (numrows * sizeof(double))) == NULL)
   {
      free (tempstruct->rhsvector);
      free (tempstruct->relationalcodesvector);
      free (*(tempstruct->colnames));
      free (tempstruct->colnames);
      free (*(tempstruct->rownames));
      free (tempstruct->rownames);
      free (tempstruct);
      
      return (NULL);
   }/* end if */


   /*
    * we know exactly how many rows we have, so initialise the
    * RHS and RANGES vector at this point, using one 'for' loop
    * to do as much useful work as possible
    */
   for (i = 0L,
	 dptr1 = tempstruct->rhsvector,
	  dptr2 = tempstruct->rangesvector;
	   i < numrows; 
	    i++, 
	     dptr1++, 
	      dptr2++)
   {
      *dptr1 = DEFAULTRHS;
      *dptr2 = DEFAULTRNG;
   }/* end for i */


   /* (over)allocate the upperbounds vector */
   if (((tempstruct->upperboundsvector) = 
	 (double *) malloc (guesscols * sizeof(double))) == NULL)
   {
      free (tempstruct->rangesvector);
      free (tempstruct->rhsvector);
      free (tempstruct->relationalcodesvector);
      free (*(tempstruct->colnames));
      free (tempstruct->colnames);
      free (*(tempstruct->rownames));
      free (tempstruct->rownames);
      free (tempstruct);
      
      return (NULL);
   }/* end if */


   /* (over)allocate the lowerbounds vector */
   if (((tempstruct->lowerboundsvector) = 
	 (double *) malloc (guesscols * sizeof(double))) == NULL)
   {
      free (tempstruct->upperboundsvector);
      free (tempstruct->rangesvector);
      free (tempstruct->rhsvector);
      free (tempstruct->relationalcodesvector);
      free (*(tempstruct->colnames));
      free (tempstruct->colnames);
      free (*(tempstruct->rownames));
      free (tempstruct->rownames);
      free (tempstruct);
      
      return (NULL);
   }/* end if */

   /* special initialisation of the UPPER and LOWER BOUNDS vectors */
   for (i = 0L, 
	 dptr1 = tempstruct->upperboundsvector,
          dptr2 = tempstruct->lowerboundsvector;
	   i < guesscols; 
	    i++, 
	     dptr1++, 
	      dptr2++)
   {
      *dptr1 = DEFAULTUBD;
      *dptr2 = DEFAULTLBD;
   }/* end for i */


   /*
    * cater for empty RHS, RANGES, and BOUNDS sections
    * by moving NULL into their respective strings
    */
   *(tempstruct->rhsname) = '\0';
   *(tempstruct->ranname) = '\0';
   *(tempstruct->bndname) = '\0';


   /* (over)allocate the number of structures to hold the data */
   if ((tempstruct->kerneldata_row = 
	 (struct rowdata *) malloc 
	  ((guesscols+guesscols) * sizeof(struct rowdata))) == NULL)
   {
      free (tempstruct->lowerboundsvector);
      free (tempstruct->upperboundsvector);
      free (tempstruct->rangesvector);
      free (tempstruct->rhsvector);
      free (tempstruct->relationalcodesvector);
      free (*(tempstruct->colnames));
      free (tempstruct->colnames);
      free (*(tempstruct->rownames));
      free (tempstruct->rownames);
      free (tempstruct);
      
      return (NULL);
   }/* end if */


   /* (over)allocate the number of column pointers to data */
   if ((tempstruct->kerneldata_col = 
	 (struct rowdata **) malloc
	  (guesscols * sizeof(struct rowdata))) == NULL)
   {
      free (tempstruct->kerneldata_row);
      free (tempstruct->lowerboundsvector);
      free (tempstruct->upperboundsvector);
      free (tempstruct->rangesvector);
      free (tempstruct->rhsvector);
      free (tempstruct->relationalcodesvector);
      free (*(tempstruct->colnames));
      free (tempstruct->colnames);
      free (*(tempstruct->rownames));
      free (tempstruct->rownames);
      free (tempstruct);
      
      return (NULL);
   }/* end if */


   return (tempstruct);

}



/* ************************************************************************** */
/* *			       DELETEMPSSTRUCT                              * */
/* ************************************************************************** */

void DeleteMPSstruct (struct MPSstruct **mystruct)
/* pre  : We are given a pointer to a pointer to a valid MPSstruct.
 * post : Safely removes all "malloc"-ed items in the MPSstruct prior
 *	  to deleting the structure itself.
 */
{
   /* free each "{m|c}alloc"-ed item in the structure */

   free ((*mystruct)->kerneldata_row);
   free ((*mystruct)->kerneldata_col);
   free ((*mystruct)->lowerboundsvector);
   free ((*mystruct)->upperboundsvector);
   free ((*mystruct)->rangesvector);
   free ((*mystruct)->rhsvector);
   free ((*mystruct)->relationalcodesvector);
   free (*((*mystruct)->colnames));
   free ((*mystruct)->colnames);
   free (*((*mystruct)->rownames));
   free ((*mystruct)->rownames);
   free (*mystruct);


   return;

}



/* ************************************************************************** */
/* *			        SETSECTIONPTRS                              * */
/* ************************************************************************** */

int SetSectionPtrs (char *memptr, 
                     char **namptr, 
                      char **rowptr,
		       char **colptr, 
		        char **rhsptr, 
			 char **ranptr,
			  char **bndptr, 
			   char **endptr, 
			    long *rcount,
			     long *ccount, 
			      unsigned long size)
/* pre  : We are given a pointer to an array of characters, 7 character
 *	  pointer pointers, 2 long integer pointers, and an unsigned long
 *	  value.
 * post : Assigns the character pointer pointers to each possible section
 *	  that can be found in an MPS file.  Since "RANGES" and "BOUNDS" are
 *	  optional MPS sections, their corresponding pointer pointers to char
 *	  are preset to NULL before being passed as arguments to this 
 *        function.
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
{
   register char thechar; /* holds a char value for immediate view    */
   char *tempptr  = NULL, /* general char pointer for array traversal */
	*limitptr = NULL; /* sentinel used in determining rhslinectr  */
   register unsigned long charsread, /* tally of characters read so far */
			  limit;     /* the file size acts as a limiter */

   /*
    * initialised to -1 to account for '\n' on line with "ROWS",
    * "COLUMNS", and "RHS"
    */
   register long rowctr     = -1L, 
	         colctr     = -1L, 
	         rhslinectr = -1L;


   /* safety setting to a register of file's size for speed */
   limit = size;


   /* this first check immediately isolates files too small to be MPS */
   if (size < MPSFILESIZECHECKLIMIT)
   {
      fprintf (stderr, 
	       "\nNot a MPS file, basic checks cannot be done\n\n");
      fflush (stderr);
      
      return (0);
   }/* end if (1) */

   /* NAME section is set to the start of the file */
   *namptr = memptr;

   /* prepare to scan the entire character array to set name section */	
   for(tempptr = memptr, 
        charsread = 0UL; 
	 charsread < limit;
	  charsread++, 
	   tempptr++)
   { 
      if (*tempptr == 'N')
      {
         if (charsread)
	 {
	    if (*(tempptr - 1) == '\n')
	    {
	       break;
	    }/* end if (3) */
	 }
	 else
	 {
	    /* if 'N' was seen at the very start of the file */
	    break;
	 }/* end if (2) */
	 
      }/* end if (1) */
       
   }/* end for charsread */

   /* we are at the end of the file, and we did not find "NAME" ? */
   if (charsread == limit)
   {
      fprintf (stderr, 
	       "\nNot a MPS file : NAME not given\n\n");
      fflush (stderr);
      
      return (0);
   }/* end if (1) */


   /*
    * Check that "NAME" is stated.  We have found "N" and are currently
    * at "N".  If adding 'JUMPNAMECHARS' to 'charsread' takes us over the
    * file size 'limit' then "NAME" is not given.  Otherwise, we use an
    * unrolled for loop to examine the 3 characters after "N" to verify
    * that they are "A", "M", and "E" respectively.
    */
   charsread += JUMPNAMECHARS;
   if (limit >= charsread)
   {
      /* since we are already at the 'N' */
      tempptr++;
      if (*tempptr != 'A')
      {
         fprintf (stderr,
	          "\nNot a MPS file : NAME not given\n\n");
	 fflush (stderr);
	 
	 return (0);
      }/* end if (2) */
      tempptr++;
	
      if (*tempptr != 'M')
      {
         fprintf (stderr,
	          "\nNot a MPS file : NAME not given\n\n");
	 fflush (stderr);
	 
	 return (0);
      }/* end if (2) */
      tempptr++;
	
      if (*tempptr != 'E')
      {
         fprintf (stderr,
	          "\nNot a MPS file : NAME not given\n\n");
	 fflush (stderr);
	 
	 return (0);
      }/* end if (2) */
      
   }
   else
   {
      fprintf (stderr, 
	       "\nNot a MPS file : NAME not given\n\n");
      fflush (stderr);
       
      return (0);
   }/* end if (1) */
   tempptr++;


   /* set the row pointer */
   for( ;
	 charsread < limit; 
	  charsread++, 
	   tempptr++)
   { 
      if ((*tempptr == 'R') && (*(tempptr-1) == '\n'))
      {
         *rowptr = tempptr;
	 
	 break;
      }/* end if (1) */
      
   }/* end for charsread */

   /* we are at the end of the file, and we did not find "ROWS" ? */
   if (charsread == limit)
   {
      fprintf (stderr,
	       "\nNot a MPS file : ROWS not given\n\n");
      fflush (stderr);
      
      return (0);
   }/* end if (1) */


   /*
    * Check that "ROWS" is stated.  We have found "\nR" and are currently
    * at "R".  If adding 'JUMPROWCHARS' to 'charsread' takes us over the
    * file size 'limit' then "ROWS" is not given.  Otherwise, we use an
    * unrolled for loop to examine the 3 characters after "R" to verify
    * that they are "O", "W", and "S" respectively.
    */
   charsread += JUMPROWCHARS;
   if (limit >= charsread)
   {
      /* since we are already at the 'R' */
      tempptr++;
      if (*tempptr != 'O')
      {
         fprintf (stderr,
	          "\nNot a MPS file : ROWS not given\n\n");
	 fflush (stderr);
	 
	 return (0);
      }/* end if (2) */
      tempptr++;
	
      if (*tempptr != 'W')
      {
         fprintf (stderr,
	          "\nNot a MPS file : ROWS not given\n\n");
	 fflush (stderr);
	 
	 return (0);
      }/* end if (2) */
      tempptr++;
	
      if (*tempptr != 'S')
      {
         fprintf (stderr,
	          "\nNot a MPS file : ROWS not given\n\n");
	 fflush (stderr);
	 
         return (0);
      }/* end if (2) */
      
   }
   else
   {
      fprintf (stderr, 
	       "\nNot a MPS file : ROWS not given\n\n");
      fflush (stderr);
      
      return (0);
   }/* end if (1) */
   tempptr++;


   /* set the column pointer */
   for( ;
	 charsread < limit;
	  charsread++, 
	   tempptr++)
   { 
      thechar = *tempptr;

      if (*(tempptr - 1) == '\n')
      {
         if (thechar == 'C')
         {
	    *colptr = tempptr;
	    rowctr++;
	    
	    break;
         }
         else if (thechar != '*')
         {
	    rowctr++;
         }/* end if (2) */

      }/* end if (1) */

   }/* end for charsread */

   /* if there are no rows then abort */
   if (! rowctr)
   {
      fprintf (stderr, 
	       "\nMPS file has no \"ROWS\" data\n\n");
      fflush (stderr);
      
      return (0);
   }/* end if (1) */

   /* are we at the end of the file ? */
   if (charsread == limit)
   {
      fprintf (stderr, 
	       "\nNot a MPS file : COLUMNS not given\n\n");
      fflush (stderr);
      
      return (0);
   }/* end if (1) */


   /*
    * Check that "COLUMNS" is stated.  We have found "\nC" and are currently
    * at "C".  If adding 'JUMPCOLCHARS' to 'charsread' takes us over the
    * file size 'limit' then "COLUMNS" is not given.  Otherwise, we use an
    * unrolled for loop to examine the 6 characters after "C" to verify
    * that they are "O", "L", "U", "M", "N", and "S" respectively.
    */
   charsread += JUMPCOLCHARS;
   if (limit >= charsread)
   {
      /* since we are already at the 'C' */
      tempptr++;
      if (*tempptr != 'O')
      {
         fprintf (stderr,
	          "\nNot a MPS file : COLUMNS not given\n\n");
	 fflush (stderr);
	 
         return (0);
      }/* end if (2) */
      tempptr++;
	
      if (*tempptr != 'L')
      {
         fprintf (stderr,
	          "\nNot a MPS file : COLUMNS not given\n\n");
	 fflush (stderr);
	 
	 return (0);
      }/* end if (2) */
      tempptr++;
	
      if (*tempptr != 'U')
      {
         fprintf (stderr,
	          "\nNot a MPS file : COLUMNS not given\n\n");
	 fflush (stderr);
	 
         return (0);
      }/* end if (2) */
      tempptr++;
	
      if (*tempptr != 'M')
      {
         fprintf (stderr,
	          "\nNot a MPS file : COLUMNS not given\n\n");
	 fflush (stderr);
	 
         return (0);
      }/* end if (2) */
      tempptr++;
	
      if (*tempptr != 'N')
      {
         fprintf (stderr,
	          "\nNot a MPS file : COLUMNS not given\n\n");
	 fflush (stderr);
	 
         return (0);
      }/* end if (2) */
      tempptr++;
	
      if (*tempptr != 'S')
      {
         fprintf (stderr,
	          "\nNot a MPS file : COLUMNS not given\n\n");
	 fflush (stderr);
	 
         return (0);
      }/* end if (2) */
      
   }
   else
   {
      fprintf (stderr,
	       "\nNot a MPS file : COLUMNS not given\n\n");
      fflush (stderr);
      
      return (0);
   }/* end if (1) */
   tempptr++;


   /* set the rhs pointer */
   for( ;
	 charsread < limit; 
	  charsread++, 
	   tempptr++)
   { 
      thechar  = *tempptr;

      if (*(tempptr - 1) == '\n')
      {
	 
         if (thechar == 'R')
         {
	    *rhsptr = tempptr;
	    colctr++;
	    
	    break;
         }
         else if (thechar != '*')
         {
	    colctr++;
         }/* end if (2) */

      }/* end if (1) */

   }/* end for charsread */

   /* if there are no columns then abort */
   if (! colctr)
   {
      fprintf (stderr, 
	       "\nMPS file has no \"COLUMNS\" data\n\n");
      fflush (stderr);
      
      return (0);
   }/* end if (1) */

   /* are we at the end of the file ? */
   if (charsread == limit)
   {
      fprintf (stderr, 
	       "\nNot a MPS file : RHS not given\n\n");
      fflush (stderr);
      
      return (0);
   }/* end if (1) */


   /* verify that we have a "RHS" section */
   charsread += JUMPRHSCHARS;
   if (limit >= charsread)
   {
      /* since we are already at 'R' */
      tempptr++;
	
      if (*tempptr != 'H')
      {
         fprintf (stderr,
	          "\nNot a MPS file : RHS not given\n\n");
	 fflush (stderr);
	 
         return (0);
      }/* end if (2) */
      tempptr++;
	
      if (*tempptr != 'S')
      {
         fprintf (stderr,
	          "\nNot a MPS file : RHS not given\n\n");
	 fflush (stderr);
	 
         return (0);
      }/* end if (2) */
      
   }	
   else
   {
      fprintf (stderr,
	       "\nNot a MPS file : RHS not given\n\n");
      fflush (stderr);
      
      return (0);
   }/* end if (1) */
   tempptr++;


   /*
    *  since ENDATA is required but RANGES and BOUNDS are not, set
    *  the end pointer first to act as sentinel
    */

   for( ;
	 charsread < limit; 
	  charsread++, 
	   tempptr++)
   { 
      if ((*tempptr == 'E') && (*(tempptr-1) == '\n'))
      {
         *endptr = tempptr;
	 
	 break;
      }/* end if (1) */
      
   }/* end for charsread */

   /* are we at the end of the file */
   if (charsread == limit)
   {
      fprintf (stderr, 
	       "\nNot a MPS file : ENDATA not given\n\n");
      fflush (stderr);
      
      return (0);
   }/* end if (1) */


   /* verify that "ENDATA" is specified */
   charsread += JUMPENDCHARS;
   if (limit >= charsread)
   {
      /* since we are at 'E' */
      tempptr++;
	
      if (*tempptr != 'N')
      {
         fprintf (stderr,
	          "\nNot a MPS file : ENDATA not given\n\n");
	 fflush (stderr);
	 
         return (0);
      }/* end if (2) */
      tempptr++;
	
      if (*tempptr != 'D')
      {
         fprintf (stderr,
	          "\nNot a MPS file : ENDATA not given\n\n");
	 fflush (stderr);
	 
         return (0);
      }/* end if (2) */
      tempptr++;
	
      if (*tempptr != 'A')
      {
         fprintf (stderr,
	          "\nNot a MPS file : ENDATA not given\n\n");
	 fflush (stderr);
	 
         return (0);
      }/* end if (2) */
      tempptr++;
	
      if (*tempptr != 'T')
      {
         fprintf (stderr,
	          "\nNot a MPS file : ENDATA not given\n\n");
	 fflush (stderr);
	 
         return (0);
      }/* end if (2) */
      tempptr++;
	
      if (*tempptr != 'A')
      {
         fprintf (stderr,
	          "\nNot a MPS file : ENDATA not given\n\n");
	 fflush (stderr);
	 
         return (0);
      }/* end if (2) */
      
   }
   else
   {
      fprintf (stderr, 
	       "\nNot a MPS file : ENDATA not given\n\n");
      fflush (stderr);
      
      return (0);
   }/* end if (1) */


   /* try to find RANGES, and isolate a warning if the RHS is empty */

   /* (R)HS and (R)ANGES, thus to avoid confusion move past the "R" */
   tempptr = (*rhsptr) + SKIPRHSCHARS;
   for (limitptr = *endptr; 
	 tempptr < limitptr; 
	  tempptr++)
   {
      thechar  = *tempptr;

      if (*(tempptr - 1) == '\n')
      {
	 
         if (thechar == 'R')
         {
	    *ranptr = tempptr;
	    rhslinectr++;
	     
	    break;
         }
	 else if (thechar != '*')
         {
	    rhslinectr++;
         }/* end if (2) */

      }/* end if (1) */
      
   }/* end for tempptr */


   /* produce the warning that the RHS is empty */
   if ( ! rhslinectr )
   {
      fprintf (stderr, 
	       "\nEmpty RHS section\n\n");
      fflush (stderr);
   }/* end if (1) */

   /* try to find bounds */
   if (*ranptr == NULL)
   {
      tempptr = *rhsptr;
      rhslinectr = -1L; /* reset the rhs line counter */
   }
   else
   {
      /* we have a RANGES section, verify the word, no bound check needed */

      /* we are at 'R' */
      tempptr++;

      if (*tempptr != 'A')
      {
         fprintf (stderr, 
	          "\nRANGES section has wrong section name\n\n");
	 fflush (stderr);
	 
	 return (0);
      }/* end if (2) */
      tempptr++;

      if (*tempptr != 'N')
      {
         fprintf (stderr, 
	          "\nRANGES section has wrong section name\n\n");
	 fflush (stderr);
	 
	 return (0);
      }/* end if (2) */
      tempptr++;

      if (*tempptr != 'G')
      {
         fprintf (stderr, 
	          "\nRANGES section has wrong section name\n\n");
	 fflush (stderr);
	 
	 return (0);
      }/* end if (2) */
      tempptr++;

      if (*tempptr != 'E')
      {
         fprintf (stderr, 
	          "\nRANGES section has wrong section name\n\n");
	 fflush (stderr);
	 
	 return (0);
      }/* end if (2) */
      tempptr++;

      if (*tempptr != 'S')
      {
         fprintf (stderr, 
	          "\nRANGES section has wrong section name\n\n");
	 fflush (stderr);
	 
	 return (0);
      }/* end if (2) */
      
   }/* end if (1) */
   tempptr++;


   while (tempptr < *endptr)
   {
      thechar = *tempptr;
      
      if (thechar == '\n')
      {
         rhslinectr++;
      }
      else if ((thechar == 'B') && (*(tempptr - 1) == '\n'))
      {
	 *bndptr = tempptr;
	 
	 break;
      }/* end if (1) */
      
      tempptr++;
      
   }/* end while tempptr */

   /* if we have a BOUNDS section, verify the name */

   if (*bndptr != NULL)
   {
      /* we are at 'B' already */
      tempptr++;

      if (*tempptr != 'O')
      {
         fprintf (stderr, 
	          "\nBOUNDS section has wrong section name\n\n");
	 fflush (stderr);
	 
	 return (0);
      }/* end if (2) */
      tempptr++;

      if (*tempptr != 'U')
      {
         fprintf (stderr, 
	          "\nBOUNDS section has wrong section name\n\n");
	 fflush (stderr);
	 
	 return (0);
      }/* end if (2) */
      tempptr++;

      if (*tempptr != 'N')
      {
         fprintf (stderr, 
	          "\nBOUNDS section has wrong section name\n\n");
	 fflush (stderr);
	 
	 return (0);
      }/* end if (2) */
      tempptr++;

      if (*tempptr != 'D')
      {
         fprintf (stderr, 
	          "\nBOUNDS section has wrong section name\n\n");
	 fflush (stderr);
	 
	 return (0);
      }/* end if (2) */
      tempptr++;

      if (*tempptr != 'S')
      {
         fprintf (stderr, 
	          "\nBOUNDS section has wrong section name\n\n");
	 fflush (stderr);
	 
	 return (0);
      }/* end if (2) */

   }/* end if (1) */


   /* produce the warning that the RHS is empty */
   if (( ! rhslinectr ) && (*ranptr == NULL))
   {
      fprintf (stderr, 
	       "\nEmpty RHS section\n\n");
      fflush (stderr);
   }/* end if (1) */


   /* give the empirical counts for rows and columns */
   *rcount = rowctr;
   *ccount = colctr;

   
   return (1);

}



/* ************************************************************************** */
/* *			           GETNAME                                  * */
/* ************************************************************************** */

int GetNAME (char **theMPSdata, 
              struct MPSstruct **mystruct, 
               char *end,
                long *linenum)
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
 *	  The array of characters is updated to create strings, through 
 *        adding NULL delimiters.
 *
 *	  Returns 1 on success, 0 on failure.
 */
{
   char *tempptr = NULL, 
        *holdptr = NULL;
   register char tempchar;
   register int xfersize = 1; /* must have space for a NULL in string */
   register long loclinenum;

#ifdef VERBOSE
fprintf (stderr, 
         "\nGetting name ...\n");
fflush (stderr);
#endif /* VERBOSE */

   loclinenum = *linenum;

   tempptr = *theMPSdata;

   /* check to see if "NAME" is at the very beginning */
   if (*tempptr == 'N')
   {
      tempptr += SKIPNAMECHARS;

      while (! isalnum(*tempptr))
      {
         tempptr++;
      }/* end while *tempptr */


      /* if no name was given, *(tempptr-1) equals "\n" */
      if (*(tempptr - 1) != '\n')
      {
         /* isolate the name and get the size of the string */
	 holdptr = tempptr;
	 while (*tempptr != '\n')
	 {
	    tempptr++;
	    xfersize++;
	 }/* end while *tempptr */
	 *tempptr = '\0';


         if (xfersize <= MAX_LPNAMESIZE)
         {
	    memcpy ((*mystruct)->lpname, holdptr, xfersize);
	 }
	 else
	 {
	    fprintf (stderr,
		     "\nLP name exceeds length of %d line #1\n\n",
		      MAX_LPNAMESIZE);
	    fflush (stderr);
	    
	    return (0);
	 }/* end if (3) */

      }
      else
      {
         /*
	  * we reflect an empty name by moving "" (ie NULL) into the
	  * space assigned in the MPSstruct
	  */
	 
         *((*mystruct)->lpname) = '\0';
	 
      }/* end if (2) */

      /* we have read one line, whether or not there is a name */
      loclinenum++;

      /* we can speed up the rest of the processing by just counting "\n" */
      for ( ;
	     tempptr < end; 
	      tempptr++)
      {
         if (*tempptr == '\n')
	 {
	    loclinenum++;
	 }/* end if (2) */
	 
      }/* end for tempptr */
      
   }
   else
   {
      for ( ;
	     tempptr < end; 
	      tempptr++)
      {
         tempchar = *tempptr;
	 if ((tempchar == 'N') && (*(tempptr - 1) == '\n'))
	 {
	    tempptr += SKIPNAMECHARS;

	    while (! isalnum(*tempptr))
	    {
	       tempptr++;
	    }/* end while *tempptr */


	    /* if no name was given, *(tempptr-1) equals "\n" */
	    if (*(tempptr - 1) != '\n')
	    {
               /* isolate name and get size of string */
	       holdptr = tempptr;
	       
	       while (*tempptr != '\n')
	       {
	          tempptr++;
		  xfersize++;
	       }/* end while *tempptr */
	       *tempptr = '\0';


	       if (xfersize <= MAX_LPNAMESIZE)
	       {
	          memcpy ((*mystruct)->lpname, holdptr, xfersize);
	       }
	       else
	       {
	          fprintf (stderr,
			   "\nLP name exceeds length of %d line #1\n\n",
			    MAX_LPNAMESIZE);
		  fflush (stderr);
		  
		  return (0);
	       }/* end if (4) */

	    }
	    else
	    {
	       /*
		* we reflect an empty name by moving "" (ie NULL) into
		* the space assigned in the MPSstruct
		*/
	       
	       *((*mystruct)->lpname) = '\0';
	       
	    }/* end if (3) */

	    /* we have read one line, but removed the "\n" */
	    loclinenum++;

         }
	 else if (tempchar == '\n')
	 {
	    loclinenum++;
	 }/* end if (2) */

      }/* end for tempptr */
	
   }/* end if (1) */

   /* reflect the changes to the line number */
   *linenum = loclinenum;

   
   return (1);

}



/* ************************************************************************** */
/* *                             GETROWS                                    * */
/* ************************************************************************** */

int GetROWS (char **theMPSfile, 
              struct MPSstruct **mystruct,
	       rowhashtable *therowhtable, 
	        char *end, 
		 long *linenum)
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
{
   char	*leadptr    = NULL, 
        *pursuitptr = NULL, 
        **dstptr    = NULL, 
        *typeptr    = NULL;
   register char rtype,    /* holds the row type */
		 tempchar; /* reduces dereferences of leadptr */
   register int	xfersize;
   register long rowid = 0L, 
	         loclinenum;

	
#ifdef VERBOSE
fprintf (stderr, 
         "\nGetting rows ...\n");
fflush (stderr);
#endif /* VERBOSE */

   /* Begin initialisations */

   /* set local line number tally to the one pointed to by 'linenum' */
   loclinenum = (*linenum) + 1L; /* platform independent */

   dstptr  = (*mystruct)->rownames;
   typeptr = (*mystruct)->relationalcodesvector;
   
   leadptr = (*theMPSfile) + SKIPROWSCHARS;

   /* preset the leadptr to point to first char in line of row data */
   while (! isgraph (*leadptr))
   {
      leadptr++;
   }/* end while *leadptr */

   /* End of initialisations, proceed with processing ROWS data */

   while (leadptr < end)
   {
      /* if we are not at a comment line, ie line starting with '*' */

      tempchar = *leadptr;
      if (tempchar != '*')
      {
         /* perform the check that the character type is allowed */
	 if (!(*(row_flags + tempchar)))
	 {
	    fprintf (stderr,
		     "\nGetROWS: line# %ld, invalid row type \"%c\"\n\n",
		      loclinenum, 
		       tempchar);
	    fflush (stderr);

	    return (0);
	 }/* end if (2) */

	 /* we have a valid row type, so we store it */
	 rtype = tempchar;

	 /* move along the leadptr */
	 leadptr++;

	 /* skip all space up to 'name' section */
	 while (!isalnum(*leadptr))
	 {
	    leadptr++;
	 }/* end while *leadptr */

	 /* pursuitptr will point to name of the row */
	 xfersize   = 1; /* space for NULL needed in string */
	 pursuitptr = leadptr;
	 while (! isspace (*leadptr))
	 {
	    leadptr++;
	    xfersize++;
	 }/* end while *leadptr */
	 *leadptr = '\0';
	 /* name is now a string */

	 /*
	  *  add the data to the hash table, but return if there
	  *  is a failure
	  */
	 if (!(addrowtotable(therowhtable, rowid, loclinenum, pursuitptr)))
	 {
	    return (0);
	 }/* end if (2) */
	 memcpy (*dstptr, pursuitptr, xfersize);
	 dstptr++;
	 *typeptr = rtype;
	 typeptr++;


#ifdef VERBOSE
printf ("GetROWS : added %s id %ld\n", 
        pursuitptr,
         rowid);
fflush (stdout);
#endif /* VERBOSE */

	 /* successful add, increment row counter */
	 rowid++;
      }
      else
      {
#ifdef VERBOSE
printf ("GetROWS : comment line at %ld\n",
        loclinenum);
fflush (stdout);
#endif /* VERBOSE */

         /* we are at a comment line which we shall skip */
	 while (*leadptr != '\n')
	 {
	    leadptr++;
	 }/* end while *leadptr */

      }/* end if (1) */

      /* whether a comment or a data line, we update the line number */
      loclinenum++; 

      /* move on to the next line */
      while (! isgraph (*leadptr))
      {
         leadptr++;
      }/* end while *leadptr */
       
      /*
       * now we either break the main while loop condition or we
       * are at the 'type' section of the next line of ROWS data
       * or the start of another comment line
       */

   }/* end while leadptr */
        
   /* alter the running total of lines read */
   *linenum = loclinenum;


   return (1);

}



/* ************************************************************************** */
/* *                             GETCOLUMNS                                 * */
/* ************************************************************************** */

int GetCOLUMNS (char **theMPSfile, 
                 struct MPSstruct **mystruct,
		  elhashtable *theelhashtable, 
                   colhashtable *thecolhashtable,
		    rowhashtable *therowhashtable, 
		     char *end, 
		      long *linenum)
/* pre  : We are given a pointer to a pointer to an array of characters, a
 *        pointer to a pointer to a valid MPSstruct, a pointer to a valid
 *        element hash table, a pointer to a valid column hash table, a
 *        pointer to a valid row hash table, a character pointer, and a
 *        pointer to a long integer.
 * post : The scanned array of characters stretches from the first argument
 *        to the sentinel limit held in the sixth argument; this is the
 *        "COLUMNS" section of the MPS input file.  The element hash table and
 *        the column hash table are updated with element and column data,
 *        helped by referencing row data held in the unchanging rowhashtable.
 *        The MPSstruct is updated with the column names and the kernel data
 *        and then a truncation operation is performed to shrink the column-
 *        related arrays since the number of columns and elements is
 *        overspecified.  The long integer is updated at the end to reflect
 *        the number of lines read.
 *
 *        The function checks for the following: duplicate column names,
 *        duplicate elements in the same section, elements defined in
 *        non-contiguous column sections, elements defined with an incorrect
 *        rowname or a name that does not exist.
 *	  
 *        If the 'atof' function should give a value of a double as '0.0',
 *        then, up to a limit, a warning is issued, but processing continues.
 *
 *        This is the only location for the sort function, used to sort the
 *        element data of a column rowwise before entry into the kernel.  The
 *        kernel is sealed by a special element item of row id -1 and value
 *        0.0 and the last colptr item points to this seal.  Columns can be
 *        addressed via their location in the "colptrs" array of the 
 *        MPSstruct.
 *
 *        The array of characters is updated to create strings, through adding
 *        NULL delimiters.
 *
 *	  Returns 1 on success, 0 on failure.
 */
{
   char *leadptr     = NULL, /* scans chars between start and end      */
        *pursuitptr1 = NULL, /* will be used to extract column names   */
        *pursuitptr2 = NULL, /* will be used to get row names          */
        *pursuitptr3 = NULL, /* will be used to get values as strings  */
        *oldcol      = NULL, /* the column name before the current one */
        *chptr1      = NULL, /* for string comparisons                 */
        *chptr2      = NULL; /* for string comparisons                 */
   register short samestring     = 1, /* checks for same column name  */
                  furtherstrings = 0, /* flag for first column name   */
                  newlinefound;       /* flag for 1 data item in line */
   register long i, 
	         j, 
	         k,          /* general counters              */
		 colid = 0L, /* the ids assigned per column   */
		 limitr,     /* the maximum number of rows    */
		 rowid,      /* the extracted row id          */
		 loclinenum, /* tally of the line number      */
		 tally = 0L; /* elements intaken for a column */
   struct rdstruct *holder = NULL, /* the array of rdstruct          */
		   *hitrow = NULL; /* rdstruct pointer in data purge */
   long *rowrecord = NULL, /* holds the row ids as they occur          */
	*currrec   = NULL; /* current rowrecord location to read/write */

   /* for the MPSstruct */
   register int	xfersize    = 1, /* size of old col string + NULL    */
		newxfersize = 1; /* size of new col string + NULL    */
   char **dstptr = NULL; /* pointer to array of column names */
   struct rowdata *rdptr  = NULL, /* pointer to scan kerneldata_rows */
		  **cdptr = NULL; /* pointer to scan kerneldata_cols */
   register unsigned long elnumber,       /* the number of elements */
		          warnings   = 0UL, /* tally of warnings      */
			  numzeroels = 0UL; /* tally of zero-elements */
#ifdef VERBOSE
   register long z; /* simple counter */
#endif /* VERBOSE */


#ifdef VERBOSE
fprintf (stderr,
         "\nGetting columns, and elements ...\n");
fflush (stderr);
#endif /* VERBOSE */

   /* *** INITIALISATIONS *** */

   /* set local line number tally to the one pointed to by 'linenum' */
   loclinenum = (*linenum) + 1L; /* platform independent */
	
   /* the number of rows is often needed */
   limitr = (*mystruct)->numberrows;

   /* start at the first string for column names */
   dstptr = (*mystruct)->colnames;

   /* get a rowdata pointer set at the first kerneldata_row item */
   rdptr = (*mystruct)->kerneldata_row;

   /* set tha first pointer of the kerneldata_cols */
   cdptr = (*mystruct)->kerneldata_col;

   /* make the above pointer point to the first element of the row data */
   *cdptr = rdptr;

   /* set up space for the history of rows read */
   if ((rowrecord = (long *) malloc (limitr * sizeof(long))) == NULL)
   {
      fprintf (stderr, 
	       "\nUnable to allocate rowrecord space\n\n");
      fflush (stderr);

      return (0);
   }/* end if (1) */

   /* set the start record */
   currrec = rowrecord;


   /* get the rdstruct array, ready to hold row data  */
   if ((holder = 
        (struct rdstruct *) malloc (sizeof(struct rdstruct) * limitr)) == NULL)
   {
      fprintf (stderr,
	       "\nUnable to get array of \"rdstruct\" of size %ld\n\n",
	        limitr);
      fflush (stderr);
      free (rowrecord);

      return (0);
   }/* end if (1) */


   /*
    *  since the "valid" flag states the location is used if set to 1,
    *  we shall preset all these critical flags to zero
    */
   for (i = 0L, 
	 hitrow = holder; 
	  i < limitr; 
	   i++, 
	    hitrow++)
   {
      hitrow->valid = 0;
   }/* end for i */


   /* preset the leadptr to point to the start of the column data */
   leadptr = (*theMPSfile) + SKIPCOLSCHARS;
   while (! isgraph (*leadptr))
   {
      leadptr++;
   }/* end while *leadptr */


   /* *** END INITIALISATIONS *** */

#ifdef VERBOSE
printf ("GetCOLUMNS : INIT SUCCESSFUL\n");
fflush (stdout);
#endif /* VERBOSE */

   /*
    * we will have at least one execution as SetSectionPtrs
    * traps the case of an empty COLUMNS section
    */
   while (leadptr < end)
   {
      if (*leadptr != '*')
      {
         newlinefound = 0;

	 /* set the column name pointer, make name a string */
	 pursuitptr1 = leadptr;
	 newxfersize = 1; /* must have space for a NULL in a string */
	 while (!isspace(*leadptr))
	 {
	    leadptr++;
	    newxfersize++;
	 }/* end while *leadptr */
	 *leadptr = '\0';
	 leadptr++;
	    
	 /* if we have over one column name already */
	 if (furtherstrings)
	 {
	    samestring = 1;

	    /* check to see if we have a new column name */
	    chptr1 = oldcol;
	    chptr2 = pursuitptr1;
	    while ((*chptr1 != '\0') || (*chptr2 != '\0'))
	    {
	       if (*chptr1 != *chptr2)
	       {
	          samestring = 0;
		  
		  break;
	       }/* end if (3) */
	       chptr1++;
	       chptr2++;
	    }/* end while *chptr1 || *chptr2 */

#ifdef VERBOSE
printf ("GetCOLUMNS : furtherstrings = 1, samestring = %d\n", 
        samestring);
fflush (stdout);
#endif /* VERBOSE */

	 }
	 else
	 {
	    /* we will now perform strcmp on further strings */
	    furtherstrings = 1;

	    /* update the name of the column */
	    oldcol = pursuitptr1;

#ifdef VERBOSE
printf ("GetCOLUMNS : FIRST COLUMNNAME = %s, cid = %ld\n", 
        oldcol, 
         colid);
fflush (stdout);
#endif /* VERBOSE */

	 }/* end if (2) {furtherstrings} */
	    

	 /* we have a new column name */
	 if (!samestring)
	 {
#ifdef VERBOSE
printf ("\nPREPURGE CONTENTS OF RDSTRUCT ARRAY:\n");
for (z = 0; 
      z < limitr; 
       z++)
{
   if ((holder+z)->valid != 0)
   {
      printf ("[%ld] = line(%ld), rn(%s), v(%s)\n", z,
              ((holder+z))->linenumber,
               ((holder+z))->rowname,
                ((holder+z))->value);
      fflush (stdout);
   }/* end if (3) */

}/* end for */
printf ("\n");
fflush (stdout);
#endif /* VERBOSE */


            /*
             * Here the program makes a choice : 
             *
             *   if (tally == 1)
             *      purge one item
             *   if (tally == 2)
             *      purge two items in order
             *   else
             *      if (tally is in limits of the nlogn lookup table)
             *         if maxrows > (tally)log2(tally)
             *            sort the rowrecord
             *            purge the rowrecord
             *         else
             *            sequential purge of rowrecord
             *      else calculate (tally)log2(tally)
             *         if this is less than maxrows
             *            sort the rowrecord
             *            purge the rowrecord
             *         else
             *            sequential purge of rowrecord
             *
             */
            /* check for trivial values of tally */
            if (tally == 1L)
            {
#ifdef VERBOSE
printf ("PURGE : tally = 1\n");
fflush (stdout);
#endif /* VERBOSE */

               i      = *rowrecord;
               hitrow = (holder + i);
               if ((rdptr->value = atof (hitrow->value)) != 0.0L)
               {
                  if (! (addeltotable(theelhashtable,
                          hitrow->linenumber, 
                           oldcol,
                            hitrow->rowname)))
                  {
			free (rowrecord);
			free (holder);

			return (0);
		  }/* end if (5) */

		  rdptr->rowid = i;
		  rdptr++;

#ifdef VERBOSE
printf ("GetCOLUMNS : PURGE cid = %ld, rid = %ld, cn = %s, rn = %s, v = %s\n",
        colid, 
         i, 
          oldcol, 
           hitrow->rowname,
            hitrow->value);
fflush (stdout);
#endif /* VERBOSE */

               }
               else
	       {
                  warnings++;
                  if (warnings <= MAXCOLWARNINGS)
                  {
                     /* mention the problem */
                     fprintf (stderr,
		      "COLUMNS Warning (line %ld) value given = %s, not set\n",
		       hitrow->linenumber, 
			hitrow->value);
		     fflush (stderr);
                  }/* end if (5) */

		  numzeroels++; /* no move from zero-valued item */
		  
               }/* end if (4) {!= 0.0L} */

               tally         = 0L; /* reset the tally value for the next run */
               hitrow->valid = 0;  /* reinit the storage */

            }
            else if (tally == 2L)
            {

#ifdef VERBOSE
printf ("PURGE : tally = 2\n");
fflush (stdout);
#endif /* VERBOSE */

               i = *rowrecord;
               j = *(rowrecord+1);

               if (i < j)
               {
                  hitrow = (holder + i);
                  if ((rdptr->value = atof (hitrow->value)) != 0.0L)
                  {
                     if (! (addeltotable(theelhashtable,
                             hitrow->linenumber, 
                              oldcol,
                               hitrow->rowname)))
                     {
                        free (rowrecord);
                        free (holder);

                        return (0);
                     }/* end if (6) */

                     rdptr->rowid = i;
                     rdptr++;

#ifdef VERBOSE
printf ("GetCOLUMNS : PURGE cid = %ld, rid = %ld, cn = %s, rn = %s, v = %s\n",
        colid, 
         i, 
          oldcol, 
           hitrow->rowname,
            hitrow->value);
fflush (stdout);
#endif /* VERBOSE */
                  }
                  else 
                  {
                     warnings++;
                     if (warnings <= MAXCOLWARNINGS)
                     {
                        /* mention the problem */
                        fprintf (stderr,
                         "COLUMNS Warning (line %ld) value given = %s, ",
		          hitrow->linenumber, hitrow->value);
			fprintf (stderr, "not set\n");
			fflush (stderr);
                     }/* end if (6) */

                     numzeroels++; /* no move from zero-valued item */
		     
                  }/* end if (5) */

                  hitrow->valid = 0; /* reinit the storage */

                  hitrow = (holder + j);
                  if ((rdptr->value = atof (hitrow->value)) != 0.0L)
                  {
                     if (! (addeltotable(theelhashtable,
                             hitrow->linenumber, 
                              oldcol,
                               hitrow->rowname)))
                     {
                        free (rowrecord);
                        free (holder);

			return (0);
                     }/* end if (6) */

                     rdptr->rowid = j;
                     rdptr++;

#ifdef VERBOSE
printf ("GetCOLUMNS : PURGE cid = %ld, rid = %ld, cn = %s, rn = %s, v = %s\n",
        colid, 
         j, 
          oldcol, 
           hitrow->rowname,
            hitrow->value);
fflush (stdout);
#endif /* VERBOSE */

                  }
                  else
                  {
                     warnings++;
                     if (warnings <= MAXCOLWARNINGS)
                     {
                        /* mention the problem */
                        fprintf (stderr,
                         "COLUMNS Warning (line %ld) value given = %s, ",
                          hitrow->linenumber, hitrow->value);
			fprintf (stderr, "not set\n");
			fflush (stderr);
                     }/* end if (6) */

                     numzeroels++; /* no move from zero-valued item */
		     
                  }/* end if (5) */

                  tally         = 0L; /* reset the tally    */
                  hitrow->valid = 0;  /* reinit the storage */

               }
               else /* j < i */
               {
                  hitrow = (holder + j);
                  if ((rdptr->value = atof (hitrow->value)) != 0.0L)
                  {
                     if (! (addeltotable(theelhashtable,
                             hitrow->linenumber, 
                              oldcol,
                               hitrow->rowname)))
                     {
                        free (rowrecord);
                        free (holder);

                        return (0);
                     }/* end if (6) */

                     rdptr->rowid = j;
                     rdptr++;

#ifdef VERBOSE
printf ("GetCOLUMNS : PURGE cid = %ld, rid = %ld, cn = %s, rn = %s, v = %s\n",
        colid, 
         j, 
          oldcol, 
           hitrow->rowname,
            hitrow->value);
fflush (stdout);
#endif /* VERBOSE */

                  }
                  else 
                  {
                     warnings++;
                     if (warnings <= MAXCOLWARNINGS)
                     {
                        fprintf (stderr,
                         "COLUMNS Warning (line %ld) value given = %s, ",
		          hitrow->linenumber, hitrow->value);
			fprintf (stderr, "not set\n");
			fflush (stderr);
                     }/* end if (6) */

                     numzeroels++; /* no move from zero-valued item */
		     
                  }/* end if (5) */

                  hitrow->valid = 0; /* reinit the storage */

                  hitrow = (holder + i);
                  if ((rdptr->value = atof (hitrow->value)) != 0.0L)
                  {
                     if (! (addeltotable(theelhashtable,
                             hitrow->linenumber, 
                              oldcol,
                               hitrow->rowname)))
                     {
                        free (rowrecord);
                        free (holder);

                        return (0);
                     }/* end if (6) */

                     rdptr->rowid = i;
                     rdptr++;

#ifdef VERBOSE
printf ("GetCOLUMNS : PURGE cid = %ld, rid = %ld, cn = %s, rn = %s, v = %s\n",
        colid, 
         i, 
          oldcol, 
           hitrow->rowname,
            hitrow->value);
fflush (stdout);
#endif /* VERBOSE */
                  }
                  else
                  {
                     warnings++;
                     if (warnings <= MAXCOLWARNINGS)
                     {
                        /* mention the problem */
                        fprintf (stderr,
                         "COLUMNS Warning (line %ld) value given = %s, ",
                          hitrow->linenumber, hitrow->value);
			fprintf (stderr, "not set\n");
			fflush (stderr);
                     }/* end if (6) */

                     numzeroels++; /* no move from zero-valued item */
		     
                  }/* end if (5) */

                  tally         = 0L; /* reset the tally    */
                  hitrow->valid = 0;  /* reinit the storage */

               }/* end if (4) */

            }/* ... if (3) {tally == 2L} */
            else /* we may need to sort ! */
            {

               if (tally < LOOKUPTABLESIZE)
               {
                  if (*(nlogntable + tally) < limitr)
                  {
                     /* check for presorted array, sort if not the case */
                     for (i = 1L, 
			   j = *rowrecord, 
			    currrec = (rowrecord + 1);
                             i < tally; 
			      i++, 
                               currrec++, 
			        j = k)
                     {
                        k = *currrec;
                        if (j > k)
                        {
#ifdef VERBOSE
printf ("\nPURGE : sort start = \n");
for (z = 0; z < tally; z++)
{
   printf ("rowrecord[%ld] = %ld\n", 
	   z, 
	    rowrecord[z]);
   fflush (stdout);
}
printf ("\n");
fflush (stdout);
#endif /* VERBOSE */

                           sort (&rowrecord, tally);

#ifdef VERBOSE
printf ("\nPURGE : sort end = \n");
for (z = 0; z < tally; z++)
{
   printf ("rowrecord[%ld] = %ld\n", 
	   z, 
	    rowrecord[z]);
   fflush (stdout);
}
printf ("\n");
fflush (stdout);
#endif /* VERBOSE */
                           break;
					
                        }/* end if (6) */

                     }/* end for i */


                     /*
                      * use tally as the loop counter, since the time
                      * efficiency is better than if we used j and at the
                      * end of the for loop we reset the tally to 0L
                      */
                     for (currrec = rowrecord; 
			   tally > 0L; 
			    tally--, 
			     currrec++)
                     {
                        i      = *currrec;
                        hitrow = (holder + i);
                        if ((rdptr->value = atof (hitrow->value)) != 0.0L)
                        {
                           if (! (addeltotable(theelhashtable,
                                   hitrow->linenumber, 
                                    oldcol,
                                     hitrow->rowname)))
                           {
			      free (rowrecord);
                              free (holder);

                              return (0);
                           }/* end if (7) */

                           rdptr->rowid = i;
                           rdptr++;

#ifdef VERBOSE
printf ("GetCOLUMNS : PURGE cid = %ld, rid = %ld, cn = %s, rn = %s, v = %s\n",
        colid, 
         i, 
          oldcol, 
           hitrow->rowname,
            hitrow->value);
fflush (stdout);
#endif /* VERBOSE */

                        }
                        else
                        {
                           warnings++;
                           if (warnings <= MAXCOLWARNINGS)
                           {
                              /* mention the problem */
                              fprintf (stderr,
                               "COLUMNS Warning (line %ld) value given = %s, ",
                                hitrow->linenumber, hitrow->value);
			      fprintf (stderr, "not set\n");
			      fflush (stderr);
                           }/* end if (7)*/

                           numzeroels++; /* no move from zero-valued item */
			   
                        }/* end if (6) */

                        hitrow->valid = 0; /* reinit the storage */

                     }/* end for currrec */
		     
                  }
                  else
                  {
                     /* SEQUENTIAL PURGE */

                     /* we must assign all previous data to the MPSstruct */
                     for (i = 0L, 
			   hitrow = holder; 
			    tally > 0L;
                             i++, 
			      hitrow++) 
                     {
                        if (hitrow->valid != 0)
                        {
                           /* expensive but necessary assignment */
                           if ((rdptr->value = atof (hitrow->value)) != 0.0L)
                           {

                              if (! (addeltotable(theelhashtable,
                                      hitrow->linenumber, 
                                       oldcol,
                                        hitrow->rowname)))
                              {
                                 free (rowrecord);
                                 free (holder);

                                 return (0);
                              }/* end if (8) */

                              rdptr->rowid = i;
                              rdptr++;

#ifdef VERBOSE
printf ("GetCOLUMNS : S_PURGE cid = %ld, rid = %ld, cn = %s, rn = %s, v = %s\n",
        colid, 
         i, 
          oldcol, 
           hitrow->rowname,
            hitrow->value);
fflush (stdout);
#endif /* VERBOSE */
                           }
                           else
                           {
                              warnings++;
                              if (warnings <= MAXCOLWARNINGS)
                              {
                                 /* mention the problem */
                                 fprintf (stderr,
                                  "COLUMNS Warning (line %ld) value given = %s",
                                   hitrow->linenumber, hitrow->value);
				 fprintf (stderr, ", not set\n");
				 fflush (stderr);
                              }/* end if (8) */

                              numzeroels++; /* no move from zero item */
			      
                           }/* end if (7) */

                           hitrow->valid = 0; /* reinit the storage */
                           tally--; /* help terminate the for loop */

                        }/* end if (6) {hitrow != NULL} */

                     }/* end for currrec */

                  }/* end if (5) */

               }
               else /* we must calculate nlogn  */
               {
                  if (limitr > (tally * fastlog2(tally)))
                  {
                     /* check for presorted array, sort if not the case */
                     for (i = 1L, 
			   j = *rowrecord, 
			    currrec = (rowrecord + 1);
                             i < tally; 
			      i++, 
                               currrec++, 
                                j = k)
                     {
                        k = *currrec;
                        if (j > k)
                        {
#ifdef VERBOSE
printf ("\nPURGE : sort start = \n");
for (z = 0; z < tally; z++)
{
   printf ("rowrecord[%ld] = %ld\n", 
	   z, 
	    rowrecord[z]);
   fflush (stdout);
}
printf ("\n");
fflush (stdout);
#endif /* VERBOSE */

                           sort (&rowrecord, tally);

#ifdef VERBOSE
printf ("\nPURGE : sort end = \n");
for (z = 0; z < tally; z++)
{
   printf ("rowrecord[%ld] = %ld\n",
	   z, 
	    rowrecord[z]);
   fflush (stdout);
}
printf ("\n");
fflush (stdout);
#endif /* VERBOSE */
                           break;
			   
                        }/* end if (6) */

                     }/* end for i */


                     /*
                      * use tally as the loop counter, since the time
                      * efficiency is better than if we used j and at the
                      * end of the for loop we reset the tally to 0L
                      */
                     for(currrec = rowrecord;
			  tally > 0L; 
			   tally--, 
			    currrec++)
                     {
                        i = *currrec;
                        hitrow = (holder + i);
                        if ((rdptr->value = atof (hitrow->value)) != 0.0L)
                        {
                           if (! (addeltotable(theelhashtable,
                                   hitrow->linenumber, 
                                    oldcol,
                                     hitrow->rowname)))
                           {
                              free (rowrecord);
                              free (holder);

                              return (0);
                           }/* end if (7) */

                           rdptr->rowid = i;
                           rdptr++;

#ifdef VERBOSE
printf ("GetCOLUMNS : PURGE cid = %ld, rid = %ld, cn = %s, rn = %s, v = %s\n",
        colid,
         i,
          oldcol,
           hitrow->rowname,
            hitrow->value);
fflush (stdout);
#endif /* VERBOSE */
                        }
                        else
                        {
                           warnings++;
                           if (warnings <= MAXCOLWARNINGS)
                           {
                              /* mention the problem */
                              fprintf (stderr,
                               "COLUMNS Warning (line %ld) value given = %s",
                                hitrow->linenumber, hitrow->value);
			      fprintf (stderr, ", not set\n");
			      fflush (stderr);
		           }/* end if (7) */

                           numzeroels++; /* no move from zero-valued item */
			   
                        }/* end if (6) */

                        hitrow->valid = 0; /* reinit the storage */

                     }/* end for currrec */
		     
                  }
                  else
                  {
                     /* SEQUENTIAL PURGE */

                     /* we must assign all previous data to the MPSstruct */
                     for (i = 0L,
			   hitrow = holder;
			    tally > 0L;
                             i++,
			      hitrow++) 
                     {
                        if (hitrow->valid != 0)
                        {
                           /* expensive but necessary assignment */
                           if ((rdptr->value = atof (hitrow->value)) != 0.0L)
                           {

                              if (! (addeltotable(theelhashtable,
                                      hitrow->linenumber, 
                                       oldcol,
                                        hitrow->rowname)))
                              {
                                 free (rowrecord);
                                 free (holder);

                                 return (0);
                              }/* end if (8) */

                              rdptr->rowid = i;
                              rdptr++;

#ifdef VERBOSE
printf ("GetCOLUMNS : S_PURGE cid = %ld, rid = %ld, cn = %s, rn = %s, v = %s\n",
        colid,
         i,
          oldcol,
           hitrow->rowname,
            hitrow->value);
fflush (stdout);
#endif /* VERBOSE */

                           }
                           else
                           {
                              warnings++;
                              if (warnings <= MAXCOLWARNINGS)
                              {
                                 /* mention the problem */
                                 fprintf (stderr,
                                  "COLUMNS Warning (line %ld) value given = %s",
                                   hitrow->linenumber, hitrow->value);
				 fprintf (stderr, ", not set\n");
				 fflush (stderr);
                              }/* end if (8) */

                              numzeroels++; /* no move from zero item */
			      
                           }/* end if (7) */

                           hitrow->valid = 0; /* reinit the storage          */
                           tally--;           /* help terminate the for loop */

                        }/* end if (6) {hitrow != NULL} */

                     }/* end for i */

                  }/* end if (5) */

               }/* end if (4) */

            }/* end if (3) */


            /* add the old column to the column hash table */
	    
#ifdef VERBOSE
printf ("GetCOLUMNS : ADD COL cid = %ld, line = %ld, cn = %s\n",
        colid,
         loclinenum,
          oldcol);
fflush (stdout);
#endif /* VERBOSE */

            if (! (addcoltotable(thecolhashtable, colid, loclinenum, oldcol)))
            {
               /*
                * column about to be defined in 2 non-contiguous blocks
                */
               free (rowrecord);
               free (holder);

               return (0);
            }/* end if (3) */	


            /* update the strings, and pointers for the name */
            memcpy (*dstptr, oldcol, xfersize);
            dstptr++;

            /* reflect change to the kernel data status */
            cdptr++;
            *cdptr = rdptr;

            oldcol = pursuitptr1;

#ifdef VERBOSE
printf ("GetCOLUMNS : COLID INCREMENT FROM colid = %ld\n", 
        colid);
fflush (stdout);
#endif /* VERBOSE */

            colid++;

            /* prepare for a new row record */
            currrec = rowrecord;

         }/* end if (2) { ! samestring } */


         /* skip space up to (1st) rowname */
         while (!isalnum(*leadptr))
         {
            leadptr++;
         }/* end while *leadptr */


         /* about to take in row name */
         pursuitptr2 = leadptr;
         while (!(isspace(*leadptr)))
         {
            leadptr++;
         }/* end while *leadptr */
         *leadptr = '\0';
         leadptr++;


         /* check that pursuitptr2 is a valid row ! */
         if ((rowid = findrow(therowhashtable, pursuitptr2)) == BADROW)
         {
            fprintf (stderr,
		     "Row name \"%s\" at %ld does not exist\n\n",
                      pursuitptr2,
		       loclinenum);
	    fflush (stderr);
            free (rowrecord);
            free (holder);

            return (0);
         }/* end if (2) */


         /* skip the space up to the value, but pick up '-' */
         while (isspace(*leadptr))
         {
            leadptr++;
         }/* end while *leadptr */

         /* we have the id of the valid row, get the value as a string */
         pursuitptr3 = leadptr;
         while (! (isspace(*leadptr)))
         {
            leadptr++;
         }/* end while *leadptr */
	 
         /* safety code to prevent accidental overwrite of newlines */
         if (*leadptr == '\n')
         {
            newlinefound = 1;
         }/* end if (2) */
         *leadptr = '\0';
         leadptr++;

         /* allocate the first of 2 potential elements on sorted array */
         hitrow = (holder + rowid);

         /* 
          *  we must perform a special test to see if we have a duplicate row
          */
         if (! hitrow->valid)
         {
            hitrow->valid = 1;
         }
         else
         {
            /* we have a duplicate element situation */
            fprintf (stderr, 
                     "Element (\"%s\", \"%s\") at line #%ld\n",
                      pursuitptr1,
		       hitrow->rowname,
		        hitrow->linenumber);
            fprintf (stderr,
                     "Duplicate element (\"%s\", \"%s\") at line #%ld\n",
                      pursuitptr1,
		       pursuitptr2,
		        loclinenum);
	    fflush (stderr);

            free (rowrecord);
            free (holder);

            return (0);

         }/* end if (2) */

         hitrow->linenumber = loclinenum;
         hitrow->rowname    = pursuitptr2;
         hitrow->value      = pursuitptr3;
         tally++;

         /* adjust the local record of rows we have read */
         *currrec = rowid;
         currrec++;

#ifdef VERBOSE
printf ("HIT: line = %ld, rn = %s. rid = %ld, v = %s, tally = %ld\n",
        hitrow->linenumber,
         hitrow->rowname,
          rowid,
           hitrow->value,
            tally);
fflush (stdout);
#endif /* VERBOSE */

         /* OK so far, find new line character, move to next data item */
         while (!isgraph(*leadptr))
         {
            if (*leadptr == '\n')
            {
               newlinefound = 1;
            }/* end if (2) */
            leadptr++;

         }/* end while *leadptr */


         /* we are at the start of the name of the second row */
         if (!newlinefound)
         {
            pursuitptr2 = leadptr;
            while (!(isspace(*leadptr)))
            {
               leadptr++;
            }/* end while *leadptr */
            *leadptr = '\0';
            leadptr++;

            /* check that pursuitptr2 is a valid row ! */
            if ((rowid = findrow(therowhashtable, pursuitptr2)) == BADROW)
            {
               fprintf (stderr,
		        "Row name \"%s\" at %ld does not exist\n\n",
                         pursuitptr2,
		          loclinenum);
	       fflush (stderr);
	       
               free (rowrecord);
               free (holder);

               return (0);
            }/* end if (3) */

            /* skip the space up to the value, but pick up '-' */
            while (isspace(*leadptr))
            {
               leadptr++;
            }/* end while *leadptr */

            /* we have the id of the valid row, get the value as a string */
            pursuitptr3 = leadptr;
            while (! (isspace(*leadptr)))
            {
               leadptr++;
            }/* end while *leadptr */
	    
            *leadptr = '\0';
            leadptr++;


            /* allocate the 2nd of 2 potential elements on sorted array */
            hitrow = (holder + rowid);

            /* 
             *  we must perform a special test to see if we have a duplicate
             *  row
             */
            if (! hitrow->valid)
            {
               hitrow->valid = 1;
            }
            else
            {
               /* we have a duplicate element situation */
               fprintf (stderr, 
                         "Element (\"%s\", \"%s\") at line #%ld\n",
                          pursuitptr1,
		           hitrow->rowname,
                            hitrow->linenumber);
               fprintf (stderr,
                         "Duplicate element (\"%s\", \"%s\") at line #%ld\n",
                          pursuitptr1,
		           pursuitptr2,
		            loclinenum);
	       fflush (stderr);

               free (rowrecord);
               free (holder);
	       
	       
               return (0);

            }/* end if (3) */

            hitrow->linenumber = loclinenum;
            hitrow->rowname    = pursuitptr2;
            hitrow->value      = pursuitptr3;
            tally++;

            /* adjust the local record of rows we have read */
            *currrec = rowid;
            currrec++;

            /* move to 1st data item on the next line */
            while (! isgraph (*leadptr))
            {
               leadptr++;
            }/* end while *leadptr */

#ifdef VERBOSE
printf ("HIT: line = %ld, rn = %s. rid = %ld, v = %s, tally = %ld\n",
        hitrow->linenumber,
         hitrow->rowname,
          rowid,
           hitrow->value,
            tally);
fflush (stdout);
#endif /* VERBOSE */


         }/* end if (2)*/

      }
      else
      {
         /* we are at a comment line so ignore it */
         while (*leadptr != '\n')
         {
            leadptr++;
         }/* end while *leadptr */

         /* move to the first printable character of the next line */
         while (! isgraph (*leadptr))
         {
            leadptr++;
         }/* end while *leadptr */

      }/* end if (1) {*leadptr != '*'} */

      /* increment the line counter, even for comment lines */
      loclinenum++;

      /* preserve the value of the old column string name size */
      xfersize = newxfersize;

   }/* end while (leadptr < end) */


#ifdef VERBOSE
printf ("\nLAST PREPURGE CONTENTS OF RDSTRUCT ARRAY:\n");
for (z = 0; z < limitr; z++)
{
   if ((holder+z)->valid != 0)
   {
      printf ("[%ld] = line(%ld), rn(%s), v(%s)\n",
	      z,
               ((holder+z))->linenumber,
                ((holder+z))->rowname,
                 ((holder+z))->value);
      fflush (stdout);
   }/* end if (1) */

}/* end for */
printf ("\n");
fflush (stdout);
#endif /* VERBOSE */


   /*
    * Here the program makes a choice : 
    *
    *   if (tally == 1)
    *      purge one item
    *   if (tally == 2)
    *      purge two items in order
    *   else
    *      if (tally is in limits of the nlogn lookup table)
    *         if maxrows > (tally)log2(tally)
    *            sort the rowrecord
    *            purge the rowrecord
    *         else
    *            sequential purge of rowrecord
    *      else calculate (tally)log2(tally)
    *            if this is less than maxrows
    *               sort the rowrecord
    *               purge the rowrecord
    *            else
    *               sequential purge of rowrecord
    *
    */

   /* check for trivial values of tally */
   if (tally == 1L)
   {

#ifdef VERBOSE
printf ("LAST PURGE tally = 1\n");
fflush (stdout);
#endif /* VERBOSE */

      i      = *rowrecord;
      hitrow = (holder + i);
      if ((rdptr->value = atof (hitrow->value)) != 0.0L)
      {
         if (! (addeltotable(theelhashtable,
                              hitrow->linenumber, 
                               oldcol,
                                hitrow->rowname)))
         {
            free (rowrecord);
            free (holder);

            return (0);
         }/* end if (3) */

         rdptr->rowid = i;
         rdptr++;

#ifdef VERBOSE
printf ("GetCOLUMNS : L_PURGE cid = %ld, rid = %ld, cn = %s, rn = %s, v = %s\n",
        colid,
         i,
          oldcol,
           hitrow->rowname,
            hitrow->value);
fflush (stdout);
#endif /* VERBOSE */

      }
      else
      {
         warnings++;
         if (warnings <= MAXCOLWARNINGS)
         {
            /* mention the problem */
            fprintf (stderr,
                      "COLUMNS Warning (line %ld) value given = %s, not set\n",
                       hitrow->linenumber,
		        hitrow->value);
	    fflush (stderr);
         }/* end if (3) */

         numzeroels++; /* no move from zero-valued item */
	 
      }/* end if (2) */

   }
   else if (tally == 2L)
   {

#ifdef VERBOSE
printf ("LAST PURGE tally = 2\n");
fflush (stdout);
#endif /* VERBOSE */

      i = *rowrecord;
      j = *(rowrecord+1);

      if (i < j)
      {
         hitrow  = (holder + i);
         if ((rdptr->value = atof (hitrow->value)) != 0.0L)
         {
            if (! (addeltotable(theelhashtable,
                                 hitrow->linenumber, 
                                  oldcol,
                                   hitrow->rowname)))
            {
               free (rowrecord);
               free (holder);

               return (0);
            }/* end if (4) */

            rdptr->rowid = i;
            rdptr++;

#ifdef VERBOSE
printf ("GetCOLUMNS : L_PURGE cid = %ld, rid = %ld, cn = %s, rn = %s, v = %s\n",
        colid,
         i,
          oldcol,
           hitrow->rowname,
            hitrow->value);
fflush (stdout);
#endif /* VERBOSE */

         }
         else 
         {
            warnings++;
            if (warnings <= MAXCOLWARNINGS)
            {
               /* mention the problem */
               fprintf (stderr,
                "COLUMNS Warning (line %ld) value given = %s, not set\n",
                 hitrow->linenumber,
                  hitrow->value);
	       fflush (stderr);
            }/* end if (4) */

            numzeroels++; /* no move from zero-valued item */
	    
         }/* end if (3) */


         hitrow = (holder + j);
         if ((rdptr->value = atof (hitrow->value)) != 0.0L)
         {
            if (! (addeltotable(theelhashtable,
                                 hitrow->linenumber, 
                                  oldcol,
                                   hitrow->rowname)))
            {
               free (rowrecord);
               free (holder);

               return (0);
            }/* end if (4) */

            rdptr->rowid = j;
            rdptr++;

#ifdef VERBOSE
printf ("GetCOLUMNS : LAST PURGE cid = %ld, rid = %ld, cn = %s, rn = %s, v = %s\n",
        colid, 
         j, 
          oldcol, 
           hitrow->rowname,
            hitrow->value);
fflush (stdout);
#endif /* VERBOSE */

         }
         else
         {
            warnings++;
            if (warnings <= MAXCOLWARNINGS)
            {
               fprintf (stderr,
                "COLUMNS Warning (line %ld) value given = %s, not set\n",
                 hitrow->linenumber,
                  hitrow->value);
	       fflush (stderr);
            }/* end if (4) */

            numzeroels++; /* no move from zero-valued item */
	    
         }/* end if (3) */

      }
      else /* j < i */
      {
         hitrow = (holder + j);
         if ((rdptr->value = atof (hitrow->value)) != 0.0L)
         {
            if (! (addeltotable(theelhashtable,
                                 hitrow->linenumber, 
                                  oldcol,
                                   hitrow->rowname)))
            {
               free (rowrecord);
               free (holder);

               return (0);
            }/* end if (4) */

            rdptr->rowid = j;
            rdptr++;

#ifdef VERBOSE
printf ("GetCOLUMNS : L_PURGE cid = %ld, rid = %ld, cn = %s, rn = %s, v = %s\n",
        colid,
         j,
          oldcol,
           hitrow->rowname,
            hitrow->value);
fflush (stdout);
#endif /* VERBOSE */

         }
         else 
         {
            warnings++;
            if (warnings <= MAXCOLWARNINGS)
            {
               /* mention the problem */
               fprintf (stderr,
                "COLUMNS Warning (line %ld) value given = %s, not set\n",
                 hitrow->linenumber,
                  hitrow->value);
	       fflush (stderr);
            }/* end if (4) */

            numzeroels++; /* no move from zero-valued item */
	    
         }/* end if (3) */


         hitrow = (holder + i);
         if ((rdptr->value = atof (hitrow->value)) != 0.0L)
         {
            if (! (addeltotable(theelhashtable,
                                 hitrow->linenumber, 
                                  oldcol,
                                   hitrow->rowname)))
            {
               free (rowrecord);
               free (holder);

               return (0);
            }/* end if (4) */

            rdptr->rowid = i;
            rdptr++;

#ifdef VERBOSE
printf ("GetCOLUMNS : L_PURGE cid = %ld, rid = %ld, cn = %s, rn = %s, v = %s\n",
        colid,
         i,
          oldcol,
           hitrow->rowname,
            hitrow->value);
fflush (stdout);
#endif /* VERBOSE */

         }
         else
         {
            warnings++;
            if (warnings <= MAXCOLWARNINGS)
            {
               /* mention the problem */
               fprintf (stderr,
                "COLUMNS Warning (line %ld) value given = %s, not set\n",
                 hitrow->linenumber,
                  hitrow->value);
	       fflush (stderr);
            }/* end if (4) */

            numzeroels++; /* no move from zero-valued item */
	    
         }/* end if (3) */

      }/* end if (2) */
      
   }
   else /* we may need to sort */
   {
      if (tally < LOOKUPTABLESIZE)
      {
         if (*(nlogntable + tally) < limitr)
         {
            /* check for presorted array, sort if not the case */
            for (i = 1L,
		  j = *rowrecord,
		   currrec = (rowrecord + 1);
                    i < tally;
		     i++,
                      currrec++,
                       j = k)
            {
               k = *currrec;
               if (j > k)
               {
#ifdef VERBOSE
printf ("\nPURGE : sort start = \n");
for (z = 0; z < tally; z++)
{
   printf ("rowrecord[%ld] = %ld\n", 
	   z, 
	    rowrecord[z]);
   fflush (stdout);
}
printf ("\n");
fflush (stdout);
#endif /* VERBOSE */

                  sort (&rowrecord, tally);

#ifdef VERBOSE
printf ("\nPURGE : sort end = \n");
for (z = 0; z < tally; z++)
{
   printf ("rowrecord[%ld] = %ld\n", 
           z, 
            rowrecord[z]);
   fflush (stdout);
}
printf ("\n");
fflush (stdout);
#endif /* VERBOSE */

                  break;
		  
               }/* end if (4) */

            }/* end for i */


#ifdef VERBOSE
printf ("\nLAST PURGE : sort end = \n");
for (z = 0; z < tally; z++)
{
   printf ("rowrecord[%ld] = %ld\n", 
	   z, 
	    rowrecord[z]);
   fflush (stdout);
}
printf ("\n");
fflush (stdout);
#endif /* VERBOSE */

            /*
             * use tally as the loop counter, since the time
             * efficiency is better than if we used j and at the
             * end of the for loop we reset the tally to 0L
             */
            for(currrec = rowrecord;
                 tally > 0L;
		  tally--,
                   currrec++)
            {
               i      = *currrec;
               hitrow = (holder + i);
               if ((rdptr->value = atof (hitrow->value)) != 0.0L)
               {
                  if (! (addeltotable(theelhashtable,
                                       hitrow->linenumber, 
                                        oldcol,
                                         hitrow->rowname)))
                  {
                     free (rowrecord);
                     free (holder);
  
                     return (0);
                  }/* end if (5) */
  
                  rdptr->rowid = i;
                  rdptr++;
  
#ifdef VERBOSE
printf ("GetCOLUMNS : L_PURGE cid = %ld, rid = %ld, cn = %s, rn = %s, v = %s\n",
        colid,
         i,
          oldcol,
           hitrow->rowname,
            hitrow->value);
fflush (stdout);
#endif /* VERBOSE */

               }
               else
               {
                  warnings++;
                  if (warnings <= MAXCOLWARNINGS)
                  {
                     /* mention the problem */
                     fprintf (stderr,
                      "COLUMNS Warning (line %ld) value given = %s, not set\n",
                       hitrow->linenumber,
                        hitrow->value);
		     fflush (stderr);
                  }/* end if (5) */
  
                  numzeroels++; /* no move from zero-valued item */
		  
               }/* end if (4) */
  
            }/* end for currrec */
	    
         }
         else
         {
            /* SEQUENTIAL PURGE */

            /* we must assign all previous data to the MPSstruct */
            for (i = 0L,
		  hitrow = holder;
		   tally > 0L;
		    i++,
                     hitrow++) 
            {
               if (hitrow->valid != 0)
               {
                  /* expensive but necessary assignment */
                  if ((rdptr->value = atof (hitrow->value)) != 0.0L)
                  {
                     if (! (addeltotable(theelhashtable,
                                          hitrow->linenumber, 
                                           oldcol,
                                            hitrow->rowname)))
                     {
                        free (rowrecord);
                        free (holder);

                        return (0);
                     }/* end if (6) */

                     rdptr->rowid = i;
                     rdptr++;

#ifdef VERBOSE
printf ("GetCOLUMNS : LSPURGE cid = %ld, rid = %ld, cn = %s, rn = %s, v = %s\n",
        colid,
         i,
          oldcol,
           hitrow->rowname,
            hitrow->value);
fflush (stdout);
#endif /* VERBOSE */

                  }
                  else
                  {
                     warnings++;
                     if (warnings <= MAXCOLWARNINGS)
                     {
                        /* mention the problem */
                        fprintf (stderr,
                         "COLUMNS Warning (line %ld) value given = %s",
                          hitrow->linenumber,
                           hitrow->value);
			fprintf (stderr, ", not set\n");
			fflush (stderr);
                     }/* end if (6) */

                     numzeroels++; /* no move from zero-valued item */
		     
                  }/* end if (5) */

                  /* we must terminate the for loop eventually */
                  tally--;

               }/* end if (4) */

            }/* end for i */
	    
         }/* end if (3) */
	 
      }
      else
      {
         if (limitr > (tally * fastlog2(tally)))
         {
            /* check for presorted array, sort if not the case */
            for (i = 1L,
		  j = *rowrecord,
		   currrec = (rowrecord + 1);
                    i < tally;
		     i++,
                      currrec++,
                       j = k)
            {
               k = *currrec;
               if (j > k)
               {
		  
#ifdef VERBOSE
printf ("\nPURGE : sort start = \n");
for (z = 0; z < tally; z++)
{
   printf ("rowrecord[%ld] = %ld\n", 
	   z, 
	    rowrecord[z]);
   fflush (stdout);
}
printf ("\n");
fflush (stdout);
#endif /* VERBOSE */

                  sort (&rowrecord, tally);

#ifdef VERBOSE
printf ("\nPURGE : sort end = \n");
for (z = 0; z < tally; z++)
{
   printf ("rowrecord[%ld] = %ld\n", 
	   z, 
	    rowrecord[z]);
   fflush (stdout);
}
printf ("\n");
fflush (stdout);
#endif /* VERBOSE */

                  break;
		  
               }/* end if (4) */

            }/* end for i */
  		 
            for(currrec = rowrecord;
                 tally > 0L;
		  tally--,
                   currrec++)
            {
               i      = *currrec;
               hitrow = (holder + i);
               if ((rdptr->value = atof (hitrow->value)) != 0.0L)
               {
                  if (! (addeltotable(theelhashtable,
                                       hitrow->linenumber, 
                                        oldcol,
                                         hitrow->rowname)))
                  {
                     free (rowrecord);
                     free (holder);
  
                     return (0);
                  }/* end if (5) */
  
                  rdptr->rowid = i;
                  rdptr++;
  
#ifdef VERBOSE
printf ("GetCOLUMNS : L_PURGE cid = %ld, rid = %ld, cn = %s, rn = %s, v = %s\n",
        colid,
         i,
          oldcol,
           hitrow->rowname,
            hitrow->value);
fflush (stdout);
#endif /* VERBOSE */

               }
               else
               {
                  warnings++;
                  if (warnings <= MAXCOLWARNINGS)
                  {
                     /* mention the problem */
                     fprintf (stderr,
                      "COLUMNS Warning (line %ld) value given = %s, not set\n",
                       hitrow->linenumber,
                        hitrow->value);
		     fflush (stderr);
                  }/* end if (5) */
  
                  numzeroels++; /* no move from zero-valued item */
			   
               }/* end if (4) */
  
            }/* end for currrec */

         }
         else
         {
            /* PERFORM SEQUENTIAL PURGE */

            /* we must assign all previous data to the MPSstruct */
            for (i = 0L,
		  hitrow = holder;
		   tally > 0L;
		    i++,
                     hitrow++) 
            {
               if (hitrow->valid != 0)
               {
                  /* expensive but necessary assignment */
                  if ((rdptr->value = atof (hitrow->value)) != 0.0L)
                  {
                     if (! (addeltotable(theelhashtable,
                                          hitrow->linenumber, 
                                           oldcol,
                                            hitrow->rowname)))
                     {
                        free (rowrecord);
                        free (holder);

                        return (0);
                     }/* end if (6) */

                     rdptr->rowid = i;
                     rdptr++;

#ifdef VERBOSE
printf ("GetCOLUMNS : LSPURGE cid = %ld, rid = %ld, cn = %s, rn = %s, v = %s\n",
        colid,
         i,
          oldcol,
           hitrow->rowname,
            hitrow->value);
fflush (stdout);
#endif /* VERBOSE */

                  }
                  else
                  {
                     warnings++;
                     if (warnings <= MAXCOLWARNINGS)
                     {
                        /* mention the problem */
                        fprintf (stderr,
                         "COLUMNS Warning (line %ld) value given = %s",
                          hitrow->linenumber,
                           hitrow->value);
			fprintf (stderr, ", not set\n");
			fflush (stderr);
                     }/* end if (6) */

                     numzeroels++; /* no move from zero-valued item */
			   
                  }/* end if (5) */

                  /* we must terminate the for loop eventually */
                  tally--;

               }/* end if (4)*/

            }/* end for i */

         }/* end if (3) */

      }/* end if (2) */

   }/* end if (1) */


   /* add the old column to the column hash table */
#ifdef VERBOSE
printf ("GetCOLUMNS : LAST ADD COL cid = %ld, line = %ld, cn = %s\n",
        colid,
         loclinenum,
          oldcol);
fflush (stdout);
#endif /* VERBOSE */

   if (! (addcoltotable(thecolhashtable,
	                 colid,
	                  loclinenum,
                           oldcol)))
   {
      /*
       * column about to be defined in 2 non-contiguous blocks
       */
      free (rowrecord);
      free (holder);

      return (0);
   }/* end if (1) */	


   /* update the strings, and pointers for the name */
   memcpy (*dstptr, oldcol, xfersize);
   dstptr++;

   /* reflect change to the kernel data status */
   cdptr++;
   *cdptr = rdptr;


#ifdef VERBOSE
printf ("GetCOLUMNS : LAST COLID INCREMENT FROM colid = %ld\n", 
        colid);
fflush (stdout);
#endif /* VERBOSE */

   colid++;


   /* update the line number information for the calling function */
   *linenum = loclinenum;

   (*mystruct)->numbercols     = colid;
   elnumber = theelhashtable->totalcontents;
   (*mystruct)->numberelements = elnumber;
   (*mystruct)->numberzeroelements = numzeroels;


   /* 
    * ATTEMPT A SERIES OF "realloc" CALLS TO TRUNCATE THE MPSstruct 
    *
    * NOTE : The following code does not conform to ANSI C, yet it 
    *        is the only portable method at the moment that will 
    *        prevent a crash.
    */
#ifndef __ANSI__C
   /* set the best size for the colnames and their pointers */
   (*(*mystruct)->colnames) = 
    (char *) realloc (*(*mystruct)->colnames,
	              (colid * sizeof(char) * MAX_COLNAMESIZE));
   
   (*mystruct)->colnames = 
    (char **) realloc ((*mystruct)->colnames, (colid * sizeof(char *)));

   /* assign the data in the bounds vectors to a better sized array */
   (*mystruct)->upperboundsvector = 
    (double *) realloc ((*mystruct)->upperboundsvector,
	                (colid * sizeof(double)));
   
   (*mystruct)->lowerboundsvector = 
    (double *) realloc ((*mystruct)->lowerboundsvector,
	                (colid * sizeof(double)));

   /* we have elnumber, plus one last element {-1,0.0L} */
   (*mystruct)->kerneldata_row =
    (struct rowdata *) realloc ((*mystruct)->kerneldata_row,
	                        ((1UL + elnumber) * sizeof(struct rowdata)));

   /* we have space for (colid) cols and one last one pointing to end */
   (*mystruct)->kerneldata_col = 
    (struct rowdata **) realloc ((*mystruct)->kerneldata_col, 
                                 ((1L + colid) * sizeof(struct rowdata *)));
#endif /* __ANSI__C */
   

   /*
    * seal in the kernel data, using existing rdptr, cdptr
    */

   /* set up the last rowdata item in a bad format deliberately */
   rdptr->rowid = BADROW; /* a negative id in a list of positive ones */
   rdptr->value = 0.0;    /* a zero value in a sparse format          */
   
   /* make the last column data pointer point to this bad item */
   cdptr++;
   *cdptr = rdptr;

   /* final cleanup operations */
   free (rowrecord);
   free (holder);

   /* finally, report any excess warnings */
   if (warnings > MAXCOLWARNINGS)
   {
      fprintf (stderr,
               "\nCOLUMNS warnings not reported = %lu\n\n",
                (warnings - MAXCOLWARNINGS));
      fflush (stderr);
   }/* end if (1) */

   
   return (1);

}



/* ************************************************************************** */
/* *                                  GETRHS                                * */
/* ************************************************************************** */


int GetRHS (char **theMPSfile,
             struct MPSstruct **mystruct,
              rowhashtable *therowhtable,
	       char *end,
	        long *linenum)
/* pre  : We are given a pointer to a pointer to an array of characters,
 *        a pointer to a pointer to a valid MPSstruct, a pointer to a valid
 *        row hash table, a character pointer, and a pointer to a long 
 *        integer.
 * post : The scanned array of characters stretches from the first argument
 *        to the sentinel limit held in the fourth argument; this is the 
 *        "RHS" section of the MPS input file.  The row hash table is not 
 *        altered, but is used to check that rows specified in the section 
 *        are valid.
 *
 *        The RHS name is saved, and the dense RHS vector field of the MPS
 *        structure is updated to reflect the values provided.
 *
 *        If the 'atof' function should give a value of a double as '0.0',
 *        then, up to a limit, a warning is issued, but processing 
 *        continues.
 *
 *        The array of characters is updated to create strings, through 
 *        adding NULL delimiters.
 *
 *        The long integer is updated at the end to reflect the lines 
 *        processed from the MPS data.
 *
 *        Returns 1 on success, 0 on failure.
 */
{
   register long loclinenum,    /* local line number counter */
                 warnings = 0L, /* tally of warnings set     */
                 rowid;         /* holds row ids when needed */
   register short newlinefound,    /* checks for 2 data items per line */
                  firststring = 1; /* we will preserve the name of RHS */
   char *leadptr    = NULL, /* scans the char array input               */
        *pursuitptr = NULL, /* sets up the names of rows for extraction */
        *valptr     = NULL; /* sets up RHS values for extraction        */
   double *rhsvector = NULL; /* helps set value of item in RHS vector */


#ifdef VERBOSE
fprintf (stderr, "\nGetting rhs ...\n");
fflush (stderr);
#endif /* VERBOSE */

   /* INITIALISATIONS */
   loclinenum = (*linenum) + 1L; /* platform independent */

   rhsvector = (*mystruct)->rhsvector;

   leadptr = (*theMPSfile) + SKIPRHSCHARS; /* char just after 'S' of 'RHS' */

   /* start at the first string of the line beneath 'RHS' */
   while (! isgraph (*leadptr))
   {
      leadptr++;
   }/* end while *leadptr */

   /* END OF INITIALISATIONS */

   while (leadptr < end)
   {
      if (*leadptr != '*')
      {
         newlinefound = 0;

         /* we encounter the RHS name first of all */
         pursuitptr = leadptr;
         while (! isspace(*leadptr))
         {
            leadptr++;
         }/* end while *leadptr */

         /* if this is the first encounter, preserve the RHS name */
         if (firststring)
         {
	    /* make pursuitptr a string */
            *leadptr = '\0';
            leadptr++;

            /* copy over the string to the MPSstruct */
            strcpy ((*mystruct)->rhsname, pursuitptr);

#ifdef VERBOSE
printf ("GetRHS : rhsname = %s, from source = %s\n",
        (*mystruct)->rhsname, pursuitptr);
fflush (stdout);
#endif /* VERBOSE */

            /* set the flag to avoid this section from now on */
            firststring = 0;
         }/* end if (2) */

         /* shunt along to the row name */
         while (! isalnum(*leadptr))
         {
            leadptr++;
         }/* end while *leadptr */

         /* make the row name a string on pursuitptr */
         pursuitptr = leadptr;
         while (! isspace(*leadptr))
         {
            leadptr++;
         }/* end while *leadptr */
         *leadptr = '\0';
         leadptr++;

         /* shunt along to the value, picking up '-' if there */
         while (isspace(*leadptr))
         {
            leadptr++;
         }/* end while *leadptr */

         /* make the value a string on valptr */
         valptr = leadptr;
         while (! isspace(*leadptr))
         {
            leadptr++;
         }/* end while *leadptr */

         /* safety check to see if we are at a newline character */
         if (*leadptr == '\n')
         {
            newlinefound = 1;
         }/* end if (2) */

         /* set valptr to a string */
         *leadptr = '\0';


         /*
          * find the row id, and set the location in the vector to the
          * value extracted from valptr
          */

         if ((rowid = findrow(therowhtable, pursuitptr)) == BADROW)
         {
            fprintf (stderr,
		     "Row name \"%s\" at %ld does not exist\n\n",
                      pursuitptr,
		       loclinenum);
	    fflush (stderr);

            return (0);
         }/* end if (2) */

         if ((*(rhsvector + rowid) = atof (valptr)) == 0.0L)
         {
            warnings++;
            if (warnings <= MAXRHSWARNINGS)
            {
               /* mention the problem */
               fprintf (stderr,
                "RHS Warning (line %ld) value given = %s, not set\n",
                 loclinenum,
                  valptr);
	       fflush (stderr);
            }/* end if (3) */
	    
         }/* end if (2) */

#ifdef VERBOSE
printf ("GetRHS : added rowname = %s, id = %ld, value = %s line = %ld\n",
        pursuitptr,
         rowid,
          valptr,
           loclinenum);
fflush (stdout);
#endif /* VERBOSE */

         /* try to find a new line */
         while (! isgraph (*leadptr))
         {
            if (*leadptr == '\n')
            {
               newlinefound = 1;
            }/* end if (2) */
            leadptr++;
         }

         if (! newlinefound)
         {
            /* set up pursuitptr to the second row name */
            pursuitptr = leadptr;
            while (! isspace(*leadptr))
            {
               leadptr++;
            }/* end while *leadptr */
            *leadptr = '\0';
            leadptr++;

            /* shunt along to the value */
            while (isspace(*leadptr))
            {
               leadptr++;
            }/* end while *leadptr */

            /* set up valptr for the second value */
            valptr = leadptr;
            while (! isspace(*leadptr))
            {
               leadptr++;
            }/* end while *leadptr */
            *leadptr = '\0';
            leadptr++;


            /*
             * find the row id, and set the location in the vector to the
             * value extracted from valptr, for the second time
             */

            if ((rowid = findrow(therowhtable, pursuitptr)) == BADROW)
            {
               fprintf (stderr,
                "Row name \"%s\" at %ld does not exist\n\n",
                 pursuitptr,
                  loclinenum);
	       fflush (stderr);

               return (0);
            }/* end if (3) */

            if ((*(rhsvector + rowid) = atof (valptr)) == 0.0L)
            {
               warnings++;
               if (warnings <= MAXRHSWARNINGS)
               {
                  /* mention the problem */
                  fprintf (stderr,
                   "RHS Warning (line %ld) value given = %s, not set\n",
                    loclinenum,
                     valptr);
		  fflush (stderr);
               }/* end if (4) */
	       
            }/* end if (3) */

#ifdef VERBOSE
printf ("GetRHS : added rowname = %s, id = %ld, value = %s line = %ld\n",
        pursuitptr,
         rowid,
          valptr,
           loclinenum);
fflush (stdout);
#endif /* VERBOSE */

            /* move to start of data on the next line */
            while (! isgraph (*leadptr))
            {
               leadptr++;
            }/* end while *leadptr */

         }/* end if (2) */

      }
      else
      {
         /* we are at a comment line */
         while (*leadptr != '\n')
         {
            leadptr++;
         }/* end while *leadptr */

         while (! isgraph (*leadptr))
         {
            leadptr++;
         }/* end while *leadptr */

      }/* end if (1) */

      /* update the local line counter, even for comments */
      loclinenum++;

   }/* end while leadptr */

   /* update the value of the linenumber */
   *linenum = loclinenum;

   /* finally, report any excess warnings */
   if (warnings > MAXRHSWARNINGS)
   {
      fprintf (stderr, "\nRHS warnings not reported = %ld\n\n",
               (warnings - MAXRNGWARNINGS));
      fflush (stderr);
   }/* end if (1) */

   
   return (1);

}



/* ************************************************************************** */
/* *                                GETRANGES                               * */
/* ************************************************************************** */


int GetRANGES (char **theMPSfile,
                struct MPSstruct **mystruct,
                 rowhashtable *therowhtable,
		  char *end,
		   long *linenum)
/* pre  : We are given a pointer to a pointer to an array of characters,
 *        a pointer to a pointer to a valid MPSstruct, a pointer to a valid
 *        row hash table, a character pointer, and a pointer to a long 
 *        integer.
 * post : The scanned array of characters stretches from the first argument
 *        to the sentinel limit held in the fourth argument; this is the 
 *        "RANGES" section of the MPS input file.  The row hash table is 
 *        not altered, but is used to check that rows specified in the 
 *        section are valid.
 *
 *        The RANGES name is saved, and the dense RANGES vector field of 
 *        the MPS structure is updated to reflect the values provided.
 *
 *        If the 'atof' function should give a value of a double as '0.0',
 *        then, up to a limit, a warning is issued, but processing 
 *        continues.
 *
 *        The array of characters is updated to create strings, through 
 *        adding NULL delimiters.
 *
 *        The long integer is updated at the end to reflect the lines 
 *        processed from the MPS data.
 *
 *        Returns 1 on success, 0 on failure.
 */
{
   register long loclinenum,    /* local line number counter */
                 warnings = 0L, /* tally of warnings set     */
                 rowid;         /* holds row ids when needed */
   register short newlinefound,    /* checks for 2 data items per line    */
                  firststring = 1; /* we will preserve the name of RANGES */
   char *leadptr    = NULL, /* scans the char array input               */
        *pursuitptr = NULL, /* sets up the names of rows for extraction */
        *valptr     = NULL; /* sets up RANGES value for extraction      */
   double *rngvector = NULL; /* helps set value of item in RANGES vector */


#ifdef VERBOSE
fprintf (stderr, "\nGetting ranges ...\n");
fflush (stderr);
#endif /* VERBOSE */


   /* INITIALISATIONS */
   loclinenum = (*linenum) + 1L; /* platform independent */

   rngvector = (*mystruct)->rangesvector;

   /* char just after 'S' of "RANGES" */
   leadptr = (*theMPSfile) + SKIPRNGCHARS;

   /* start at the first string of the line beneath 'RANGES' */
   while (! isgraph (*leadptr))
   {
      leadptr++;
   }/* end while *leadptr */

   /* END OF INITIALISATIONS */

   while (leadptr < end)
   {
      if (*leadptr != '*')
      {
         newlinefound = 0;

         /* we encounter the RANGES name first of all */
         pursuitptr = leadptr;
         while (! isspace(*leadptr))
         {
            leadptr++;
         }/* end while *leadptr */

         /* if this is the first encounter, preserve the RANGES name */
         if (firststring)
         {
            /* make pursuitptr a string */
            *leadptr = '\0';
            leadptr++;

            /* copy over the string to the MPSstruct */
            strcpy ((*mystruct)->ranname, pursuitptr);

#ifdef VERBOSE
printf ("GetRANGES : ranname = %s, from source = %s\n",
        (*mystruct)->ranname, pursuitptr);
fflush (stdout);
#endif /* VERBOSE */

            /* set the flag to avoid this section from now on */
            firststring = 0;
	    
         }/* end if (2) */

         /* shunt along to the row name */
         while (! isalnum(*leadptr))
         {
            leadptr++;
         }/* end while *leadptr */

         /* make the row name a string on pursuitptr */
         pursuitptr = leadptr;
         while (! isspace(*leadptr))
         {
            leadptr++;
         }/* end while *leadptr */
         *leadptr = '\0';
         leadptr++;

         /* shunt along to the value, picking up '-' if there */
         while (isspace(*leadptr))
         {
            leadptr++;
         }/* end while *leadptr */

         /* make the value a string on valptr */
         valptr = leadptr;
         while (! isspace(*leadptr))
         {
            leadptr++;
         }/* end while *leadptr */

         /* safety check to see if we are at a newline character */
         if (*leadptr == '\n')
         {
            newlinefound = 1;
         }/* end if (2) */

         /* set valptr to a string */
         *leadptr = '\0';


         /*
          * find the row id, and set the location in the vector to the
          * value extracted from valptr
          */

         if ((rowid = findrow(therowhtable, pursuitptr)) == BADROW)
         {
            fprintf (stderr,
		     "Row name \"%s\" at %ld does not exist\n\n",
                      pursuitptr,
		       loclinenum);
	    fflush (stderr);

            return (0);
         }/* end if (2) */

         if ((*(rngvector + rowid) = atof (valptr)) == 0.0L)
         {
            warnings++;
            if (warnings <= MAXRNGWARNINGS)
            {
               /* mention the problem */
               fprintf (stderr,
                "RANGES Warning (line %ld) value given = %s, not set\n",
                 loclinenum,
                  valptr);
	       fflush (stderr);
            }/* end if (3) */
	    
         }/* end if (2) */

#ifdef VERBOSE
printf ("GetRANGES : added rowname = %s, id = %ld, value = %s line = %ld\n",
        pursuitptr,
         rowid,
          valptr,
           loclinenum);
fflush (stdout);
#endif /* VERBOSE */

         /* try to find a new line */
         while (! isgraph (*leadptr))
         {
            if (*leadptr == '\n')
            {
               newlinefound = 1;
            }/* end if (2) */
	    
            leadptr++;
	    
	 }/* end while *leadptr */

         if (! newlinefound)
         {
            /* set up pursuitptr to the second row name */
            pursuitptr = leadptr;
            while (! isspace(*leadptr))
            {
               leadptr++;
            }/* end while *leadptr */
            *leadptr = '\0';
            leadptr++;

            /* shunt along to the value */
            while (isspace(*leadptr))
            {
               leadptr++;
            }/* end while *leadptr */

            /* set up valptr for the second value */
            valptr = leadptr;
            while (! isspace(*leadptr))
            {
               leadptr++;
            }
            *leadptr = '\0';
            leadptr++;


            /*
	     * find the row id, and set the location in the vector to the
             * value extracted from valptr, for the second time
             */

            if ((rowid = findrow(therowhtable, pursuitptr)) == BADROW)
            {
               fprintf (stderr,
		        "Row name \"%s\" at %ld does not exist\n\n",
                         pursuitptr,
		          loclinenum);
	       fflush (stderr);

               return (0);
            }/* end if (3) */

            if ((*(rngvector + rowid) = atof (valptr)) == 0.0L)
            {
               warnings++;
               if (warnings <= MAXRNGWARNINGS)
               {
                  /* mention the problem */
                  fprintf (stderr,
                   "RANGES Warning (line %ld) value given = %s, not set\n",
                    loclinenum,
                     valptr);
		  fflush (stderr);
               }/* end if (4) */
	       
            }/* end if (3) */

#ifdef VERBOSE
printf ("GetRANGES : added rowname = %s, id = %ld, value = %s line = %ld\n",
        pursuitptr,
         rowid,
          valptr,
           loclinenum);
fflush (stdout);
#endif /* VERBOSE */

            /* move to start of data on the next line */
            while (! isgraph (*leadptr))
            {
              leadptr++;
            }

         }/* end if (2) */

      }
      else
      {
         /* we are at a comment line */
         while (*leadptr != '\n')
         {
            leadptr++;
         }/* end while *leadptr */

         while (! isgraph (*leadptr))
         {
            leadptr++;
         }/* end while *leadptr */

      }/* end if (1) */

      /* update the local counters */
      loclinenum++;

   }/* end while leadptr */

   /* update the value of the linenumber */
   *linenum = loclinenum;

   /* finally, report any excess warnings */
   if (warnings > MAXRNGWARNINGS)
   {
      fprintf (stderr,
	       "\nRANGES warnings not reported = %ld\n\n",
                (warnings - MAXRNGWARNINGS));
      fflush (stderr);
   }/* end if (1) */

   
   return (1);

}



/* ************************************************************************** */
/* *                               GETBOUNDS                                * */
/* ************************************************************************** */


int GetBOUNDS (char **theMPSfile, 
                struct MPSstruct **mystruct,
                 colhashtable *thecolhashtable, 
                  char *end, 
                   long *linenum)
/* pre  : We are given a pointer to a pointer to an array of characters,
 *        a pointer to a pointer to a valid MPSstruct, a pointer to a valid
 *        column hash table, a character pointer, and a pointer to a long
 *        integer.
 * post : The scanned array of characters stretches from the first argument
 *        to the sentinel limit held in the fourth argument; this is the 
 *        "BOUNDS" section of the MPS input file.  The column hash table is 
 *        not altered, but is used to check that columns specified in the
 *        section are valid.
 *
 *        The BOUNDS name is saved, and the dense upper BOUNDS vector field
 *        of the MPS structure is updated, as well as the lower BOUNDS vector
 *        to reflect the values provided.  The exact values are preset to
 *        +(infinity) and zero respectively first of all.
 *
 *        If the 'atof' function should give a value of a double as '0.0',
 *        then, up to a limit, a warning is issued, but processing continues.
 *
 *        The array of characters is updated to create strings, through 
 *        adding NULL delimiters.
 *
 *        The long integer is updated at the end to reflect the lines 
 *        processed from the MPS data.
 *
 *        Returns 1 on success, 0 on failure.
 */
{
   double *upptr  = NULL,        /* pointer to upper bounds vector   */
          *lowptr = NULL;        /* pointer to lower bounds vector   */
   register double tempval;      /* stores the converted double      */
   register long loclinenum,     /* local line number counter        */
	         warnings = 0L,  /* number of warnings               */
	         colid;          /* ID of the current column         */
   char *leadptr    = NULL,      /* character array scanner          */
        *pursuitptr = NULL,      /* string lock 1                    */
        *valptr     = NULL;      /* points to ASCII "double" value   */
   register char tempChar,       /* the second character of the type */
	         gofurther;      /* sentinel for further scanning    */
   
   
#ifdef VERBOSE
fprintf (stderr, "\nGetting bounds ...\n\n");
fflush (stderr);
#endif /* VERBOSE */
      
   /* use a lock as a sentinel for now */
   pursuitptr = *theMPSfile;
   
   /* 
    * find the number of new line characters in this section, using
    * "colid" temporarily here for efficiency
    */
   colid = -1L; /* disregard the "BOUNDS\n" new line */
   for (leadptr = pursuitptr;
	 leadptr < end;
	  leadptr++)
   {
      if (*leadptr == '\n')
      {
	 colid++;
      }/* end if (1) */
      
   }/* end for leadptr */
   
   /* reduce branching by terminating here for an empty section */
   if (! colid)
   {
      /* we add one to the line number count */
      (*linenum)++;
      
      return (1);
      
   }/* end if (1) */
   
   
   /* 
    * We have at least one line of data in the BOUNDS section, so scan 
    * this first line to get the name of the BOUNDS.
    */
   
   /* set up the local line counter */
   loclinenum = (*linenum) + 1L;

   /* scan characters from just past the 'S' of "BOUNDS" */
   leadptr = pursuitptr + SKIPBNDCHARS;
   
   /* get to the first column of the first BOUNDS line */
   while (! isgraph (*leadptr))
   {
      leadptr++;
   }/* end while *leadptr */
   
   /* get to the first type of bounds, skipping every comment line */
   while (leadptr < end)
   {
      /* if this is not a comment line then leave the loop */
      if (*leadptr != '*')
      {
	 break;
      }/* end if (1) */
      
      /* scan the line to the end */
      while (*leadptr != '\n')
      {
	 leadptr++;
      }/* end while *leadptr */
      
      /* get to the first column of the next line */
      while (! isgraph (*leadptr))
      {
	 leadptr++;
      }/* end while *leadptr */
      
      /* don't forget to update the local line counter */
      loclinenum++;
      
   }/* end while leadptr */
   
   
   /* 
    * Do not forget to break out early when the whole of the BOUNDS 
    * section is comments.
    */
   if (leadptr >= end)
   {
      /* update the line counter */
      *linenum = loclinenum;
      
      return (1);
      
   }/* end if (1) */
   
   
   /*
    * At this stage we are at the first line of data for the BOUNDS 
    * section, and we shall assume that there is a "value" field.
    */
   gofurther = 1;
   
   /* set up the pointers to the upper and lower bounds vectors */
   upptr  = (*mystruct)->upperboundsvector;
   lowptr = (*mystruct)->lowerboundsvector;
   
   /* get the second character of the type, then jump over the type */
   leadptr++;
   tempChar = *leadptr;
   leadptr++;   
   
   /* get to the bounds name of the data line */
   while (isspace (*leadptr))
   {
      leadptr++;
   }/* end while *leadptr */
   
   /* lock onto the bounds name */
   pursuitptr = leadptr;
   
   /* get to the end of the name */
   while (! isspace (*leadptr))
   {
      leadptr++;
   }/* end while *leadptr */
   
   /* make a string right here */
   *leadptr = '\0';
   leadptr++;
   
   /* save the name in the data structure */
   strcpy ((*mystruct)->bndname, pursuitptr);
   
#ifdef VERBOSE
printf ("GetBOUNDS : bndname = %s, from source = %s, line (%ld)\n",
        (*mystruct)->bndname,
         pursuitptr,
          loclinenum);
fflush (stdout);
#endif /* VERBOSE */   
   
   /* get to the column name */
   while (isspace (*leadptr))
   {
      leadptr++;
   }/* end while *leadptr */
   
   /* lock on to the column name, and recycle "pursuitptr" */
   pursuitptr = leadptr;

   /* get to the end of the column name */
   while (! isspace (*leadptr))
   {
      leadptr++;
   }/* end while *leadptr */

   /* now pay careful attention to whether or not there is a '\n' character */
   if (*leadptr == '\n')
   {
      /* make a string where we are locked on pursuitptr */
      *leadptr = '\0';
      leadptr++;
      
      /* get to the first column of the next line */
      while (! isgraph (*leadptr))
      {
	 leadptr++;
      }/* end while *leadptr */
      
      /* there will be no other fields to process */
      gofurther = 0;
      
      /* don't forget to update the number of lines processed */
      loclinenum++;
   }
   else
   {
      /* make a string where we are */
      *leadptr = '\0';
      leadptr++;
      
      /* carefully scan all of the "space" characters for a hidden new line */
      while (isspace (*leadptr))
      {
	 /* if we have a new line ... */
	 if (*leadptr == '\n')
	 {
	    /* skip to the first column of the next line */
	    leadptr++;
	    
	    while (! isgraph (*leadptr))
	    {
	       leadptr++;
	    }/* end while *leadptr */
	    
	    /* there are no more fields to process */
	    gofurther = 0;
	    
	    /* we will update the count of the lines processed */
	    loclinenum++;
	    
	    /* we can leave the overlying loop early */
	    break;
	    
	 }/* end if (2) */
	 
	 /* get to the first "non-space" character */
	 leadptr++;
	 
      }/* end while *leadptr */
      
   }/* end if (1) */
   
   /* get a value field only if you have to */
   if (gofurther)
   {      
      /* lock on to the ASCII double value */
      valptr = leadptr;
      
      /* shift to the end of the characters */
      while (! isspace (*leadptr))
      {
	 leadptr++;
      }/* end while *leadptr */
      
      /* make a string where we are */
      *leadptr = '\0';
      leadptr++;
      
      /* get to the first column of the next line */
      while (! isgraph (*leadptr))
      {
	 leadptr++;
      }/* end while *leadptr */
      
      /* don't forget to update the local line number */
      loclinenum++;
      
   }/* end if (1) */

      
   /*
    * Now we are ready to deal with the string fields that have 
    * been isolated on "pursuitptr", and possibly "valptr" based 
    * on the BOUNDS type isolated by the character in "tempChar".
    */
   if ((colid = findcol (thecolhashtable, pursuitptr)) == BADCOL)
   {
      fprintf (stderr,
	       "Column name \"%s\" at %ld does not exist\n\n",
	        pursuitptr,
	         loclinenum);
      fflush (stderr);
      
      return (0);
   }/* end if (1) */
   
   switch (tempChar)
   {
      case ('O') : /* LO */
	           if ((*(lowptr + colid) = atof (valptr)) == 0.0)
		   {
		      warnings++;
		      if (warnings < MAXBNDWARNINGS)
		      {
			 /* mention the problem */
			 fprintf (stderr,
			  "BOUNDS Warning (line %ld) value given = %s",
			   loclinenum,
			    valptr);
			 fprintf (stderr,
			  ", not set\n");
			 fflush (stderr);
		      }/* end if (2) */
		      
		   }/* end if (1) */
		   
#ifdef VERBOSE
printf ("GetBOUNDS : LO at col id = %ld, value = %E, line (%ld)\n",
        colid,
         *(lowptr + colid),
          loclinenum);
fflush (stdout);
#endif /* VERBOSE */
		   
                   break;
	 
      case ('P') : /* UP */
	           if ((tempval = atof (valptr)) == 0.0)
		   {
		      warnings++;
		      if (warnings < MAXBNDWARNINGS)
		      {
			 /* mention the problem */
			 fprintf (stderr,
			  "BOUNDS Warning (line %ld) value given = %s",
			   loclinenum,
			    valptr);
			 fprintf (stderr,
			  ", not set\n");
			 fflush (stderr);
		      }/* end if (2) */
		            
		   }
		   else
		   {
		      *(upptr + colid) = tempval;
		   }/* end if (1) */
		   
#ifdef VERBOSE
printf ("GetBOUNDS : UP at col id = %ld, value = %E, line (%ld)\n",
        colid,
         *(upptr + colid),
          loclinenum);
fflush (stdout);
#endif /* VERBOSE */

                   break;
	 
      case ('X') : /* FX */
	           if ((tempval = atof (valptr)) == 0.0)
		   {
		      warnings++;
		      if (warnings < MAXBNDWARNINGS)
		      {
			 /* mention the problem */
			 fprintf (stderr,
			  "BOUNDS Warning (line %ld) value given = %s\n",
			   loclinenum,
			    valptr);
			 fflush (stderr);
			 
		      }/* end if (2) */
		      
		   }/* end if (1) */
		   
		   *(upptr  + colid) = tempval;
		   *(lowptr + colid) = tempval;
		   
#ifdef VERBOSE
printf ("GetBOUNDS : FX at col id = %ld, values = (%E , %E), line (%ld)\n",
        colid,
         *(lowptr + colid),
          *(upptr + colid),
           loclinenum);
fflush (stdout);
#endif /* VERBOSE */

                   break;
	 
	 
      case ('R') : /* FR */
	           *(lowptr + colid) = MINUS_INF;
		   *(upptr  + colid) = PLUS_INF;
		   
#ifdef VERBOSE
printf ("GetBOUNDS : FR at col id = %ld, values = (-inf, +inf), line (%ld)\n",
        colid,
         loclinenum);
fflush (stdout);
#endif /* VERBOSE */

                   break;
	 
      case ('I') : /* MI */
	           if (valptr != NULL)
		   {
		      if ((tempval = atof (valptr)) == 0.0)
		      {
			 warnings++;
			 if (warnings < MAXBNDWARNINGS)
			 {
			    fprintf (stderr,
			     "BOUNDS Warning (line %ld) value given = %s",
	                      loclinenum,
                               valptr);
			    fprintf (stderr,
			     ", not set\n");
			    fflush (stderr);
			 }/* end if (3) */
			 
		      }
		      else
		      {
			 *(upptr + colid) = tempval;
		      }/* end if (2) */
		      
		   }/* end if (1) */
		   *(lowptr + colid) = MINUS_INF;
		   
#ifdef VERBOSE
printf ("GetBOUNDS : MI at col id = %ld, value = -inf, line (%ld)\n",
        colid,
         loclinenum);
fflush (stdout);
#endif /* VERBOSE */

                   break;
		   
      case ('L') : /* PL */
	           if (valptr != NULL)
		   {
		      if ((tempval = atof (valptr)) == 0.0)
		      {
			 warnings++;
			 if (warnings < MAXBNDWARNINGS)
			 {
			    fprintf (stderr,
			     "BOUNDS Warning (line %ld) value given = %s",
	                      loclinenum,
                               valptr);
			    fprintf (stderr,
			     ", not set\n");
			    fflush (stderr);
			    
			 }/* end if (3) */
			 
		      }
		      else
		      {
			 *(lowptr + colid) = tempval;
		      }/* end if (2) */
		      
		   }/* end if (1) */
		   *(upptr + colid) = PLUS_INF;
		   
#ifdef VERBOSE
printf ("GetBOUNDS : PL at col id = %ld, value = +inf, line (%ld)\n",
        colid,
         loclinenum);
fflush (stdout);
#endif /* VERBOSE */

                   break;
	 
      case ('V') : /* BV */
	           *(upptr  + colid) = 1.0;
		   *(lowptr + colid) = 0.0;
		   
#ifdef VERBOSE
printf ("GetBOUNDS : BV at col id = %ld, values = (0,1), line (%ld)\n",
        colid,
         loclinenum);
fflush (stdout);
#endif /* VERBOSE */

                   break;		   
	 
      default    : /* undefined type */
	           warnings++;
	           if (warnings < MAXBNDWARNINGS)
		   {
	              fprintf (stderr,
	               "BOUNDS Warning (line %ld) undefined type\n",
	                loclinenum);
		      fflush (stderr);
		   }/* end if (1) */
	    
	           break;
   
   }/* end switch tempChar */
      
   if (valptr != NULL)
   {
      valptr = NULL;
   }/* end if (1) */

      
   /*
    * At this point we have processed the first line of data in the BOUNDS 
    * section, now we process the rest without the need to catch the name 
    * of the bound.
    */
   while (leadptr < end)
   {
      if (*leadptr != '*')
      {
	 /* get the second character of the BOUNDS type */
	 leadptr++;
	 tempChar = *leadptr;
	 leadptr++;
	 
	 /* get to the column name by skipping over the BOUNDS name */
	 while (isspace (*leadptr))
	 {
	    leadptr++;
	 }/* end while *leadptr */
	 
	 while (! isspace (*leadptr))
	 {
	    leadptr++;
	 }/* end while *leadptr */
	 
	 while (isspace (*leadptr))
	 {
	    leadptr++;
	 }/* end while *leadptr */
	 
	 /* 
	  * get the column name into string format paying very careful
	  * attention to whether or not a new line character is here
	  */
	 pursuitptr = leadptr;
	 
	 while (! isspace (*leadptr))
	 {
	    leadptr++;
	 }/* end while *leadptr */
	 
	 /* assume we have one more column to go always */
	 gofurther = 1;
	 
         if (*leadptr == '\n')
         {
            /* make a string where we are locked on pursuitptr */
            *leadptr = '\0';
            leadptr++;
      
            /* there will be no other fields to process */
            gofurther = 0;
      
         }
         else
         {
            /* make a string where we are */
            *leadptr = '\0';
            leadptr++;
      
            /* carefully scan all of the "space" characters for a hidden new line */
            while (isspace (*leadptr))
            {
	       /* if we have a new line ... */
	       if (*leadptr == '\n')
	       {
		  /* get to the next character for consistency */
		  leadptr++;
		  
	          /* there are no more fields to process */
	          gofurther = 0;
	    
	          /* we can leave the overlying loop early */
	          break;
	    
	       }/* end if (3) */
	 
	       /* keep going through the "space" characters */
	       leadptr++;
	 
            }/* end while *leadptr */
      
         }/* end if (2) */
	 
         /* get a value field only if you have to */
         if (gofurther)
         {
            /* lock on to the ASCII double value */
            valptr = leadptr;
      
            /* shift to the end of the characters */
            while (! isspace (*leadptr))
            {
	       leadptr++;
            }/* end while *leadptr */
      
            /* make a string where we are */
            *leadptr = '\0';
            leadptr++;
      
         }/* end if (2) */

         /*
          * now we are ready to deal with the string fields that have 
          * been isolated on "pursuitptr", and possibly "valptr" based 
          * on the BOUNDS type isolated by the character in "tempChar"
          */
         if ((colid = findcol (thecolhashtable, pursuitptr)) == BADCOL)
         {
            fprintf (stderr,
	     "Column name \"%s\" st %ld does not exist\n\n",
              pursuitptr,
	       loclinenum);
	    fflush (stderr);
      
            return (0);
         }/* end if (2) */
   
         switch (tempChar)
         {
            case ('O') : /* LO */
	                 if ((*(lowptr + colid) = atof (valptr)) == 0.0)
		         {
		            warnings++;
		            if (warnings < MAXBNDWARNINGS)
		            {
			       /* mention the problem */
			       fprintf (stderr,
			        "BOUNDS Warning (line %ld) value given = %s",
			         loclinenum,
			          valptr);
			       fprintf (stderr,
			        ", not set\n");
			       fflush (stderr);
		            }/* end if (3) */
		      
		         }/* end if (2) */
		   
#ifdef VERBOSE
printf ("GetBOUNDS : LO at col id = %ld, value = %E, line (%ld)\n",
        colid,
         *(lowptr + colid),
          loclinenum);
fflush (stdout);
#endif /* VERBOSE */
		   
                         break;
	 
            case ('P') : /* UP */
	                 if ((tempval = atof (valptr)) == 0.0)
		         {
		            warnings++;
		            if (warnings < MAXBNDWARNINGS)
		            {
			       /* mention the problem */
			       fprintf (stderr,
			        "BOUNDS Warning (line %ld) value given = %s",
			         loclinenum,
			          valptr);
			       fprintf (stderr,
			        ", not set\n");
			       fflush (stderr);
		            }/* end if (3) */
		      
		         }
		         else
		         {
		            *(upptr + colid) = tempval;
		         }/* end if (2) */

#ifdef VERBOSE
printf ("GetBOUNDS : UP at col id = %ld, value = %E, line (%ld)\n",
        colid,
         *(upptr + colid),
          loclinenum);
fflush (stdout);
#endif /* VERBOSE */

                         break;
	 
            case ('X') : /* FX */
	                 if ((tempval = atof (valptr)) == 0.0)
		         {
		            warnings++;
		            if (warnings < MAXBNDWARNINGS)
		            {
			       /* mention the problem */
			       fprintf (stderr,
			        "BOUNDS Warning (line %ld) value given = %s\n",
			         loclinenum,
			          valptr);
			       fflush (stderr);
			       
			    }/* end if (3) */
			    
		         }/* end if (2) */
			 		    
                         *(upptr  + colid) = tempval;
		         *(lowptr + colid) = tempval;

#ifdef VERBOSE
printf ("GetBOUNDS : FX at col id = %ld, values = (%E , %E), line (%ld)\n",
        colid,
         *(lowptr + colid),
          *(upptr + colid),
           loclinenum);
fflush (stdout);
#endif /* VERBOSE */

                         break;
	 
	 
            case ('R') : /* FR */
	                 *(lowptr + colid) = MINUS_INF;
		         *(upptr  + colid) = PLUS_INF;
		   
#ifdef VERBOSE
printf ("GetBOUNDS : FR at col id = %ld, values = (-inf , +inf), line (%ld)\n",
        colid,
         loclinenum);
fflush (stdout);
#endif /* VERBOSE */

                         break;
	 
            case ('I') : /* MI */
	                 if (valptr != NULL)
		         {
		            if ((tempval = atof (valptr)) == 0.0)
		            {
			       warnings++;
			       if (warnings < MAXBNDWARNINGS)
			       {
			          fprintf (stderr,
			           "BOUNDS Warning (line %ld) value given = %s",
	                            loclinenum,
                                     valptr);
			          fprintf (stderr,
			           ", not set\n");
			          fflush (stderr);
				  
			       }/* end if (4) */
			       
		            }
		            else
		            {
			       *(upptr + colid) = tempval;
		            }/* end if (3) */
		      
		         }/* end if (2) */
		         *(lowptr + colid) = MINUS_INF;
		   
#ifdef VERBOSE
printf ("GetBOUNDS : MI at col id = %ld, value = -inf, line (%ld)\n",
        colid,
         loclinenum);
fflush (stdout);
#endif /* VERBOSE */

                         break;
		   
            case ('L') : /* PL */
	                 if (valptr != NULL)
		         {
		            if ((tempval = atof (valptr)) == 0.0)
		            {
			       warnings++;
			       if (warnings < MAXBNDWARNINGS)
			       {
			          fprintf (stderr,
			           "BOUNDS Warning (line %ld) value given = %s",
	                            loclinenum,
                                     valptr);
			          fprintf (stderr,
			           ", not set\n");
			          fflush (stderr);
				  
			       }/* end if (4) */
			       
		            }
		            else
		            {
			       *(lowptr + colid) = tempval;
		            }/* end if (3) */
		      
		         }/* end if (2) */
		         *(upptr + colid) = PLUS_INF;
		   
#ifdef VERBOSE
printf ("GetBOUNDS : PL at col id = %ld, value = +inf, line (%ld)\n",
        colid,
         loclinenum);
fflush (stdout);
#endif /* VERBOSE */

                         break;
	 
            case ('V') : /* BV */
	                 *(upptr  + colid) = 1.0;
		         *(lowptr + colid) = 0.0;
		   
#ifdef VERBOSE
printf ("GetBOUNDS : BV at col id = %ld, values = (0,1), line (%ld)\n",
        colid,
         loclinenum);
fflush (stdout);
#endif /* VERBOSE */

                         break;		   
	 
            default    : /* undefined type */
		         warnings++;
	                 if (warnings < MAXBNDWARNINGS)
			 {
	                    fprintf (stderr,
	                     "BOUNDS Warning (line %ld) undefined type\n",
	                      loclinenum);
			    fflush (stderr);
			 }/* end if (2) */
	    
	                 break;
   
         }/* end switch tempChar */
	 
	 /* reset the "valptr" local variable for safety */
	 if (valptr != NULL)
	 {
	    valptr = NULL;
	 }/* end if (2) */

      }
      else 
      {
         /* comment line */
	 while (*leadptr != '\n')
	 {
	    leadptr++;
	 }/* end while *leadptr */
	 
      }/* end if (1) */
      
      /* shunt along to the first data column of the next line */
      while (! isgraph (*leadptr))
      {
	 leadptr++;
      }/* end while *leadptr */
      
      /* update the local line number counter */
      loclinenum++;
      
   }/* end while (leadptr < end) */
   
   /* 
    * Hard part all done ! 
    */
   
   /* update the local line number count */
   *linenum = loclinenum;
   
   /* report any excess warnings */
   if (warnings > MAXBNDWARNINGS)
   {
      fprintf (stderr,
	       "\nBOUNDS warnings not reported = %ld\n\n",
	        (warnings - MAXBNDWARNINGS));
      fflush (stderr);
   }/* end if (1) */
   
   
   return (1);
	 
}



/* ************************************************************************** */
/* *                              GETMPSDATA                                * */
/* ************************************************************************** */


struct MPSstruct *GetMPSdata (char *filename)
/* pre  : We are given a valid pointer to a string of characters.
 * post : This is the overall function responsible for transforming the
 *        MPS input file located by its argument into the final MPS 
 *        structure.
 *
 *        In any error situation, it returns NULL, otherwise it returns a
 *        pointer to an internally allocated MPS structure with the MPS
 *        data residing within it.
 */
{
   char *filedata = NULL,
        *nameptr  = NULL,
        *rowptr   = NULL,
        *colptr   = NULL,
        *rhsptr   = NULL,
        *ranptr   = NULL,
        *bndptr   = NULL,
        *endptr   = NULL;
   long emprowctr = 0L,
        empcolctr = 0L;
   rowhashtable myrowhashtable;
   colhashtable mycolhashtable;
   elhashtable  myelhashtable;
   long linenumber = 1L;
   unsigned long thefilesize;
   struct MPSstruct *mympsptr = NULL;

#ifdef VERBOSE
   register unsigned long i;
#endif /* VERBOSE */

   
   /* put file into main memory : PASS #1 */
   if ((filedata = Readfileintomemory (filename, &thefilesize)) == NULL)
   {
      return (NULL);
   }/* end if (1) */


   /*
    *  set all the sentinel pointers and obtain empirical row
    *  and column counts : PASS #2
    */
   if (! (SetSectionPtrs (filedata,
	                   &nameptr,
	                    &rowptr,
	                     &colptr,
	                      &rhsptr,
                               &ranptr,
			        &bndptr,
				 &endptr,
				  &emprowctr,
				   &empcolctr,
                                    thefilesize)))
   {
      free (filedata);

      return (NULL);
   }/* end if (1) */


   /* initialisation of the MPS structure */
   if ((mympsptr = InitMPSstruct (emprowctr, empcolctr)) == NULL)
   {
      free (filedata);

      return (NULL);
   }/* end if (1) */


   /*
    * extract file data into intermediate memory form : PASS #3
    */

   /* get the name of the LP problem */
   if (!(GetNAME(&nameptr, &mympsptr, rowptr, &linenumber)))
   {
      free (filedata);
      DeleteMPSstruct (&mympsptr);

      return (NULL);
   }/* end if (1) */
   
#ifdef VERBOSE
printf ("GetNAME : lpname = %s\n\n", 
        mympsptr->lpname);
fflush (stdout);
#endif /* VERBOSE */


   /* set up the row hash table */
   if (!(initrowhashtable (&myrowhashtable, emprowctr)))
   {
      free (filedata);
      DeleteMPSstruct (&mympsptr);

      return (NULL);
   }/* end if (1) */

   /* get the rows */
   if (!(GetROWS(&rowptr,
	          &mympsptr,
	           &myrowhashtable,
                    colptr,
	             &linenumber)))
   {
      deleterowhashtable (&myrowhashtable);
      free (filedata);
      DeleteMPSstruct (&mympsptr);

      return (NULL);
   }/* end if (1) */

#ifdef VERBOSE
printf ("\nMPS rownames and types: <id> <type> <name>\n\n");
for (i = 0; i < mympsptr->numberrows; i++)
{
   printf ("%ld %c %s\n",
	   i,
	    mympsptr->relationalcodesvector[i],
             mympsptr->rownames[i]);
   fflush (stdout);
}/* end for i */
fflush (stdout);
#endif /* VERBOSE */

   /* set up the column hash table */
   if (!(initcolhashtable (&mycolhashtable, empcolctr)))
   {
      deleterowhashtable (&myrowhashtable);
      free (filedata);
      DeleteMPSstruct (&mympsptr);

      return (NULL);
   }/* end if (1) */

   /* set up the element hash table */

   /* maximum (2*empcolctr) elements */
   if (!(initelhashtable (&myelhashtable, (empcolctr+empcolctr))))
   {
      deleterowhashtable (&myrowhashtable);
      deletecolhashtable (&mycolhashtable);	
      free (filedata);
      DeleteMPSstruct (&mympsptr);

      return (NULL);
   }/* end if (1) */

   /* get the columns */

   if (!(GetCOLUMNS (&colptr,
	              &mympsptr,
	               &myelhashtable,
	                &mycolhashtable,
                         &myrowhashtable,
	                  rhsptr,
	                   &linenumber)))
   {
      deleterowhashtable (&myrowhashtable);
      deletecolhashtable (&mycolhashtable);	
      deleteelhashtable  (&myelhashtable);
      free (filedata);
      DeleteMPSstruct (&mympsptr);

      return (NULL);
   }/* end if (1) */

#ifdef VERBOSE
printf ("\nTotal columns = %ld\n", 
        mympsptr->numbercols);
printf ("MPS colnames : <id> <name>\n\n");
for (i = 0; i < mympsptr->numbercols; i++)
{
   printf ("%ld %s\n", 
	   i, 
	    mympsptr->colnames[i]);
   fflush (stdout);
   
}/* end for i */

printf ("\nTotal elements = %lu\n", 
        mympsptr->numberelements);
for (i = 0; i < mympsptr->numberelements+1 ; i++)
{
   if (i == mympsptr->numberelements)
   {
      printf ("\nspecial check of last item in kernel row data\n");
      fflush (stdout);
   }/* end if (1) */

   printf ("rowid = %ld\tvalue = %f\n",
           mympsptr->kerneldata_row[i].rowid,
            mympsptr->kerneldata_row[i].value);
   fflush (stdout);
   
}/* end for i */

printf ("\nXREF colptrs\n");
for (i = 0; i < (mympsptr->numbercols + 1); i++)
{
   printf ("cptr->rowid[%ld] = %ld\tvalue = %f\n", i,
           mympsptr->kerneldata_col[i]->rowid,
            mympsptr->kerneldata_col[i]->value);
   fflush (stdout);
   
}/* end for i */

printf ("\nrows = %lu\ncols = %lu\nelements = %lu\n\n",
        myrowhashtable.totalcontents,
         mycolhashtable.totalcontents,
          myelhashtable.totalcontents);

fflush (stdout);
#endif /* VERBOSE */

   /* we have no more need of the element hash table */
   deleteelhashtable  (&myelhashtable);


   if (ranptr != NULL)
   {
      if (!(GetRHS (&rhsptr,
	             &mympsptr,
	              &myrowhashtable,
                       ranptr,
	                &linenumber)))
      {
         free (filedata);
         DeleteMPSstruct (&mympsptr);
         deleterowhashtable (&myrowhashtable);
         deletecolhashtable (&mycolhashtable);

         return (NULL);
      }/* end if (2) */

      if (bndptr != NULL)
      {
         if (!(GetRANGES (&ranptr,
	                   &mympsptr,
	                    &myrowhashtable,
                             bndptr,
	                      &linenumber)))
         {
            free (filedata);
            DeleteMPSstruct (&mympsptr);
            deleterowhashtable (&myrowhashtable);
            deletecolhashtable (&mycolhashtable);

            return (NULL);
         }/* end if (3) */

         deleterowhashtable (&myrowhashtable);

         if (!(GetBOUNDS (&bndptr,
	                   &mympsptr,
	                    &mycolhashtable,
                             endptr,
	                      &linenumber)))
         {
            free (filedata);
            DeleteMPSstruct (&mympsptr);
            deletecolhashtable (&mycolhashtable);

            return (NULL);
         }/* end if (3) */

         free (filedata);
         deletecolhashtable (&mycolhashtable);

         return (mympsptr);

      }
      else
      {
         if (!(GetRANGES (&ranptr,
	                   &mympsptr,
	                    &myrowhashtable,
                             endptr,
	                      &linenumber)))
         {
            free (filedata);
            DeleteMPSstruct (&mympsptr);
            deleterowhashtable (&myrowhashtable);
            deletecolhashtable (&mycolhashtable);

            return (NULL);

         }/* end if (3) */

         free (filedata);
         deleterowhashtable (&myrowhashtable);
         deletecolhashtable (&mycolhashtable);

         return (mympsptr);

      }/* end if (2) */

   }
   else if (bndptr != NULL)
   {
      if (!(GetRHS(&rhsptr,
	            &mympsptr,
	             &myrowhashtable,
                      bndptr,
	               &linenumber)))
      {
         free (filedata);
         DeleteMPSstruct (&mympsptr);
         deleterowhashtable (&myrowhashtable);
         deletecolhashtable (&mycolhashtable);

         return (NULL);
      }/* end if (2) */

      deleterowhashtable (&myrowhashtable);

      if (!(GetBOUNDS(&bndptr,
	               &mympsptr,
	                &mycolhashtable,
                         endptr,
	                  &linenumber)))
      {
         free (filedata);
         DeleteMPSstruct (&mympsptr);
         deletecolhashtable (&mycolhashtable);

         return (NULL);
      }/* end if (2) */

      free (filedata);
      deletecolhashtable (&mycolhashtable);

      return (mympsptr);
      
   }
   else
   {
      if (!(GetRHS(&rhsptr,
	            &mympsptr,
	             &myrowhashtable,
                      endptr,
	               &linenumber)))
      {
         free (filedata);
         DeleteMPSstruct (&mympsptr);
         deleterowhashtable (&myrowhashtable);
         deletecolhashtable (&mycolhashtable);

         return (NULL);

      }/* end if (2) */

      free (filedata);
      deleterowhashtable (&myrowhashtable);
      deletecolhashtable (&mycolhashtable);

      return (mympsptr);

   }/* end if (1) */

}



#ifdef DRIVER
/*
 * *****************************************************************************
 * *                    START OF DISCARDABLE HARNESS (1)                       *
 * *****************************************************************************
 */


/* ************************** PRESENTATION CODE ***************************** */


/* ************************************************************************** */
/* *                                 MENU                                   * */
/* ************************************************************************** */


void menu ()
/* pre  : None.
 * post : Responsible for the display of menu functions to stdout.
 * NOTE : This should be a MACRO !
 */
{
   printf ("\nOPTIONS :\n---------\n\n");
   printf ("t = input time\n");
   printf ("N = Linear Programming problem name\n\n");
   printf ("r = row breakdown\t\tR = row total\n");
   printf ("c = column breakdown\t\tC = column total\n");
   printf ("E = total number of elements\tZ = total zero-valued elements");
   printf ("\n\nHISTOGRAMS\n\t1 = ranges of absolute values\n");
   printf ("\t2 = rows with x elements\n");
   printf ("\t3 = columns with x elements\n\n");
   printf ("RIM VECTORS\n\t4 = RHS vector\n");
   printf ("\t5 = RANGES vector\n");
   printf ("\t6 = UPPER BOUNDS vector\n");
   printf ("\t7 = LOWER BOUNDS vector\n\n");
   printf ("\tq = quit\n\n");
   printf ("Please enter a character option : ");
   fflush (stdout);
   

   return;

}



/* ************************************************************************** */
/* *                            ROWBREAKDOWN                                * */
/* ************************************************************************** */


void rowbreakdown (struct MPSstruct *mystruct, FILE *out)
/* pre  : We are given a valid pointer to an MPS structure, and a valid
 *        pointer to FILE.
 * post : Prints all row-related data into the specified file from the
 *        MPS structure.
 */
{
   register long i,
	         j,
	         limit,
	         limitc,
	         tally;
   struct rowdata *tempptr  = NULL,
	          **topcptr = NULL;
   char **name  = NULL,
        *rcvptr = NULL;


   fprintf (out, "BREAK DOWN OF ROWS\n\n");

   tempptr = mystruct->kerneldata_row;
   topcptr = ((mystruct->kerneldata_col)+1);
   name    = mystruct->rownames;
   rcvptr  = mystruct->relationalcodesvector;
   limit   = mystruct->numberrows;
   limitc  = mystruct->numbercols;


   /* for all the rows */
   for (i = 0; i < limit; i++)
   {
      /* give the details of the new row */
      fprintf (out,
               "\nrow = %s\tid = %ld\ttype = %c\n",
                *name,
	         i,
	          *rcvptr);
      fflush (out);

      tally = 0L;

      /* set up the column scanners */
      tempptr = mystruct->kerneldata_row;
      topcptr = ((mystruct->kerneldata_col)+1);

      /* scan every column kernel data pointer */
      for (j = 0; j < limitc; j++)
      {
         while (tempptr != *topcptr)
         {
            /* print for row matches to the current row */
            if ((tempptr->rowid) == i)
            {
#ifdef VERBOSE
               fprintf (out,
                        "column = %s\tid = %ld\tvalue = %f\n",
                         *((mystruct->colnames)+j),
                          j,
		           tempptr->value);
	       fflush (out);
#endif /* VERBOSE */

	       tally ++;
			
            }/* end if (1) */
	    
            tempptr++;
		
         }/* end while tempptr */
	 
         topcptr++;

      }/* end for j */

      /* give the number of non-zero elements */
      fprintf (out, 
	       "#non-zero elements = %ld\n", 
	        tally);
      fflush (out);

      /* get the next row name, and type */
      name++;
      rcvptr++;

   }/* end for i */

   fprintf (out, 
	    "\n\n");
   fflush (out);

   
   return;

}



/* ************************************************************************** */
/* *                            COLBREAKDOWN                                * */
/* ************************************************************************** */


void colbreakdown (struct MPSstruct *mystruct, FILE *out)
/* pre  : We are given a valid pointer to an MPS structure, and a valid
 *        pointer to FILE.
 * post : Prints all row-related data into the specified file from the
 *        MPS structure.
 */
{
   register long i,
	         limit,
	         tempval,
	         tally;
   struct rowdata *tempptr  = NULL,
	          **topcptr = NULL;
   char **name = NULL;


   fprintf (out, 
	    "BREAK DOWN OF COLUMNS\n\n");
   fflush (out);

   tempptr = mystruct->kerneldata_row;
   topcptr = ((mystruct->kerneldata_col)+1);
   name    = mystruct->colnames;
   limit   = mystruct->numbercols;

   /* for all the columns */
   for (i = 0; i < limit; i++)
   {
      tally = 0L;

      fprintf (out, 
	       "\ncolumn = %s\tid = %ld\n", 
	        *name, 
	         i);
      fflush (out);

      while (tempptr != *topcptr)
      {
         tempval = tempptr->rowid;
	 
#ifdef VERBOSE
         fprintf (out,
                  "row = %s\tid = %ld\ttype = %c\tvalue = %f\n",
                   *((mystruct->rownames)+tempval),
                    tempval,
                     *((mystruct->relationalcodesvector)+tempval),
                      tempptr->value);
	 fflush (out);
#endif /* VERBOSE */
	 
         tally++;

         tempptr++;
		
      }/* end while tempptr */
      
      topcptr++;
      name++;

      fprintf (out, 
	       "#non-zero elements = %ld\n", 
	        tally);
      fflush (out);
	
   }/* end for i */

   fprintf (out,
	    "\n\n");
   fflush (out);


   return;

}
			


/* ************************************************************************** */
/* *                              HISTOGRAM1                                * */
/* ************************************************************************** */


void histogram1 (struct MPSstruct *mystruct, FILE *out)
/* pre  : We are given a valid pointer to an MPS structure, and a valid
 *        pointer to FILE.
 * post : Prints a sidelong histogram detailing the absolute values
 *        of elements stated in the MPS file into the specified file,
 *        taking the data from the MPS structure.
 */
{
   register double currval;
   register unsigned long cnt,
	                  limit,
	                  i;
   unsigned long tallies[NUMBUCKETS1];
   register int j;
   struct rowdata *temp = NULL;


   /* simple initialisation of the tallies */
   for (j = 0; j < NUMBUCKETS1; j++)
   {
      *(tallies + j) = 0UL;
   }/* end for i */


   /* initialisations for the generation of the histogram */
   temp  = mystruct->kerneldata_row;
   limit = mystruct->numberelements;

   for (i = 0UL; i < limit; i++)
   {
      /* we want absolute values only */
      currval = temp->value;
      if (currval < 0.0L)
      {
         currval = -(currval);
      }/* end if (1) */
		
      for (j = 0; j < NUMBUCKETS1-1; j++)
      {
         if (currval <= (*(buckets+j)))
         {
            (*(tallies+j))++;
	    
            break;
	    
         }/* end if (1) */

         /* if we went through the whole for loop above */
         if (j == NUMBUCKETS1-1)
         {
            (*(tallies+j))++;
         }/* end if (1) */
				
      }/* end for j */

      temp++;

   }/* end for i */


   /* print the histogram from left to right across screen */
   fprintf (out, 
	    "\nHISTOGRAM #1 : RANGE OF ABSOLUTE VALUES\n");
   fprintf (out, 
	    "STAGES = *(10.0L)\n\n");
   fprintf (out, 
	    "MINIMUM <= %.1e\n", 
	     (*buckets));
   fflush (out);

   for (j = 0; j < NUMBUCKETS1; j++)
   {
      /* reuse currval, and avoid "double" multiplication */
      currval = *(buckets+j);
      fprintf (out, 
	       "%.1e|", 
	        currval);
      fflush (out);
		
      cnt = *(tallies+j);
      if (cnt != 0)
      {
         if (cnt > MAXSTARS)
         {
            for (i = 0UL; i < MAXSTARS; i++)
            {
               fputc ('*', out);
            }/* end for i */
	    
            fputc ('/', out);
	    
         }
         else
         {
            for (i = 0UL; i < cnt; i++)
            {
               fputc ('*', out);
            }/* end for i */
	    
         }/* end if (2) */
	 
         fprintf (out, 
	          "/ %lu", 
	           cnt);
	 fflush (out);
	 
      }/* end if (1) */
      
      fputc ('\n',
	     out);
      fflush (out);

   }/* end for j */
   
   fprintf (out, 
	    "MAXIMUM >= %.1e", 
	     *(buckets+(NUMBUCKETS1-1)));		
   fprintf (out, 
	    "\n\n");
   fflush (out);


   return;

}



/* ************************************************************************** */
/* *                            HISTOGRAM2                                  * */
/* ************************************************************************** */


void histogram2 (struct MPSstruct *mystruct, FILE *out)
/* pre  : We are given a valid pointer to an MPS structure, and a valid
 *        pointer to FILE.
 * post : Prints a sidelong histogram detailing the breakdown of rows with 
 *        'x' number of elements stated in the MPS file into the specified
 *        file, taking the data from the MPS structure.
 */
{
   register long cnt,
	         j,
	         limitr;
   register unsigned long i,
	                  numberels;
   long *tallyrows = NULL,
        *traverser = NULL,
        tallies[NUMBUCKETS2];
   struct rowdata *tempptr = NULL;


   /* initialisations */
   for (j = 0L; j < NUMBUCKETS2; j++)
   {
      *(tallies+j) = 0L;
   }/* end for */

   tempptr   = mystruct->kerneldata_row;
   numberels = mystruct->numberelements;
   limitr    = mystruct->numberrows;

   /* get the space for the row tallies and set to 0 */
   if ((tallyrows = (long *) calloc (limitr, sizeof(long))) == NULL)
   {
      fprintf (stderr, 
	       "\nUnable to get psace for row tallies\n\n");
      fflush (stderr);
      
      return;
   }/* end if (1) */

   /* obtain the first array of tallies in row order */
   for (i = 0UL; i < numberels; i++)
   {
      (*(tallyrows+(tempptr->rowid)))++;
      tempptr++;
   }/* end for i */

   /* obtain the histogram data scanning the rowwise tallies */
   traverser = tallyrows;
   
#ifdef __ANSI__C
   for (i = 0UL; i < (unsigned long)limitr; i++)
#else
   for (i = 0UL; i < limitr; i++)
#endif /* __ANSI__C */
   {
      /* check to see if we overshoot the array bounds for tallies */
      cnt = *traverser;
      if (cnt >= NUMBUCKETS2)
      {
         cnt = NUMBUCKETS2-1;
      }/* end if (1) */
      (*(tallies+cnt))++;

      traverser++;

   }/* end for i */


   /* print the histogram */
   fprintf (out, 
	    "\nHISTOGRAM #2 : Rows with x elements\n");
   fprintf (out, 
	    "granularity = 1\n\n");
   fprintf (out, 
	    "number of rows with elements MINIMUM <= 0\n");
   fflush (out);
   
   for (j = 0L; j < NUMBUCKETS2; j++)
   {
      fprintf (out, 
	       "%02ld|", 
	        j);
      fflush (out);
      
      cnt = *(tallies+j);
      if (cnt != 0L)
      {
         if (cnt > MAXSTARS)
         {
            for (i = 0UL; i < MAXSTARS; i++)
            {
               fputc ('*', out);
            }/* end for i */
	    
            fputc ('/', out);
	    
         }
         else
         {
#ifdef __ANSI__C
            for (i = 0UL; i < (unsigned long)cnt; i++)
#else
            for (i = 0UL; i < cnt; i++)
#endif /* __ANSI__C */	       
            {
               fputc ('*', out);
            }/* end for i */
	    
         }/* end if (2) */
	 
         fprintf (out, 
	          "/ %lu", 
	           cnt);
	 fflush (out);
	 
      }/* end if (1) */
      
      fputc ('\n', out);

   }/* end for j */
   
   fprintf (out,
	    "number of rows with elements MAXIMUM >= %ld",
             (NUMBUCKETS2-1));
   fprintf (out, 
	    "\n\n");
   fflush (out);

   /* cleanup allocated memory */
   free (tallyrows);

   
   return;

}



/* ************************************************************************** */
/* *                              HISTOGRAM3                                * */
/* ************************************************************************** */


void histogram3 (struct MPSstruct *mystruct, FILE *out)
/* pre  : We are given a valid pointer to an MPS structure, and a valid
 *        pointer to FILE.
 * post : Prints a sidelong histogram detailing the breakdown of columns with
 *        'x' number of elements stated in the MPS file into the specified 
 *        file, taking the data from the MPS structure.
 */
{
   register long cnt,
	         i,
	         j,
	         limit,
	         tempval,
	         tally;
   long tallies[NUMBUCKETS2];
   struct rowdata *tempptr  = NULL,
	          **topcptr = NULL;

	
   /* initialisations */
   tempptr = mystruct->kerneldata_row;
   topcptr = ((mystruct->kerneldata_col)+1);
   limit   = mystruct->numbercols;

   for (i = 0L; i < NUMBUCKETS2; i ++)
   {
      *(tallies + i) = 0L;
   }/* end for i */


   /* for all the columns */
   for (i = 0L; i < limit; i++)
   {
      tally = 0L;

      while (tempptr != *topcptr)
      {
         tempval = tempptr->rowid;
         tally++;
         tempptr++;
      }/* end while tempptr */


      /* produce the histogram data */

      /* check we do not overshoot end of array */
      if (tally >= NUMBUCKETS2)
      {
         tally = NUMBUCKETS2-1;
      }/* end if (1) */
      
      (*(tallies+tally))++;

      topcptr++;

   }/* end for i */

   /* print the histogram */
   fprintf (out, 
	    "\nHISTOGRAM #3 : Columns with x elements\n");
   fprintf (out, 
	    "granularity = 1\n\n");
   fprintf (out, 
	    "number of columns with elements MINIMUM <= 0\n");
   fflush (out);
   
   for (j = 0L; j < NUMBUCKETS2; j++)
   {
      fprintf (out, 
	       "%02ld|", 
	        j);
      fflush (out);
      
      cnt = *(tallies+j);
      if (cnt != 0L)
      {
         if (cnt > MAXSTARS)
         {
            for (i = 0L; i < MAXSTARS; i++)
            {
               fputc ('*', out);
            }/* end for i */
	    
            fputc ('/', out);
	    
         }
         else
         {
            for (i = 0L; i < cnt; i++)
            {
               fputc ('*', out);
            }/* end for i */
	    
         }/* end if (2) */
	 
         fprintf (out, 
	          "/ %lu", 
	           cnt);
	 fflush (out);
	 
      }/* end if (1) */
      
      fputc ('\n', out);

   }/* end for j */
   
   fprintf (out,
	    "number of columns with elements MAXIMUM >= %ld",
             (NUMBUCKETS2-1));
   fprintf (out, 
	    "\n\n");
   fflush (out);


   return;

}



/* ************************************************************************** */
/* *                             PRINTRHSVEC                                * */
/* ************************************************************************** */


void printRHSvec  (struct MPSstruct *mystruct, FILE *out)
/* pre  : We are given a valid pointer to an MPS structure, and a valid
 *        pointer to FILE.
 * post : Prints a sidelong histogram detailing the breakdown of columns with
 *        'x' number of elements stated in the MPS file into the specified
 *        file, taking the data from the MPS structure.
 */
{
   double *currval = NULL; /* used to scan entire vector */
   char **rnames = NULL; /* for printing out rownames */
   register long i,      /* general counter    */
                 limitr; /* the number of rows */


   /* find the number of rows */
   limitr = mystruct->numberrows;

   /* preliminary data */
   fprintf (out,
	    "\nRHS VECTOR DATA : rhs name = %s\n",
             mystruct->rhsname);
   fprintf (out, 
	    "\t<row name> = <value>\n\n");
   fflush (out);

   /* print out the vector data in dense format */
   for (i = 0L,
	 currval = mystruct->rhsvector,
          rnames = mystruct->rownames;
	   i < limitr;
            i++,
             currval++,
              rnames++)
   {
      fprintf (out, 
	       "\t%s = %f\n", 
	        *rnames, 
	         *currval);
      fflush (out);
   }/* end for i */
   
   fprintf (out, 
	    "\n\n");
   fflush (out);


   return;

}



/* ************************************************************************** */
/* *                               PRINTRNGVEC                              * */
/* ************************************************************************** */


void printRNGvec  (struct MPSstruct *mystruct, FILE *out)
/* pre  : We are given a valid pointer to an MPS structure, and a valid
 *        pointer to a FILE.
 * post : Prints out in dense format the entire contents of the RANGES 
 *        vector to the file from the MPS structure.
 */
{
   double *currval = NULL; /* used to scan entire vector */
   char **rnames = NULL; /* for printing out rownames */
   register long i,      /* general counter    */
                 limitr; /* the number of rows */


   /* find the number of rows */
   limitr = mystruct->numberrows;

   /* preliminary data */
   fprintf (out,
	    "\nRANGES VECTOR DATA : range name = %s\n",
             mystruct->ranname);
   fprintf (out, 
	    "\t<row name> = <value>\n\n");
   fflush (out);

   /* print out the vector data in dense format */
   for (i = 0L,
	 currval = mystruct->rangesvector,
          rnames = mystruct->rownames;
	   i < limitr;
            i++,
             currval++,
              rnames++)
   {
      fprintf (out, 
	       "\t%s = %f\n", 
	        *rnames, 
	         *currval);
      fflush (out);
   }/* end for i */
   
   fprintf (out, 
	    "\n\n");
   fflush (out);


   return;

}



/* ************************************************************************** */
/* *                             PRINTLBDVEC                                * */
/* ************************************************************************** */


void printLBDvec  (struct MPSstruct *mystruct, FILE *out)
/* pre  : We are given a valid pointer to an MPS structure, and a valid
 *        pointer to a FILE.
 * post : Prints out in dense format the entire contents of the LOWER BOUNDS
 *        VECTOR to the file from the MPS structure.
 */
{
   double *currval = NULL; /* used to scan entire vector */
   char **cnames = NULL; /* for printing out column names */
   register long i,      /* general counter       */
                 limitc; /* the number of columns */


   /* find the number of columns */
   limitc = mystruct->numbercols;

   /* preliminary data */
   fprintf (out,
            "\nLOWER BOUNDS VECTOR DATA : bounds name = %s\n",
             mystruct->bndname);
   fprintf (out, 
	    "\t<column name> = <value>\n\n");
   fflush (out);

   /* print out the vector data in dense format */
   for (i = 0L,
	 currval = mystruct->lowerboundsvector,
          cnames = mystruct->colnames;
	   i < limitc;
            i++,
             currval++,
              cnames++)
   {
      if (*currval == MINUS_INF)
      {
         fprintf (out, 
	          "\t%s = (- INFINITY)\n", 
	           *cnames);
      }
      else
      {
         fprintf (out, 
	          "\t%s = %.6e\n", 
	           *cnames, 
	            *currval);
      }/* end if (1) */

      fflush (out);
      
   }/* end for i */
   
   fprintf (out, 
	    "\n\n");
   fflush (out);


   return;

}



/* ************************************************************************** */
/* *                            PRINTUBDVEC                                 * */
/* ************************************************************************** */


void printUBDvec  (struct MPSstruct *mystruct, FILE *out)
/* pre  : We are given a valid pointer to an MPS structure, and a valid
 *        pointer to a FILE.
 * post : Prints out in dense format the entire contents of the UPPER BOUNDS
 *        VECTOR to the file from the MPS structure.
 */
{
   double *currval = NULL; /* used to scan entire vector */
   char **cnames = NULL; /* for printing out column names */
   register long i,      /* general counter       */
                 limitc; /* the number of columns */


   /* find the number of columns */
   limitc = mystruct->numbercols;

   /* preliminary data */
   fprintf (out,
	    "\nUPPER BOUNDS VECTOR DATA : bounds name = %s\n",
             mystruct->bndname);
   fprintf (out, 
	    "\t<column name> = <value>\n\n");
   fflush (out);

   /* print out the vector data in dense format */
   for (i = 0L,
	 currval = mystruct->upperboundsvector,
          cnames = mystruct->colnames;
	   i < limitc;
            i++,
             currval++,
	      cnames++)
   {
      if (*currval ==  PLUS_INF)
      {
         fprintf (out, 
	          "\t%s = (+ INFINITY)\n", 
	           *cnames);
      }
      else
      {
         fprintf (out, 
	          "\t%s = %.6e\n", 
	           *cnames, 
	            *currval);
      }/* end if (1) */
      
      fflush (out);

   }/* end for i */
   
   fprintf (out, 
	    "\n\n");
   fflush (out);


   return;

}



/* ******************* END OF PRESENTATION IMPLEMENTATIONS ****************** */

/*
 * *****************************************************************************
 * *                     END OF DISCARDABLE HARNESS (1)                        *
 * *****************************************************************************
 */
#endif /* DRIVER */



/* ************************************************************************** */
/* *                        END OF IMPLEMENTATIONS                          * */
/* ************************************************************************** */



#ifdef DRIVER
/*
 * *****************************************************************************
 * *                    START OF DISCARDABLE HARNESS (2)                       *
 * *****************************************************************************
 */


/* ************************************************************************** */
/* *                                MAIN                                    * */
/* ************************************************************************** */


int main (int argc, char **argv)
/* pre  : We are given at least 1, and no more than 3 arguments at the
 *        command line.
 * post : Responsible for the entire presentation harness.
 */
{
   struct MPSstruct *mydataptr = NULL;
   
   /* PRESENTATION */
   register int answer;
   register double totaltimetaken;
   FILE *outfile = stderr;
   char *iobuffer = NULL; /* a safety setting */
   /* END PRESENTATION */

   /* for timing functionality */
   TIMESTRUCTURE start,
	         end;


   if ((argc < 2) || (argc > 4))
   {
      fprintf (stderr, "\n\tREADMPS - presentation harness\n");
      fprintf (stderr,
               "\nformat = readmps MPSfile [output [n | NCREZrc1234567]]\n");
      fprintf (stderr, "\nOutput file options :\n\n");
      fprintf (stderr, "n = non-interactive, input time only\n\n");
      fprintf (stderr, "N = LP problem name\n");
      fprintf (stderr, "E = element count\tZ = zero value count\n");
      fprintf (stderr, "R = row count\t\tr = row breakdown\n");
      fprintf (stderr, "C = column count\tc = column breakdown\n");
      fprintf (stderr, "\n1 = histogram of absolute values\n");
      fprintf (stderr, "2 = histogram of elements in rows\n");
      fprintf (stderr, "3 = histogram of elements in columns\n\n");
      fprintf (stderr, "4 = RHS vector\n");
      fprintf (stderr, "5 = RANGES vector\n");
      fprintf (stderr, "6 = UPPER BOUNDS vector\n");
      fprintf (stderr, "7 = LOWER BOUNDS vector\n\n");
      fflush (stderr);
      
      fclose (outfile);
      
      exit (0);
   }/* end if (1) */

   if (argc > 2)
   {
      if ((outfile = fopen (*(argv+2), "wb")) == NULL)
      {
         fprintf (stderr,
                  "\nUnable to create output file %s\n\n",
                   *(argv+2));
	 fflush (stderr);
	 
         exit (0);
      }/* end if (2) */

      /*
       * if we can set a large I/O buffer for the result file then do so
       */
      if ((iobuffer = (char *) malloc (sizeof(char) * FILEBUFFSIZE)) != NULL)
      {
         setvbuf (outfile, iobuffer, _IOFBF, FILEBUFFSIZE);
      }/* end if (2) */

   }/* end if (1) */

   /* start the timing */
   gettimedata(&start);
	
   if ((mydataptr = GetMPSdata (*(argv+1))) == NULL)
   {
      fprintf (stderr, 
	       "\nUnable to make the MPS data structure\n\n");
      fflush (stderr);
      free (iobuffer);
      fclose (outfile);
      
      exit (0);
   }/* end if (1) */


   /* end the timing and extract the result */
   gettimedata(&end);

   totaltimetaken = gettotaltime(start,end);


   /* *** PRESENTATION HARNESS *** */

   fprintf (outfile, 
	    "\nFILE = %s\n", 
	     *(argv+1));

   if (argc == 4)
   {
      fprintf (outfile,
	       "\n***** INPUT TIME = %.3f seconds\n\n",
                totaltimetaken);

      if ((strchr (*(argv+3), 'N')) != NULL)
      {
         fprintf (outfile,
	          "\n***** LP name = %s\n\n",
                   mydataptr->lpname);
      }/* end if (2) */

      if ((strchr (*(argv+3), 'R')) != NULL)
      {
         fprintf (outfile,
	          "\n***** NUMBER OF ROWS = %ld\n\n",
                   mydataptr->numberrows);
      }/* end if (2) */

      if ((strchr (*(argv+3), 'r')) != NULL)
      {
         rowbreakdown (mydataptr, outfile);
      }/* end if (2) */
		
      if ((strchr (*(argv+3), 'C')) != NULL)
      {
         fprintf (outfile,
	          "\n***** NUMBER OF COLUMNS = %ld\n\n",
                   mydataptr->numbercols);
      }/* end if (2) */

      if ((strchr (*(argv+3), 'c')) != NULL)
      {
         colbreakdown (mydataptr, outfile);
      }/* end if (2) */
	 
      if ((strchr (*(argv+3), 'E')) != NULL)
      {
         fprintf (outfile,
	          "\n***** NUMBER OF ELEMENTS = %lu\n\n",
                   mydataptr->numberelements);
      }/* end if (2) */

      if ((strchr (*(argv+3), 'Z')) != NULL)
      {
         fprintf (outfile,
                  "\n***** NUMBER OF ELEMENTS VALUED AT ZERO = %lu\n\n",
                   mydataptr->numberzeroelements);
      }/* end if (2) */

      if ((strchr (*(argv+3), '1')) != NULL)
      {
         histogram1 (mydataptr, outfile);
      }/* end if (2) */

      if ((strchr (*(argv+3), '2')) != NULL)
      {
         histogram2 (mydataptr, outfile);
      }/* end if (2) */

      if ((strchr (*(argv+3), '3')) != NULL)
      {
         histogram3 (mydataptr, outfile);
      }/* end if (2) */

      if ((strchr (*(argv+3), '4')) != NULL)
      {
         printRHSvec (mydataptr, outfile);
      }/* end if (2) */

      if ((strchr (*(argv+3), '5')) != NULL)
      {
         printRNGvec (mydataptr, outfile);
      }/* end if (2) */

      if ((strchr (*(argv+3), '6')) != NULL)
      {
         printUBDvec (mydataptr, outfile);
      }/* end if (2) */

      if ((strchr (*(argv+3), '7')) != NULL)
      {
         printLBDvec (mydataptr, outfile);
      }/* end if (2) */

   }
   else /* interactive */
   {
      menu ();
      do {
            answer = getchar();
            getchar (); /* discard the carriage return */
            switch (answer)
            {
               case 't' :
                          fprintf (outfile,
                           "\n***** INPUT TIME = %.3f seconds\n\n",
                            totaltimetaken);
                          menu ();
                          break;

               case '1' :
		          histogram1 (mydataptr, outfile);
                          menu ();
                          break;

	       case '2' :
		          histogram2 (mydataptr, outfile);
                          menu ();
                          break;

	       case '3' : 
		          histogram3 (mydataptr, outfile);
                          menu ();
                          break;

	       case '4' :
		          printRHSvec (mydataptr, outfile);
                          menu ();
                          break;

	       case '5' :
		          printRNGvec (mydataptr, outfile);
                          menu ();
                          break;

	       case '6' :
		          printUBDvec (mydataptr, outfile);
                          menu ();
                          break;

	       case '7' :
		          printLBDvec (mydataptr, outfile);
                          menu ();
                          break;

	       case 'N' :
		          fprintf (outfile,
                           "\n***** LP name = %s\n\n",
                            mydataptr->lpname);
                          menu ();
                          break;

	       case 'r' :
		          rowbreakdown (mydataptr, outfile);
                          menu ();
                          break;

	       case 'R' :
		          fprintf (outfile,
			   "\n***** NUMBER OF ROWS = %ld\n\n",
                            mydataptr->numberrows);
                          menu ();
                          break;

	       case 'c' :
		          colbreakdown (mydataptr, outfile);
                          menu ();
                          break;

	       case 'C' :
		          fprintf (outfile,
                           "\n***** NUMBER OF COLUMNS = %ld\n\n",
                            mydataptr->numbercols);
                          menu ();
                          break;

	       case 'E' : fprintf (outfile,
		           "\n***** NUMBER OF ELEMENTS = %lu\n\n",
                            mydataptr->numberelements);
                          menu ();
                          break;

	       case 'Z' :
		          fprintf (outfile,
                           "\n***** NUMBER ZERO-VALUED ELEMENTS = %lu\n\n",
                            mydataptr->numberzeroelements);
                          menu ();
                          break;

	       case 'q' :
		          break;

	       default  :
		          fprintf (stderr, 
		           "\n\t*** Incorrect option ! ***\n");
                          menu ();
                          break;
			  
            }/* end switch */

         } while (answer != 'q'); /* end do .. while */

   }/* end if (1) */
   fflush (outfile);

   /* *** END PRESENTATION HARNESS *** */


   /* the MPSstruct is not subject to timing */
   DeleteMPSstruct (&mydataptr);

   /* even if (iobuffer == NULL), free will not cause a problem */
   free (iobuffer);
   fclose (outfile);


   return (0);

}



/*
 * *****************************************************************************
 * *                      END OF DISCARDABLE HARNESS (2)                       *
 * *****************************************************************************
 */
#endif /* DRIVER */



/* ********************************* END ************************************ */
