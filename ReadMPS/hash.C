/*
********************************************************************************
* FILE        : hash.C
* DESCRIPTION : Implementation file for hash.h
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



/* ************************************************************************** */
/* *                          LOCAL HEADER FILES                            * */
/* ************************************************************************** */


#include "hash.h"



/*
 *******************************************************************************
 *                             LOCAL CONSTANTS                                 *
 *******************************************************************************
*/


/*
 * constant serving as the lookup table for getting the right
 * size for a hash table, values are the primes at increments of
 * 1000
 */

const long primehashtablesizes[1000] = {
	1009L,		2003L,		3001L,		4001L,		
	5003L,		6007L,		7001L,		8009L,		
	9001L,		10007L,		11003L,		12007L,		
	13001L,		14009L,		15013L,		16001L,		
	17011L,		18013L,		19001L,		20011L,		
	21001L,		22003L,		23003L,		24001L,		
	25013L,		26003L,		27011L,		28001L,		
	29009L,		30011L,		31013L,		32003L,		
	33013L,		34019L,		35023L,		36007L,		
	37003L,		38011L,		39019L,		40009L,		
	41011L,		42013L,		43003L,		44017L,		
	45007L,		46021L,		47017L,		48017L,		
	49003L,		50021L,		51001L,		52009L,		
	53003L,		54001L,		55001L,		56003L,		
	57037L,		58013L,		59009L,		60013L,		
	61001L,		62003L,		63029L,		64007L,		
	65003L,		66029L,		67003L,		68023L,		
	69001L,		70001L,		71011L,		72019L,		
	73009L,		74017L,		75011L,		76001L,		
	77003L,		78007L,		79031L,		80021L,		
	81001L,		82003L,		83003L,		84011L,		
	85009L,		86011L,		87011L,		88001L,		
	89003L,		90001L,		91009L,		92003L,		
	93001L,		94007L,		95003L,		96001L,		
	97001L,		98009L,		99013L,		100003L,	
	101009L,	102001L,	103001L,	104003L,	
	105019L,	106013L,	107021L,	108007L,	
	109001L,	110017L,	111029L,	112019L,	
	113011L,	114001L,	115001L,	116009L,	
	117017L,	118033L,	119027L,	120011L,	
	121001L,	122011L,	123001L,	124001L,	
	125003L,	126001L,	127031L,	128021L,	
	129001L,	130003L,	131009L,	132001L,	
	133013L,	134033L,	135007L,	136013L,	
	137029L,	138007L,	139021L,	140009L,	
	141023L,	142007L,	143053L,	144013L,	
	145007L,	146009L,	147011L,	148013L,	
	149011L,	150001L,	151007L,	152003L,	
	153001L,	154001L,	155003L,	156007L,	
	157007L,	158003L,	159013L,	160001L,	
	161009L,	162007L,	163003L,	164011L,	
	165001L,	166013L,	167009L,	168013L,	
	169003L,	170003L,	171007L,	172001L,	
	173021L,	174007L,	175003L,	176017L,	
	177007L,	178001L,	179021L,	180001L,	
	181001L,	182009L,	183023L,	184003L,	
	185021L,	186007L,	187003L,	188011L,	
	189011L,	190027L,	191021L,	192007L,	
	193003L,	194003L,	195023L,	196003L,	
	197003L,	198013L,	199021L,	200003L,	
	201007L,	202001L,	203011L,	204007L,	
	205019L,	206009L,	207013L,	208001L,	
	209021L,	210011L,	211007L,	212029L,	
	213019L,	214003L,	215051L,	216023L,	
	217001L,	218003L,	219001L,	220009L,	
	221021L,	222007L,	223007L,	224011L,	
	225023L,	226001L,	227011L,	228013L,	
	229003L,	230003L,	231001L,	232003L,	
	233021L,	234007L,	235003L,	236017L,	
	237011L,	238001L,	239017L,	240007L,	
	241013L,	242009L,	243011L,	244003L,	
	245023L,	246011L,	247001L,	248021L,	
	249017L,	250007L,	251003L,	252001L,	
	253003L,	254003L,	255007L,	256019L,	
	257003L,	258019L,	259001L,	260003L,	
	261011L,	262007L,	263009L,	264007L,	
	265003L,	266003L,	267017L,	268003L,	
	269023L,	270001L,	271003L,	272003L,	
	273001L,	274007L,	275003L,	276007L,	
	277003L,	278017L,	279001L,	280001L,	
	281023L,	282001L,	283001L,	284003L,	
	285007L,	286001L,	287003L,	288007L,	
	289001L,	290011L,	291007L,	292021L,	
	293021L,	294001L,	295007L,	296011L,	
	297019L,	298013L,	299011L,	300007L,	
	301013L,	302009L,	303007L,	304009L,	
	305017L,	306011L,	307009L,	308003L,	
	309007L,	310019L,	311009L,	312007L,	
	313003L,	314003L,	315011L,	316003L,	
	317003L,	318001L,	319001L,	320009L,	
	321007L,	322001L,	323003L,	324011L,	
	325001L,	326023L,	327001L,	328007L,	
	329009L,	330017L,	331013L,	332009L,	
	333019L,	334021L,	335009L,	336029L,	
	337013L,	338017L,	339023L,	340007L,	
	341017L,	342037L,	343019L,	344017L,	
	345001L,	346013L,	347003L,	348001L,	
	349007L,	350003L,	351011L,	352007L,	
	353011L,	354001L,	355007L,	356023L,	
	357031L,	358031L,	359003L,	360007L,	
	361001L,	362003L,	363017L,	364027L,	
	365003L,	366001L,	367001L,	368021L,	
	369007L,	370003L,	371027L,	372013L,	
	373003L,	374009L,	375017L,	376001L,	
	377011L,	378011L,	379007L,	380041L,	
	381001L,	382001L,	383011L,	384001L,	
	385001L,	386017L,	387007L,	388009L,	
	389003L,	390001L,	391009L,	392011L,	
	393007L,	394007L,	395023L,	396001L,	
	397013L,	398011L,	399023L,	400009L,	
	401017L,	402023L,	403001L,	404009L,	
	405001L,	406013L,	407023L,	408011L,	
	409007L,	410009L,	411001L,	412001L,	
	413009L,	414013L,	415013L,	416011L,	
	417007L,	418007L,	419047L,	420001L,	
	421009L,	422029L,	423001L,	424001L,	
	425003L,	426007L,	427001L,	428003L,	
	429007L,	430007L,	431017L,	432001L,	
	433003L,	434009L,	435037L,	436003L,	
	437011L,	438001L,	439007L,	440009L,	
	441011L,	442003L,	443011L,	444001L,	
	445001L,	446003L,	447001L,	448003L,	
	449003L,	450001L,	451013L,	452009L,	
	453023L,	454009L,	455003L,	456007L,	
	457001L,	458009L,	459007L,	460013L,	
	461009L,	462013L,	463003L,	464003L,	
	465007L,	466009L,	467003L,	468001L,	
	469009L,	470021L,	471007L,	472019L,	
	473009L,	474017L,	475037L,	476009L,	
	477011L,	478001L,	479023L,	480013L,	
	481001L,	482017L,	483017L,	484019L,	
	485021L,	486023L,	487007L,	488003L,	
	489001L,	490001L,	491003L,	492007L,	
	493001L,	494023L,	495017L,	496007L,	
	497011L,	498013L,	499021L,	500009L,	
	501001L,	502001L,	503003L,	504001L,	
	505027L,	506047L,	507029L,	508009L,	
	509023L,	510007L,	511001L,	512009L,	
	513001L,	514001L,	515041L,	516017L,	
	517003L,	518017L,	519011L,	520019L,	
	521009L,	522017L,	523007L,	524047L,	
	525001L,	526027L,	527053L,	528001L,	
	529003L,	530017L,	531017L,	532001L,	
	533003L,	534007L,	535013L,	536017L,	
	537001L,	538001L,	539003L,	540041L,	
	541001L,	542021L,	543017L,	544001L,	
	545023L,	546001L,	547007L,	548003L,	
	549001L,	550007L,	551003L,	552001L,	
	553013L,	554003L,	555029L,	556007L,	
	557017L,	558007L,	559001L,	560017L,	
	561019L,	562007L,	563009L,	564013L,	
	565013L,	566011L,	567011L,	568019L,	
	569003L,	570001L,	571001L,	572023L,	
	573007L,	574003L,	575009L,	576001L,	
	577007L,	578021L,	579011L,	580001L,	
	581029L,	582011L,	583007L,	584011L,	
	585019L,	586009L,	587017L,	588011L,	
	589021L,	590021L,	591023L,	592019L,	
	593003L,	594023L,	595003L,	596009L,	
	597031L,	598007L,	599003L,	600011L,	
	601021L,	602029L,	603011L,	604001L,	
	605009L,	606017L,	607001L,	608011L,	
	609043L,	610031L,	611011L,	612011L,	
	613007L,	614041L,	615019L,	616003L,	
	617011L,	618029L,	619007L,	620003L,	
	621007L,	622009L,	623003L,	624007L,	
	625007L,	626009L,	627017L,	628013L,	
	629003L,	630017L,	631003L,	632029L,	
	633001L,	634003L,	635003L,	636017L,	
	637001L,	638023L,	639007L,	640007L,	
	641051L,	642011L,	643009L,	644009L,	
	645011L,	646003L,	647011L,	648007L,	
	649001L,	650011L,	651017L,	652019L,	
	653033L,	654001L,	655001L,	656023L,	
	657017L,	658001L,	659011L,	660001L,	
	661009L,	662003L,	663001L,	664009L,	
	665011L,	666013L,	667013L,	668009L,	
	669023L,	670001L,	671003L,	672019L,	
	673019L,	674017L,	675029L,	676007L,	
	677011L,	678023L,	679033L,	680003L,	
	681001L,	682001L,	683003L,	684007L,	
	685001L,	686003L,	687007L,	688003L,	
	689021L,	690037L,	691001L,	692009L,	
	693019L,	694019L,	695003L,	696019L,	
	697009L,	698017L,	699001L,	700001L,	
	701009L,	702007L,	703013L,	704003L,	
	705011L,	706001L,	707011L,	708007L,	
	709043L,	710009L,	711001L,	712007L,	
	713021L,	714029L,	715019L,	716003L,	
	717001L,	718007L,	719009L,	720007L,	
	721003L,	722011L,	723029L,	724001L,	
	725009L,	726007L,	727003L,	728003L,	
	729019L,	730003L,	731033L,	732023L,	
	733003L,	734003L,	735001L,	736007L,	
	737017L,	738011L,	739003L,	740011L,	
	741001L,	742009L,	743027L,	744019L,	
	745001L,	746017L,	747037L,	748003L,	
	749011L,	750019L,	751001L,	752009L,	
	753001L,	754003L,	755009L,	756011L,	
	757019L,	758003L,	759001L,	760007L,	
	761003L,	762001L,	763001L,	764003L,	
	765007L,	766021L,	767017L,	768013L,	
	769003L,	770027L,	771011L,	772001L,	
	773021L,	774001L,	775007L,	776003L,	
	777001L,	778013L,	779003L,	780029L,	
	781003L,	782003L,	783007L,	784009L,	
	785003L,	786001L,	787021L,	788009L,	
	789001L,	790003L,	791003L,	792023L,	
	793043L,	794009L,	795001L,	796001L,	
	797003L,	798023L,	799003L,	800011L,	
	801001L,	802007L,	803027L,	804007L,	
	805019L,	806009L,	807011L,	808019L,	
	809023L,	810013L,	811037L,	812011L,	
	813013L,	814003L,	815029L,	816019L,	
	817013L,	818011L,	819001L,	820037L,	
	821003L,	822007L,	823001L,	824017L,	
	825001L,	826019L,	827009L,	828007L,	
	829001L,	830003L,	831023L,	832003L,	
	833009L,	834007L,	835001L,	836047L,	
	837017L,	838003L,	839009L,	840023L,	
	841003L,	842003L,	843043L,	844001L,	
	845003L,	846037L,	847009L,	848017L,	
	849019L,	850009L,	851009L,	852011L,	
	853007L,	854017L,	855031L,	856021L,	
	857009L,	858001L,	859003L,	860009L,	
	861001L,	862009L,	863003L,	864007L,	
	865001L,	866003L,	867001L,	868019L,	
	869017L,	870007L,	871001L,	872017L,	
	873017L,	874001L,	875011L,	876011L,	
	877003L,	878011L,	879001L,	880001L,	
	881003L,	882017L,	883013L,	884003L,	
	885023L,	886007L,	887017L,	888001L,	
	889001L,	890003L,	891001L,	892019L,	
	893003L,	894011L,	895003L,	896003L,	
	897007L,	898013L,	899009L,	900001L,	
	901007L,	902009L,	903017L,	904019L,	
	905011L,	906007L,	907019L,	908003L,	
	909019L,	910003L,	911003L,	912007L,	
	913013L,	914021L,	915007L,	916031L,	
	917003L,	918011L,	919013L,	920011L,	
	921001L,	922021L,	923017L,	924019L,	
	925019L,	926017L,	927001L,	928001L,	
	929003L,	930011L,	931003L,	932003L,	
	933001L,	934001L,	935003L,	936007L,	
	937003L,	938017L,	939007L,	940001L,	
	941009L,	942013L,	943003L,	944003L,	
	945031L,	946003L,	947027L,	948007L,	
	949001L,	950009L,	951001L,	952001L,	
	953023L,	954001L,	955037L,	956003L,	
	957031L,	958007L,	959009L,	960017L,	
	961003L,	962009L,	963019L,	964009L,	
	965023L,	966011L,	967003L,	968003L,	
	969011L,	970027L,	971021L,	972001L,	
	973001L,	974003L,	975011L,	976009L,	
	977021L,	978001L,	979001L,	980027L,	
	981011L,	982021L,	983063L,	984007L,	
	985003L,	986023L,	987013L,	988007L,	
	989011L,	990001L,	991009L,	992011L,	
	993001L,	994013L,	995009L,	996001L,	
	997001L,	998009L,	999007L,	1000003L
};



/*
 *******************************************************************************
 *                              LOCAL PROTOTYPES                               *
 *******************************************************************************
*/


extern void deleterowhashtablenode (rowhashtable *, long);
/* pre  : We are given a pointer to a valid rowhashtable,
 *        and a long integer.
 * post : Removes the node at the head of the row hash table
 *        location indexed by the long argument.
 */


extern void deletecolhashtablenode (colhashtable *, long);
/* pre  : We are given a pointer to a valid colhashtable,
 *        and a long integer.
 * post : Removes the node at the head of the column hash table
 *        location indexed with the long argument.
 */


extern void deleteelhashtablenode  (elhashtable *, long);
/* pre  : We are given a pointer to a valid elhashtable,
 *        and a long integer.
 * post : Removes the node at the head of the element hash table
 *        location indexed with the long argument.
 */



/*
 *******************************************************************************
 *                             IMPLEMENTATIONS                                 *
 *******************************************************************************
*/


/* ************************************************************************** */
/* *                            HASHFUNCTION                                * */
/* ************************************************************************** */


unsigned long hashfunction (const unsigned char *data1,
                             const unsigned char *data2)
/* pre  : We are given a 2 valid pointers to unsigned character strings.
 * post : (Adapted from "Practical Algorithms for Programmers", p 71)
 *        Returns an unsigned long hash value.
 */
{
   register unsigned long h = 0UL, 
	                  g; /* to generate hash value */

   
   /* process the first argument */
   while (*data1 != '\0')
   {
      h = (h << 4) + *data1;
      data1++;
      if ((g = (h & 0xF0000000)))
      {
         h ^= g >> 24; /* see C operator precedence */
      }/* end if (1) */

      h &= ~g;
      
   }/* end while *data1 */

   /* process the second argument */
   while (*data2 != '\0')
   {
      h = (h << 4) + *data2;
      data2++;
      if ((g = (h & 0xF0000000)))
      {
         h ^= g >> 24; /* see C operator precedence */
      }/* end if (1) */

      h &= ~g;
      
   }/* end while *data2 */


   return (h);

}


/* specific to row hash tables */


/* ************************************************************************** */
/* *                           INITROWHASHTABLE                             * */
/* ************************************************************************** */


int initrowhashtable (rowhashtable *thetable, long tablesz)
/* pre  : We are given a valid pointer to a row hash table, and
 *        an empirical value for the size of the table.
 * post : Returns 0 if the table could not be allocated in memory.
 *        Returns 1 if successful, and the size of the hash table's array
 *        of nodes is set to the best value corresponding to 'tablesz'.
 */
{
   struct rowhashnode **tempptr; /* general intermediate pointer */
   register long truesize,            /* best table size value  */
                 i,                   /* general counter        */
                 index,               /* to scan the primes     */
                 tablesize = tablesz; /* temporary table "size" */


   /*
    * return the front or back end of the prime table if tablesz < (MINSIZE)
    * or tablesz > (MAXSIZE) respectively,
    * otherwise, index is ((tablesz / 1000)-1) if tablesz is a multiple
    * of 1000, or (tablesz / 1000) otherwise
    *
    * faster than both sequential and a binary search
    */
   if (tablesize < MINSIZE)
   {
      truesize = MINPRIME;
   }
   else if (tablesize > MAXSIZE)
   {
      truesize = MAXPRIME;
   }
   else
   {
      /* additions are faster than multiplication */
      tablesize += tablesize; /* double the value */
      index = (tablesize / 1000L) - 1L;

      /* if the size was over a multiple of 1000, increment index */
      if ((tablesize % 1000L) > 0L)
      {
         index++;
      }/* end if (2) */

      /* get the prime value of the size of the table */
      truesize = *(primehashtablesizes+index);
      
   }/* end if (1) */


   if ((thetable->rowhashtablearray = (struct rowhashnode **) 
	 malloc (truesize * sizeof (struct rowhashnode *))) == NULL)
   {
      fprintf (stderr, 
	       "\nUnable to intialise row hash table\n\n");
      fflush (stderr);
      
      return (0);
      
   }/* end if (1) */

   
   thetable->arraysize     = truesize;
   thetable->totalcontents = 0UL;
   
   for (i = 0L,
	 tempptr = thetable->rowhashtablearray; 
	  i < truesize; 
	   i++,
	    tempptr++)
   {
      *tempptr = NULL;
   }/* end for i */


   return (1);

}



/* ************************************************************************** */
/* *                           ADDROWTOTABLE                                * */
/* ************************************************************************** */


int addrowtotable (rowhashtable *thetable, 
                    long rowid, 
                     long linenum,
                      char *therowname)
/* pre  : We are given a valid pointer to a rowhashtable, 2 long
 *        integers, and a char pointer.
 * post : Adds the row name 'therowname' via the 'char *' to the
 *        row hash table, assigning that row its row id and
 *        line number respectively from the 2 long values.
 *        If the same row already exists in the table we trap this
 *        critical error.
 *
 *        Returns 0 on failure, 1 on success.
 */
{
   char	*chptr1 = NULL, 
        *chptr2 = NULL; /* for comparing strings */
   struct rowhashnode *tempptr    = NULL, /* general purpose pointer   */
                      *bufptr     = NULL, /*             "             */
                      *pursuitptr = NULL; /* used in external chaining */
   struct rowstruct *currstruct = NULL; /* the node we are at */
   register unsigned long hashval; /* our given hash value */
   register int nodeitems, /* the number of nodes */
                i;         /* general counter     */
   register unsigned short samestring; /* flag set on matching names */


   hashval = 
    (hashfunction((const unsigned char *)therowname, 
	          (const unsigned char *)"")) % (thetable->arraysize);

   bufptr = *((thetable->rowhashtablearray) + hashval);

   if (bufptr == NULL)
   {
      /* need to add a new rowhashnode */
      if ((tempptr = 
	    (struct rowhashnode *) malloc (sizeof(struct rowhashnode))) == NULL)
      {
	 fprintf (stderr, 
	          "\nUnable to allocate memory for row hash node\n\n");
	 fflush (stderr);
	 
         return (0);
	 
      }/* end if (2) */

      tempptr->numelements   = 1;
      tempptr->nextnode      = NULL;
      currstruct             = tempptr->elements;
      currstruct->row_id     = rowid;
      currstruct->linenumber = linenum;
      currstruct->rowname    = therowname;

      *(thetable->rowhashtablearray+hashval) = tempptr;

   }
   else
   {
      /* collision situation */
      tempptr = bufptr;
      while (tempptr != NULL)
      {
         nodeitems = tempptr->numelements;
         for (i = 0; i < nodeitems; i++)
         {
            chptr1 = therowname;
            chptr2 = ((tempptr->elements)+i)->rowname;

            /* own version of "strcmp" function */
            samestring = 1;
            while ((*chptr1 != '\0') || (*chptr2 != '\0'))
            {
               if (*chptr1 != *chptr2)
               {
                  samestring = 0;
		  
                  break;
		  
               }/* end if (2) */
               chptr1++;
               chptr2++;

            }/* end while *chptr1 || *chptr2 */

            if (samestring)
            {
               fprintf (stderr,
		        "\nrow called \"%s\" at line #%ld\n",
                         therowname,
                          ((tempptr->elements)+i)->linenumber);		
               fprintf (stderr, 
		        "duplicate row at line #%ld\n", 
		         linenum);
	       fflush (stderr);

               return (0);
	       
            }/* end if (2) */

         }/* end for i */

         pursuitptr = tempptr;
         tempptr    = tempptr->nextnode;

      }/* end while tempptr */


      /*
       * collision not due to duplicate row name so add new row
       */

      tempptr = pursuitptr; /* move to last node */

      i = tempptr->numelements;
      if (i < NODE_EL_NUMBER)
      {
         currstruct             = (tempptr->elements+i);
         currstruct->row_id     = rowid;
         currstruct->linenumber = linenum;
         currstruct->rowname    = therowname;

         /* increment the local node's element count */
         tempptr->numelements++;

      }
      else /* we need a new row hash node */
      {
         if ((bufptr = (struct rowhashnode *) 
	       malloc (sizeof(struct rowhashnode))) == NULL)
         {
            fprintf (stderr,
                     "\nUnable to allocate memory for row hash node\n\n");
	    fflush (stderr);
	    
            return (0);
	    
         }/* end if (3) */
		
          bufptr->numelements    = 1;
          bufptr->nextnode       = NULL;
          currstruct             = bufptr->elements;
          currstruct->row_id     = rowid;
          currstruct->linenumber = linenum;
          currstruct->rowname    = therowname;

          tempptr->nextnode = bufptr;

      }/* end if (2) */

   }/* end if (1) */


   /*
    *  data successfully added, we increment the total number of items
    *  in the whole hash table
    */

   thetable->totalcontents++;


   return (1);

}



/* ************************************************************************** */
/* *                               FINDROW                                  * */
/* ************************************************************************** */


long findrow (rowhashtable *thetable, char *name)
/* pre  : We are given a valid pointer to a rowhashtable, and a 
 *        character pointer to a string.
 * post : Returns (-1) if the string pointed to is not in
 *        the row hash table, else the unique id of the row
 *        associated with the provided row name.
 */
{
   char *chptr1 = NULL,
        *chptr2 = NULL;
   struct rowhashnode *tempptr = NULL;
   register unsigned long hashval;
   register int nodeitems,
                i;
   register unsigned short samestring;


   /* we find the location of the "name" in the hash table */
   hashval = (hashfunction((const unsigned char *)name,
              (const unsigned char *)"")) % (thetable->arraysize);

   /* setup for a sequential search */
   tempptr = *((thetable->rowhashtablearray) + hashval);

   while (tempptr != NULL)
   {
      nodeitems = tempptr->numelements;
      for (i = 0; i < nodeitems; i++)
      {
         chptr1 = name;
	 chptr2 = ((tempptr->elements)+i)->rowname;

         /* own version of "strcmp" function */
         samestring = 1;
	 while ((*chptr1 != '\0') || (*chptr2 != '\0'))
         {
            if (*chptr1 != *chptr2)
            {
               samestring = 0;
	       
               break;
	       
            }/* end if (1) */
            chptr1++;
            chptr2++;

         }/* end while *chptr1 || *chptr2 */

         if (samestring)
         {
            return (((tempptr->elements)+i)->row_id);
         }/* end if (1) */

      }/* end for i */

      /* prepare to scan another node */
      tempptr = tempptr->nextnode;

   }/* end while tempptr */


   /* we found no matching row */
   return (-1);

}


/* ************************************************************************** */
/* *                       DELETEROWHASHTABLENODE                           * */
/* ************************************************************************** */


void deleterowhashtablenode (rowhashtable *thetable, long location)
/* pre  : We are given a pointer to a valid rowhashtable,
 *        and a long integer.
 * post : Removes the node at the head of the row hash table
 *        location indexed by the long argument.
 */
{
   struct rowhashnode *tempptr = NULL;


   tempptr = *((thetable->rowhashtablearray) + location);
   *((thetable->rowhashtablearray) + location) = tempptr->nextnode;

   free (tempptr);


   return;

}


/* ************************************************************************** */
/* *                          DELETEROWHASHTABLE                            * */
/* ************************************************************************** */


void deleterowhashtable (rowhashtable *thetable)
/* pre  : We are given a pointer to a valid rowhashtable.
 * post : Safely frees all allocated memory locked in the row hash table 
 *        before deleting the table itself.
 */
{
   struct rowhashnode **tempptr = NULL;
   register long i, 
                 tablesize;


   tablesize = thetable->arraysize;
   tempptr   = thetable->rowhashtablearray;
   for (i = 0L; i < tablesize; i++)
   {
      /* until the linked list of nodes is empty, delete each node */
      while (*(tempptr + i) != NULL)
      {
         deleterowhashtablenode (thetable, i);
      }/* end while *(tempptr + i) */
      
   }/* end for i */

   free (tempptr);


   thetable->arraysize         = 0L;
   thetable->totalcontents     = 0UL;
   thetable->rowhashtablearray = NULL;


   return;

}



/* specific to col hash tables */


/* ************************************************************************** */
/* *                            INITCOLHASHTABLE                            * */
/* ************************************************************************** */


int initcolhashtable (colhashtable *thetable, long tablesz)
/* pre  : We are given a valid colhashtable pointer, and a
 *        long integer.
 * post : Creates a column hash table of a size best corresponding
 *        to the value of the given long argument.
 *
 *        Returns 1 on success, 0 on failure.
 */
{
   struct colhashnode **tempptr = NULL;
   register long truesize, 
                 i, 
	         index, 
	         tablesize = tablesz;


   /*
    * return the front or back end of the prime table if 
    * tablesz < (MINSIZE) or tablesz > (MAXSIZE) respectively,
    * otherwise, index is ((tablesz / 1000)-1) if tablesz is a multiple
    * of 1000, or (tablesz / 1000) otherwise
    *
    * faster than both sequential and a binary search
    */
   if (tablesize < MINSIZE)
   {
      truesize = MINPRIME;
   }
   else if (tablesize > MAXSIZE)
   {
      truesize = MAXPRIME;
   }
   else
   {
      /* additions are faster than multiplication */
      tablesize += tablesize; /* double the value */
      index = (tablesize / 1000L) - 1L;

      /* if the size was over a multiple of 1000, increment index */
      if ((tablesize % 1000L) > 0L)
      {
         index++;
      }/* end if (2) */

      /* get the prime value of the size of the table */
      truesize = *(primehashtablesizes+index);
      
   }/* end if (1) */   


   if ((thetable->colhashtablearray =
	 (struct colhashnode **) malloc 
	   (truesize * sizeof (struct colhashnode *))) == NULL)
   {
      fprintf (stderr, 
	       "\nUnable to intialise col hash table\n\n");
      fflush (stderr);
      
      return (0);
      
   }/* end if (1) */


   thetable->arraysize     = truesize;
   thetable->totalcontents = 0UL;
 
   for (i = 0L,
	 tempptr = thetable->colhashtablearray; 
	  i < truesize; 
	   i++,
	    tempptr++)
   {
      *tempptr = NULL;
   }/* end for i */


   return (1);

}



/* ************************************************************************** */
/* *                           ADDCOLTOTABLE                                * */
/* ************************************************************************** */


int addcoltotable (colhashtable *thetable, 
                    long colid,
                     long linenum,
                      char *thecolname)
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
{
   char *chptr1 = NULL,
        *chptr2 = NULL;
   struct colhashnode *tempptr    = NULL,
	              *bufptr     = NULL,
	              *pursuitptr = NULL;
   struct colstruct *currstruct = NULL;
   register unsigned long hashval;
   register int nodeitems,
	        i;
   register unsigned short samestring;


   hashval =  (hashfunction((const unsigned char *)thecolname,
               (const unsigned char *)"")) % (thetable->arraysize);

   bufptr = *((thetable->colhashtablearray) + hashval);

   if (bufptr == NULL)
   {
      /* need to add a new colhashnode */
      if ((tempptr = (struct colhashnode *)
	    malloc (sizeof(struct colhashnode))) == NULL)
      {
         fprintf (stderr, 
	          "\nUnable to allocate memory for col hash node\n\n");
	 fflush (stderr);
	 
         return (0);
	 
      }/* end if (2) */

      tempptr->numelements   = 1;
      tempptr->nextnode      = NULL;
      currstruct             = tempptr->elements;
      currstruct->col_id     = colid;
      currstruct->linenumber = linenum;
      currstruct->colname    = thecolname;

      *(thetable->colhashtablearray+hashval) = tempptr;

   }
   else
   {
      /* collision situation */
      tempptr = bufptr;
      while (tempptr != NULL)
      {
         nodeitems = tempptr->numelements;
	 for (i = 0; i < nodeitems; i++)
         {
            chptr1 = thecolname;
            chptr2 = ((tempptr->elements)+i)->colname;

            /* own version of "strcmp" function */
            samestring = 1;
            while ((*chptr1 != '\0') || (*chptr2 != '\0'))
            {
               if (*chptr1 != *chptr2)
               {
                  samestring = 0;
		  
                  break;
			
               }/* end if (2) */
	       
               chptr1++;
               chptr2++;

            }/* end while *chptr1 || *chptr2 */

            if (samestring)
            {
               fprintf (stderr,
		        "\ncolumn called \"%s\" at line #%ld\n",
                         thecolname,
                          ((tempptr->elements)+i)->linenumber);		
               fprintf (stderr,
		        "duplicate column at line #%ld\n",
		         linenum);
               fprintf (stderr, 
		        "columns must be declared in 1 segment\n");
	       fflush (stderr);

               return (0);
	       
            }/* end if (2) */

         }/* end for i */

	 pursuitptr = tempptr;
         tempptr    = tempptr->nextnode;

      }/* end while tempptr */


      /*
       * collision not due to duplicate col name so add new col
       */

      tempptr = pursuitptr; /* move to last node */

      i = tempptr->numelements;
      if (i < NODE_EL_NUMBER)
      {
         currstruct             = (tempptr->elements+i);
         currstruct->col_id     = colid;
         currstruct->linenumber = linenum;
         currstruct->colname    = thecolname;

         /* increment the local node's element count */
         tempptr->numelements++;

      }
      else /* we need a new col hash node */
      {
         if ((bufptr = (struct colhashnode *) 
	       malloc (sizeof(struct colhashnode))) == NULL)
         {
            fprintf (stderr,
                     "\nUnable to allocate memory for col hash node\n\n");
	    fflush (stderr);
	    
            return (0);
	    
         }/* end if (3) */
		
         bufptr->numelements    = 1;
         bufptr->nextnode       = NULL;
         currstruct             = bufptr->elements;
         currstruct->col_id     = colid;
         currstruct->linenumber = linenum;
         currstruct->colname    = thecolname;

         tempptr->nextnode = bufptr;

      }/* end if (2) */

   }/* end if (1) */


   /*
    *  data successfully added, we increment the total number of items
    *  in the whole hash table
    */

   thetable->totalcontents++;


   return (1);

}



/* ************************************************************************** */
/* *                              FINDCOL                                   * */
/* ************************************************************************** */


long findcol (colhashtable *thetable, char *name)
/* pre  : We are given a pointer to a valid colhashtable, and a
 *        character pointer pointing to a string.
 * post : Returns (-1) if the string given via the character pointer
 *        does not occur in the table, else it returns the unique
 *        column id of that column.
 */
{
   char *chptr1 = NULL,
        *chptr2 = NULL;
   struct colhashnode *tempptr = NULL;
   register unsigned long hashval;
   register int nodeitems,
	        i;
   register unsigned short samestring;


   /* we find the location of the "name" in the hash table */
   hashval = (hashfunction((const unsigned char *)name,
		(const unsigned char *)"")) % (thetable->arraysize);

   /* setup for a sequential search */
   tempptr = *((thetable->colhashtablearray) + hashval);

   while (tempptr != NULL)
   {
      nodeitems = tempptr->numelements;
      for (i = 0; i < nodeitems; i++)
      {
         chptr1 = name;
         chptr2 = ((tempptr->elements)+i)->colname;

         /* own version of "strcmp" function */
         samestring = 1;
         while ((*chptr1 != '\0') || (*chptr2 != '\0'))
         {
            if (*chptr1 != *chptr2)
            {
               samestring = 0;
	       
               break;
	       
            }/* end if (1) */
	    
            chptr1++;
            chptr2++;

         }/* end while *chptr1 || *chptr2 */

         if (samestring)
         {
            return (((tempptr->elements)+i)->col_id);
         }/* end if (1) */

      }/* end for i */

      /* prepare to scan another node */
      tempptr = tempptr->nextnode;

   }/* end while tempptr */


   /* we found no matching col */
   return (-1);

}



/* ************************************************************************** */
/* *                       DELETECOLHASHTABLENODE                           * */
/* ************************************************************************** */


void deletecolhashtablenode (colhashtable *thetable, long location)
/* pre  : We are given a pointer to a valid colhashtable,
 *        and a long integer.
 * post : Removes the node at the head of the column hash table
 *        location indexed with the long argument.
 */
{
   struct colhashnode *tempptr = NULL;


   tempptr = *((thetable->colhashtablearray) + location);
   *((thetable->colhashtablearray) + location) = tempptr->nextnode;

   free (tempptr);


   return;

}


/* ************************************************************************** */
/* *                           DELETECOLHASHTABLE                           * */
/* ************************************************************************** */


void deletecolhashtable (colhashtable *thetable)
/* pre  : We are given a pointer to a valid colhashtable.
 * post : Safely frees all memory locked in the nodes of the
 *        addressed colhashtable before deleting the table itself.
 */
{
   struct colhashnode **tempptr = NULL;
   register long i,
	         tablesize;


   tablesize = thetable->arraysize;
   tempptr = thetable->colhashtablearray;
   for (i = 0L; i < tablesize; i++)
   {
      /* until the linked list of nodes is empty, delete each node */
      while (*(tempptr + i) != NULL)
      {
         deletecolhashtablenode (thetable, i);
      }/* end while *(tempptr + i) */
      
   }/* end for i */

   free (tempptr);


   thetable->arraysize         = 0L;
   thetable->totalcontents     = 0UL;
   thetable->colhashtablearray = NULL;


   return;

}



/* specific to element hash tables */


/* ************************************************************************** */
/* *                            INITELHASHTABLE                             * */
/* ************************************************************************** */


int initelhashtable (elhashtable *thetable, long tablesz)
/* pre  : We are given a valid pointer to an elhashtable, and
 *        a long integer.
 * post : Creates an elhashtable of size decided by the value
 *        given by the long argument.
 *
 *        Returns 1 on success, 0 on failure.
 */
{
   struct elhashnode **tempptr = NULL;
   register long truesize, 
	         i, 
	         index, 
	         tablesize = tablesz;


   /*
    * return the front or back end of the prime table if tablesz < (MINSIZE)
    * or tablesz > (MAXSIZE) respectively,
    * otherwise, index is ((tablesz / 1000)-1) if tablesz is a multiple
    * of 1000, or (tablesz / 1000) otherwise
    *
    * faster than both sequential and a binary search
    */
   if (tablesize < MINSIZE)
   {
      truesize = MINPRIME;
   }
   else if (tablesize > MAXSIZE)
   {
      truesize = MAXPRIME;
   }
   else
   {
      /* additions are faster than multiplication */
      tablesize += tablesize; /* double the value */
      index = (tablesize / 1000L) - 1L;

      /* if the size was over a multiple of 1000, increment index */
      if ((tablesize % 1000L) > 0L)
      {
         index++;
      }/* end if (2) */

      /* get the prime value of the size of the table */
      truesize = *(primehashtablesizes+index);
      
   }/* end if (1) */


   if ((thetable->elhashtablearray = 
	 (struct elhashnode **) malloc 
	    (truesize * sizeof (struct elhashnode *))) == NULL)
   {
      fprintf (stderr, 
	       "\nUnable to intialise el hash table\n\n");
      fflush (stderr);
      
      return (0);
      
   }/* end if (1) */


   thetable->arraysize     = truesize;
   thetable->totalcontents = 0UL;
   
   for (i = 0L,
	 tempptr = thetable->elhashtablearray;
	  i < truesize; 
	   i++,
	    tempptr++)
   {
      *tempptr = NULL;
   }/* end for i */


   return (1);

}



/* ************************************************************************** */
/* *                            ADDELTOTABLE                                * */
/* ************************************************************************** */


int addeltotable (elhashtable *thetable,
                   long linenum,
                    char *colname,
                     char *rowname)
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
{
   char *chptr1 = NULL,
        *chptr2 = NULL;
   struct elhashnode *tempptr    = NULL,
	             *bufptr     = NULL,
	             *pursuitptr = NULL;
   struct elementstruct *currstruct = NULL;
   register unsigned long hashval;
   register int nodeitems,
	        i;
   register unsigned short samestring;


   hashval = (hashfunction((const unsigned char *)colname,
               (const unsigned char *)rowname)) % (thetable->arraysize);

   bufptr = *((thetable->elhashtablearray) + hashval);

   if (bufptr == NULL)
   {
      /* need to add a new elhashnode */
      if ((tempptr =
	    (struct elhashnode *) malloc (sizeof(struct elhashnode))) == NULL)
      {
         fprintf (stderr, 
	          "\nUnable to allocate memory for el hash node\n\n");
	 fflush (stderr);
	 
         return (0);
	 
      }/* end if (2) */

      tempptr->numelements      = 1;
      tempptr->nextnode         = NULL;
      currstruct                = tempptr->elements;
      currstruct->linenumber    = linenum;
      currstruct->columnname    = colname;
      currstruct->assoc_rowname = rowname;

      *(thetable->elhashtablearray+hashval) = tempptr;

   }
   else
   {
      /* collision situation */
      tempptr = bufptr;
      while (tempptr != NULL)
      {
         nodeitems = tempptr->numelements;
         for (i = 0; i < nodeitems; i++)
         {
            chptr1 = colname;
            chptr2 = ((tempptr->elements)+i)->columnname;

            /* own version of "strcmp" function */
            samestring = 1;
            while ((*chptr1 != '\0') || (*chptr2 != '\0'))
            {
               if (*chptr1 != *chptr2)
               {
                  samestring = 0;
		  
                  break;
			
               }/* end if (2) */
	       
               chptr1++;
               chptr2++;

            }/* end while *chptr1 || *chptr2 */

            /* column names match, now check the rownames */
            if (samestring)
            {
               chptr1 = rowname;
               chptr2 = ((tempptr->elements)+i)->assoc_rowname;

               /* own version of "strcmp" function */
               samestring = 1;
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

               if (samestring)
               {
                  fprintf (stderr,
                   "Element (\"%s\", \"%s\") at line #%ld\n",
                    ((tempptr->elements)+i)->columnname,
                     ((tempptr->elements)+i)->assoc_rowname,
                      ((tempptr->elements)+i)->linenumber);
                  fprintf (stderr,
                           "Duplicate element (\"%s\", \"%s\") at line #%ld\n",
                            colname,
			     rowname,
			      linenum);
		  fflush (stderr);

                  return (0);

               }/* end if (3) */

            }/* end if (2) */

         }/* end for i */

	 pursuitptr = tempptr;
         tempptr    = tempptr->nextnode;

      }/* end while tempptr */


      /*
       * collision not due to duplicate (col,row) name so add new el
       */

      tempptr = pursuitptr; /* move to last node */

      i = tempptr->numelements;
      if (i < NODE_EL_NUMBER)
      {
         currstruct                = (tempptr->elements+i);
         currstruct->linenumber    = linenum;
         currstruct->columnname    = colname;
         currstruct->assoc_rowname = rowname;

         /* increment the local node's element count */
         tempptr->numelements++;

      }
      else /* we need a new el hash node */
      {
         if ((bufptr = 
	       (struct elhashnode *) malloc 
	          (sizeof(struct elhashnode))) == NULL)
         {
            fprintf (stderr,
                     "\nUnable to allocate memory for el hash node\n\n");
	    fflush (stderr);
	    
            return (0);
	    
         }/* end if (3) */
		
         bufptr->numelements       = 1;
         bufptr->nextnode          = NULL;
         currstruct                = bufptr->elements;
         currstruct->linenumber    = linenum;
         currstruct->columnname    = colname;
         currstruct->assoc_rowname = rowname;

         tempptr->nextnode = bufptr;

      }/* end if (2) */

   }/* end if (1) */


   /*
    *  data successfully added, we increment the total number of items
    *  in the whole hash table
    */

   thetable->totalcontents++;


   return (1);

}



/* ************************************************************************** */
/* *                        DELETEELHASHTABLENODE                           * */
/* ************************************************************************** */


void deleteelhashtablenode (elhashtable *thetable, long location)
/* pre  : We are given a pointer to a valid elhashtable,
 *        and a long integer.
 * post : Removes the node at the head of the element hash table
 *        location indexed with the long argument.
 */
{
   struct elhashnode *tempptr = NULL;


   tempptr = *((thetable->elhashtablearray) + location);
   *((thetable->elhashtablearray) + location) = tempptr->nextnode;

   free (tempptr);


   return;

}



/* ************************************************************************** */
/* *                           DELETEELHASHTABLE                            * */
/* ************************************************************************** */


void deleteelhashtable (elhashtable *thetable)
/* pre  : We are given a pointer to a valid elhashtable.
 * post : Safely frees all memory locked in the element hash
 *        table nodes before removing the table itself.
 */
{
   struct elhashnode **tempptr = NULL;
   register long i,
	         tablesize;


   tablesize = thetable->arraysize;
   tempptr = thetable->elhashtablearray;
   for (i = 0L; i < tablesize; i++)
   {
      /* until the linked list of nodes is empty, delete each node */
      while (*(tempptr + i) != NULL)
      {
         deleteelhashtablenode (thetable, i);
      }/* end while *(tmpptr + i) */
      
   }/* end for i */

   free (tempptr);


   thetable->arraysize        = 0L;
   thetable->totalcontents    = 0UL;
   thetable->elhashtablearray = NULL;


   return;

}



/* ************************************************************************** */
/* *                       END OF IMPLEMENTATIONS                           * */
/* ************************************************************************** */




/* ******************************* END ************************************** */
