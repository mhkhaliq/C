/*
********************************************************************************
* FILE        : Hex.c
* DESCRIPTION : Prints a hexadecimal view of a file to stdout
* AUTHOR      : M H Khaliq
* LICENSE     : MIT
********************************************************************************
*/



/*
********************************************************************************
* SYSTEM HEADER FILES
********************************************************************************
*/

#include <stdio.h>
#include <stdlib.h>



/*
********************************************************************************
* DEFINITIONS
********************************************************************************
*/

#define IO_BUFFER_SIZE 65536
#define DATA_ARRAY_SIZE 16
#define ASCII_STRING_SIZE (DATA_ARRAY_SIZE + 1)
#define MIN_PRINT_ASCII 0x21
#define MAX_PRINT_ASCII 0x7E
#define UNPRINTABLE_CHAR '.'


/*
********************************************************************************
* MAIN
********************************************************************************
*/

int main ( int argc, char **argv ) {
  FILE *in = NULL;
  char ioBuf[IO_BUFFER_SIZE];
  unsigned char dataA[DATA_ARRAY_SIZE];
  unsigned char asciiS[ASCII_STRING_SIZE];
  int dataRead;
  unsigned long bytesOffset = 0UL;
  int i;
  unsigned char byte;

  setbuf ( stderr, NULL );
  if ( argc != 2 ) {
    fprintf ( stderr, "Usage: %s <file>\n", argv[0] );
    exit ( 1 );
  }/* end if 1 */
  if ( ( in = fopen( argv[1], "rb" ) ) == NULL ) {
    fprintf ( stderr, "Cannot open file '%s' for reading !\n", argv[1] );
    exit ( 2 );
  }/* end if 1 */
  if ( setbuf ( in, ioBuf, _IOFBF, IO_BUFFER_SIZE ) == - 1 ) {
    fprintf ( stderr, "Cannot set user-defined buffer to file '%s' !\n", 
        argv[1] );
  }/* end if 1 */
  setbuf ( stdout, NULL );
  asciiS[ASCII_STRING_SIZE - 1] = '\0';
  for ( ; ; ) {
    dataRead = fread ( dataA, sizeof( unsigned char ), DATA_ARRAY_SIZE, in );
    for ( i = 0; i < dataRead; ++i ) {
      byte = *(dataA + i);
      *(asciiS + i) = ( ( byte >= MIN_PRINT_ASCII ) && ( byte <= MAX_PRINT_ASCII ) ) ? byte : '.';
    }/* end for */
    switch ( dataRead ) {
      case ( 16 ):
        printf ( "%08lX  %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X   %s\n",
            bytesOffset,
            *dataA,
            *(dataA + 1),
            *(dataA + 2),
            *(dataA + 3),
            *(dataA + 4),
            *(dataA + 5),
            *(dataA + 6),
            *(dataA + 7),
            *(dataA + 8),
            *(dataA + 9),
            *(dataA + 10),
            *(dataA + 11),
            *(dataA + 12),
            *(dataA + 13),
            *(dataA + 14),
            *(dataA + 15),
            asciiS );
        break;
      case ( 15 ):
        *(asciiS + i ) = '\0';
        printf ( "%08lX  %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X      %s\n",
            bytesOffset,
            *dataA,
            *(dataA + 1),
            *(dataA + 2),
            *(dataA + 3),
            *(dataA + 4),
            *(dataA + 5),
            *(dataA + 6),
            *(dataA + 7),
            *(dataA + 8),
            *(dataA + 9),
            *(dataA + 10),
            *(dataA + 11),
            *(dataA + 12),
            *(dataA + 13),
            *(dataA + 14),
            asciiS );
        goto END;
      case ( 14 ):
        *(asciiS + i ) = '\0';
        printf ( "%08lX  %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X         %s\n",
            bytesOffset,
            *dataA,
            *(dataA + 1),
            *(dataA + 2),
            *(dataA + 3),
            *(dataA + 4),
            *(dataA + 5),
            *(dataA + 6),
            *(dataA + 7),
            *(dataA + 8),
            *(dataA + 9),
            *(dataA + 10),
            *(dataA + 11),
            *(dataA + 12),
            *(dataA + 13),
            asciiS );
        goto END;
      case ( 13 ):
        *(asciiS + i ) = '\0';
        printf ( "%08lX  %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X            %s\n",
            bytesOffset,
            *dataA,
            *(dataA + 1),
            *(dataA + 2),
            *(dataA + 3),
            *(dataA + 4),
            *(dataA + 5),
            *(dataA + 6),
            *(dataA + 7),
            *(dataA + 8),
            *(dataA + 9),
            *(dataA + 10),
            *(dataA + 11),
            *(dataA + 12),
            asciiS );
        goto END;
      case ( 12 ):
        *(asciiS + i ) = '\0';
        printf ( "%08lX  %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X               %s\n",
            bytesOffset,
            *dataA,
            *(dataA + 1),
            *(dataA + 2),
            *(dataA + 3),
            *(dataA + 4),
            *(dataA + 5),
            *(dataA + 6),
            *(dataA + 7),
            *(dataA + 8),
            *(dataA + 9),
            *(dataA + 10),
            *(dataA + 11),
            asciiS );
        goto END;
      case ( 11 ):
        *(asciiS + i ) = '\0';
        printf ( "%08lX  %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X                  %s\n",
            bytesOffset,
            *dataA,
            *(dataA + 1),
            *(dataA + 2),
            *(dataA + 3),
            *(dataA + 4),
            *(dataA + 5),
            *(dataA + 6),
            *(dataA + 7),
            *(dataA + 8),
            *(dataA + 9),
            *(dataA + 10),
            asciiS );
        goto END;
      case ( 10 ):
        *(asciiS + i ) = '\0';
        printf ( "%08lX  %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X                     %s\n",
            bytesOffset,
            *dataA,
            *(dataA + 1),
            *(dataA + 2),
            *(dataA + 3),
            *(dataA + 4),
            *(dataA + 5),
            *(dataA + 6),
            *(dataA + 7),
            *(dataA + 8),
            *(dataA + 9),
            asciiS );
        goto END;
      case ( 9 ):
        *(asciiS + i ) = '\0';
        printf ( "%08lX  %02X %02X %02X %02X %02X %02X %02X %02X %02X                        %s\n",
            bytesOffset,
            *dataA,
            *(dataA + 1),
            *(dataA + 2),
            *(dataA + 3),
            *(dataA + 4),
            *(dataA + 5),
            *(dataA + 6),
            *(dataA + 7),
            *(dataA + 8),
            asciiS );
        goto END;
      case ( 8 ):
        *(asciiS + i ) = '\0';
        printf ( "%08lX  %02X %02X %02X %02X %02X %02X %02X %02X                           %s\n",
            bytesOffset,
            *dataA,
            *(dataA + 1),
            *(dataA + 2),
            *(dataA + 3),
            *(dataA + 4),
            *(dataA + 5),
            *(dataA + 6),
            *(dataA + 7),
            asciiS );
        goto END;
      case ( 7 ):
        *(asciiS + i ) = '\0';
        printf ( "%08lX  %02X %02X %02X %02X %02X %02X %02X                              %s\n",
            bytesOffset,
            *dataA,
            *(dataA + 1),
            *(dataA + 2),
            *(dataA + 3),
            *(dataA + 4),
            *(dataA + 5),
            *(dataA + 6),
            asciiS );
        goto END;
      case ( 6 ):
        *(asciiS + i ) = '\0';
        printf ( "%08lX  %02X %02X %02X %02X %02X %02X                                 %s\n",
            bytesOffset,
            *dataA,
            *(dataA + 1),
            *(dataA + 2),
            *(dataA + 3),
            *(dataA + 4),
            *(dataA + 5),
            asciiS );
        goto END;
      case ( 5 ):
        *(asciiS + i ) = '\0';
        printf ( "%08lX  %02X %02X %02X %02X %02X                                    %s\n",
            bytesOffset,
            *dataA,
            *(dataA + 1),
            *(dataA + 2),
            *(dataA + 3),
            *(dataA + 4),
            asciiS );
        goto END;
      case ( 4 ):
        *(asciiS + i ) = '\0';
        printf ( "%08lX  %02X %02X %02X %02X                                       %s\n",
            bytesOffset,
            *dataA,
            *(dataA + 1),
            *(dataA + 2),
            *(dataA + 3),
            asciiS );
        goto END;
      case ( 3 ):
        *(asciiS + i ) = '\0';
        printf ( "%08lX  %02X %02X %02X                                          %s\n",
            bytesOffset,
            *dataA,
            *(dataA + 1),
            *(dataA + 2),
            asciiS );
        goto END;
      case ( 2 ):
        *(asciiS + i ) = '\0';
        printf ( "%08lX  %02X %02X                                             %s\n",
            bytesOffset,
            *dataA,
            *(dataA + 1),
            asciiS );
        goto END;
      case ( 1 ):
        *(asciiS + i ) = '\0';
        printf ( "%08lX  %02X                                                %s\n",
            bytesOffset,
            *dataA,
            asciiS );
        goto END;
      case ( 0 ):
        goto END;
    }/* end switch */
    bytesOffset += dataRead;
  }/* end for */

END:
  if ( fclose ( in ) != 0 ) {
    fprintf ( stderr, "Cannot close file '%s' !\n", argv[1] );
  }/* end if 1 */


  return ( 0 );
}


/*
 * Local Variables:
 * mode:c
 * End:
 */
