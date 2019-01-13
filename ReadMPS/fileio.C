/*
********************************************************************************
* FILE        : fileio.C
* DESCRIPTION : Implementation file for fileio.h
* AUTHOR      : M H Khaliq
* LICENSE     : MIT
********************************************************************************
*/



/*
 *******************************************************************************
 *			      HEADER FILES				       *
 *******************************************************************************
*/



/* ************************************************************************** */
/* *			  ANSI C HEADER FILES			            * */
/* ************************************************************************** */

#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#ifdef __ANSI__C
#include <string.h>
#else
#include <sys/stat.h>
#endif /* __ANSI__C */



/* ************************************************************************** */
/* *			   LOCAL HEADER FILES			            * */
/* ************************************************************************** */


#include "fileio.h"



/*
 *******************************************************************************
 *			    LOCAL PROTOTYPES			               *
 *******************************************************************************
*/

#ifdef __ANSI__C
extern void deletefrontnode (struct datanode **);
/* pre  : We are given a valid pointer to datanode pointer.
 * post : Deletes the first data node found from the pointer
 *	  to the linked list of datanodes given as argument.
 * NOTE : There must be at least 1 node to delete.
 */
#endif /* __ANSI__C */



/*
 *******************************************************************************
 *			       IMPLEMENTATIONS				       *
 *******************************************************************************
*/



/* ************************************************************************** */
/* *			       DELETEFRONTNODE				    * */
/* ************************************************************************** */


#ifdef __ANSI__C
void deletefrontnode (struct datanode **thelist)
/* pre  : We are given a valid pointer to datanode pointer.
 * post : Deletes the first data node found from the pointer
 *	  to the linked list of datanodes given as argument.
 * NOTE : There must be at least 1 node to delete.
 */
{
   struct datanode *tempptr; /* local pointer for "free" */

   
   /* get the first node */
   tempptr = *thelist;

   /* set the list to have the subsequent node as first node */
   *thelist = tempptr->next;

   /* release the old front node from memory */
   free (tempptr);


   return;

}
#endif /* __ANSI__C */



/* ************************************************************************** */
/* *			      READFILEINTOMEMORY			    * */
/* ************************************************************************** */


char * Readfileintomemory (char *filename, unsigned long *size)
/* pre  : We are given the full name and path of the input file, and a
 *	  valid pointer to an unsigned long.
 * post : Returns NULL on any error condition, otherwise returns a
 *	  character pointer to an array of characters holding the whole
 *	  of the data of the file located by "filename", and size points
 *	  the value of the size of the file.
 */
{
#ifdef __ANSI__C
   struct datanode *list     = NULL, /* temporary linked list of all data */
                   *currnode = NULL, /* track the tail of list for speed  */
                   *tempptr  = NULL; /* general purpose node pointer      */
   register int i,             /* general purpose counter          */
                nodetally = 1, /* count of nodes, at least 1       */
                readchars;     /* checks chars read in each "read" */
   char *dst = NULL; /* helps assign node data to final array */
#else
   struct stat statbuf; /* holds "stat" data for "filename" */
#endif /* __ANSI__C */
   register unsigned long filesize = 0UL; /* gives size of final array */
   char *filebuffer = NULL; /* the pointer to the returned array */
   FILE *infile = NULL; /* the file isolated by "filename" */
   
   
#ifdef UNIX
   /* in one operation set the size of the file data array */
   if ((stat (filename, &statbuf)) == BADSTAT)
   {
      
      return (NULL);
      
   }/* end if (1) */
   
   filesize = (unsigned long)statbuf.st_size;
#endif /* UNIX */

   
   /* open the file */
   if ((infile = fopen (filename, "rb")) == NULL)
   {
      fprintf (stderr, 
	       "\nUnable to open file %s\n\n", 
	        filename);
      fflush (stderr);
      
      return (NULL);
      
   }/* end if (1) */

   
#ifdef __ANSI__C
   /* initialise the list of datanode with one node */
   if ((tempptr = 
	 (struct datanode *) malloc (sizeof(struct datanode))) == NULL)
   {
      fprintf (stderr, 
	       "\nUnable to set up data list for file\n\n");
      fflush (stderr);
      fclose (infile);
      
      return (NULL);
      
   }/* end if (1) */
   list     = tempptr;
   currnode = tempptr;

   
   /*
    * This 'for' loop terminates once 'readchars' is less than 
    * 'READBUFFERSIZE'.  The way it is coded is to capitalise on the
    * code above which removes the need to constantly test for an
    * end of file condition per iteration of the 'for' loop
    */
   for ( ; ; )
   {
      /* break loop once we read fewer than READBUFFERSIZE chars */
      if ((readchars = 
	    fread (currnode->data, sizeof(char),READBUFFERSIZE, infile)) 
             < READBUFFERSIZE)
      {
         filesize += readchars;
	 
         break;
	 
      }/* end if (1) */

      filesize += READBUFFERSIZE;

      /* get a new node, since we use them up on each pass */
      if ((tempptr = 
	    (struct datanode *) malloc (sizeof(struct datanode))) == NULL)
      {
         fprintf (stderr, 
	          "\nUnable to set up data list for file\n\n");
	 fflush (stderr);
         fclose (infile);

         /* safely delete all nodes so far */
         for (i = 0; i < nodetally; i++)
         {
            deletefrontnode (&list);
         }/* end for i */
	 
         return (NULL);

      }/* end if (1) */

      /* update the status of the linked list */
      currnode->next = tempptr;
      currnode       = currnode->next;
      nodetally++;

   }/* end for ;; */
#endif /* __ANSI__C */

   
   /* allocate the return buffer */	
   if ((filebuffer = 
	 (char *) malloc ((filesize) * sizeof (char))) == NULL)
   {
      fprintf (stderr,
                "\nUnable to allocate %lu bytes of memory for file buffer\n\n",
                 filesize);
      fflush (stderr);
      fclose (infile);
      
#ifdef __ANSI__C
      for (i = 0; i < nodetally; i++)
      {
         deletefrontnode (&list);
      }/* end for i */
#endif /* __ANSI__C */
		
      return (NULL);

   }/* end if (1) */

   
#ifdef __ANSI__C
   /* go through the linked list and assign the nodes in order to array */
   for (i = 1,
	 currnode = list,
	  dst = filebuffer; 
	   i < nodetally; 
	    i++,
	     currnode = currnode->next,
	      dst += READBUFFERSIZE)
   {
      memcpy (dst, currnode->data, READBUFFERSIZE);  
   }/* end for i */

		
   /* read the last data */
   memcpy (dst, currnode->data, readchars);
#else
   /* in one operation assign the whole of the file to the array */
   fread (filebuffer, sizeof (char), filesize, infile);
#endif /* __ANSI__C */

   
   /* set the value of size for the calling function */
   *size = filesize;

   
   /* cleanup operations */
   fclose (infile);
#ifdef __ANSI__C
   for (i = 0; i < nodetally; i++)
   {
      deletefrontnode (&list);
   }/* end for i */
#endif /* __ANSI__C */


   return (filebuffer);

}



/* ************************************************************************** */
/* *			      END OF IMPLEMENTATIONS                        * */
/* ************************************************************************** */




/* *********************************** END ********************************** */
