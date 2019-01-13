/*
********************************************************************************
* FILE        : hash.h
* DESCRIPTION : Header file for all hash table functionality
* AUTHOR      : M H Khaliq
* LICENSE     : MIT
********************************************************************************
*/



#ifndef __HASH_H
#define __HASH_H



/*
 *******************************************************************************
 *                                  DEFINES                                    *
 *******************************************************************************
*/


/*
 * define the size of the array of <row/el/col>structures held in a
 * <row/el/col>node
 *
 * NOTE : In case of memory problems, reduce this number to 2.
 *        If saturation of the hash tables is expected ( > 8.0e+6 items )
 *        then increase the number to 8 or more
 *
 */

#define NODE_EL_NUMBER 2


/*
 * define values specific to the hash table initialisations
 *
 * NOTE : it is EXTREMELY INADVISABLE to alter the 4 defines below 
 *		=====================
 *
 */

#define MINSIZE  500L
#define MAXSIZE  500000L

#define MINPRIME 1009L
#define MAXPRIME 1000003L



/*
 *******************************************************************************
 *                              STRUCTURES                                     *
 *******************************************************************************
*/


/* FOR ROWS */

struct rowstruct {
   long row_id,     /* unique identifiers for each row, starting at 0 */
        linenumber; /* the line identifying the location of the row   */
   char *rowname;   /* pointer to the string naming the row           */
};

struct rowhashnode {
   int                numelements;              /* number of items in node */
   struct rowstruct   elements[NODE_EL_NUMBER]; /* array of node data      */
   struct rowhashnode *nextnode;                /* link to next node       */
};

typedef struct {
   long               arraysize;           /* tally of the number of nodes   */
   unsigned long      totalcontents;       /* number of items in whole table */
   struct rowhashnode **rowhashtablearray; /* pointer to node pointers array */
} rowhashtable;


/* FOR COLUMNS */

struct colstruct {
   long col_id,     /* unique identifiers for each column, start at 0  */
        linenumber; /* the line identifying the location of the column */
   char *colname;   /* pointer to string naming the column             */
};

/* developed on the same principle as rowhashnode */
struct colhashnode {
   int                numelements;
   struct colstruct   elements[NODE_EL_NUMBER];
   struct colhashnode *nextnode;
};

/* developed on the same principle as rowhashtable */
typedef struct {
   long               arraysize;
   unsigned long      totalcontents;
   struct colhashnode **colhashtablearray;
} colhashtable;


/* for elements */

struct elementstruct {
   long linenumber;     /* line location of the element */
   char *columnname,    /* name of the column           */
        *assoc_rowname; /* name of the row              */
};

/* developed on the same principle as rowhashnode */
struct elhashnode {
   int                  numelements;
   struct elementstruct elements[NODE_EL_NUMBER];
   struct elhashnode    *nextnode;
};

/* developed on the same principle as rowhashtable */
typedef struct {
   long              arraysize;
   unsigned long     totalcontents;
   struct elhashnode **elhashtablearray;
} elhashtable;


/*
 *******************************************************************************
 *                                 PROTOTYPES                                  *
 *******************************************************************************
*/


#ifdef __C__PLUS__PLUS
extern "C" {
#else
extern 
#endif /* __C__PLUS__PLUS */
unsigned long hashfunction (const unsigned char *,
                             const unsigned char *);
/* pre  : We are given 2 valid pointers to strings.
 * post : Returns a calculated hash value for the string(s).
 *
 * NOTE : If only one string is to be processed, set the second
 *        argument to "NULL".
 *
 */


/* specific to row hash tables */
#ifndef __C__PLUS__PLUS
extern 
#endif
int initrowhashtable (rowhashtable *, long);
/* pre  : We are given a valid rowhashtable pointer, and a long
 *        integer.
 * post : Creates a row hash table set to best size given from
 *        the long argument.
 *
 *        Returns 0 on failure, 1 on success.
 */


#ifndef __C__PLUS__PLUS
extern 
#endif
int addrowtotable (rowhashtable *, long, long, char *);
/* pre  : We are given a valid pointer to a rowhashtable, 2 long
 *        integers, and a char pointer.
 * post : Adds the row name addressed by the 'char *' to the
 *        row hash table, assigning that row its row id and
 *        line number respectively from the 2 long values.
 *        If the same row already exists in the table we trap this
 *        critical error.
 *
 *        Returns 0 on failure, 1 on success.
 */


#ifndef __C__PLUS__PLUS
extern 
#endif
long findrow (rowhashtable *, char *);
/* pre  : We are given a valid pointer to a rowhashtable, and a 
 *        character pointer to a string.
 * post : Returns (-1) if the string pointed to is not in
 *        the row hash table, else the unique id of the row
 *        associated with the provided row name.
 */


#ifndef __C__PLUS__PLUS
extern 
#endif
void deleterowhashtable (rowhashtable *);
/* pre  : We are given a pointer to a valid rowhashtable.
 * post : Safely frees all allocated memory locked
 *        in the row hash table before deleting the table
 *        itself.
 */



/* specific to column hash tables */

#ifndef __C__PLUS__PLUS
extern 
#endif
int initcolhashtable (colhashtable *, long);
/* pre  : We are given a valid colhashtable pointer, and a
 *        long integer.
 * post : Creates a column hash table of a size best corresponding
 *        to the value of the given long argument.
 *
 *        Returns 1 on success, 0 on failure.
 */


#ifndef __C__PLUS__PLUS
extern 
#endif
int addcoltotable (colhashtable *, long, long, char *);
/* pre  : We are given a pointer to a valid colhashtable, 2 long
 *        integers, and a character pointer pointing to a string.
 * post : Adds an entry assigned the name given by the character
 *        pointer to the column hash table providing it does
 *        not exist already.  That entry is uniquely identified by
 *        the column id and the line number that are given by the
 *        2 long integers respectively.
 *
 *        Returns 0 on failure, 1 on success.
 */


#ifndef __C__PLUS__PLUS
extern 
#endif
long findcol (colhashtable *, char *);
/* pre  : We are given a pointer to a valid colhashtable, and a
 *        character pointer pointing to a string.
 * post : Returns (-1) if the string given via the character pointer
 *        does not occur in the table, else it returns the unique
 *        column id of that column.
 */


#ifndef __C__PLUS__PLUS
extern 
#endif
void deletecolhashtable (colhashtable *);
/* pre  : We are given a pointer to a valid colhashtable.
 * post : Safely frees all memory locked in the nodes of the
 *        addressed colhashtable before deleting the table itself.
 */



/* specific to element hash tables */

#ifndef __C__PLUS__PLUS
extern 
#endif
int initelhashtable (elhashtable *, long);
/* pre  : We are given a valid pointer to an elhashtable, and
 *        a long integer.
 * post : Creates an elhashtable of size decided by the value
 *        given by the long argument.
 *
 *        Returns 1 on success, 0 on failure.
 */


#ifndef __C__PLUS__PLUS
extern 
#endif
int addeltotable (elhashtable *, long, char *, char *);
/* pre  : We are given a pointer to a valid elhashtable, a long
 *        integer, and 2 char pointers to strings.
 * post : If the element uniqely identified by the row and the
 *        column names respectively passed as the 'char *'
 *        arguments does not already exist in the element hash
 *        table, that element is added to the table, with the
 *        associated line number (the long argument) attached to it.
 *
 *        Returns 0 on failure, and 1 on success.
 */


#ifndef __C__PLUS__PLUS
extern 
#endif
void deleteelhashtable (elhashtable *);
/* pre  : We are given a pointer to a valid elhashtable.
 * post : Safely frees all memory locked in the element hash
 *        table nodes before removing the table itself.
 */


#ifdef __C__PLUS__PLUS
}
#endif /* __C__PLUS__PLUS */

/* ************************************************************************** */
/* *                           END OF PROTOTYPES                            * */
/* ************************************************************************** */


#endif /* __HASH_H */


/* ********************************** END *********************************** */
