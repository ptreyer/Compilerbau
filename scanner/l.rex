/* Project:  COCKTAIL training
 * Descr:    a simple scanner generated with rex
 * Kind:     REX scanner specification (solution)
 * Author:   Dr. Juergen Vollmer <juergen.vollmer@informatik-vollmer.de>
 * $Id: l.rex.in,v 1.10 2014/05/14 08:46:39 vollmer Exp $
 */

SCANNER l_scan

EXPORT {
/* code to be put into Scanner.h */

# include "Position.h"

/* Token Attributes.
 * For each token with user defined attributes, we need a typedef for the
 * token attributes.
 * The first struct-field must be of type tPosition!
 */
typedef struct {tPosition Pos; char* Value;} tint_const;
typedef struct {tPosition Pos; char* Value;} tfloat_const;
typedef struct {tPosition Pos; char* Value;} tstring_const;


/* There is only one "actual" token, during scanning. Therfore
 * we use a UNION of all token-attributes as data type for that unique
 * token-attribute variable.
 * All token (with and without user defined attributes) have one
 * attribute: the source position:
 *     tPosition     Position;
 */
typedef union {
  tPosition     Position;
  tint_const    int_const;
  tfloat_const float_const;
  tstring_const string_const;
} l_scan_tScanAttribute;

/* Tokens are coded as int's, with values >=0
 * The value 0 is reserved for the EofToken, which is defined automatically
 */
# define tok_int_const    1
# define tok_float_const  2
# define tok_string_const 4

#define tok_begin 5
#define tok_end 6
#define tok_plus 7

} // EXPORT

GLOBAL {
  # include <stdlib.h>
  # include "rString.h"
} // GLOBAL

LOCAL {
 /* user-defined local variables of the generated GetToken routine */
 #define MAX_STRING_LEN 2048
 char string[MAX_STRING_LEN+1]; int len;
 int NestingLevel = 0;
}  // LOCAL

DEFAULT {
  /* What happens if no scanner rule matches the input */
  WritePosition (stderr, l_scan_Attribute.Position);
  fprintf (stderr, " Illegal character [%c]\n", *l_scan_TokenPtr);
} // DEFAULT

EOF {
  /* What should be done if the end-of-input-file has been reached? */

  /* E.g.: check hat strings and comments are closed. */
  switch (yyStartState) {
  case STD:
    /* ok */
    break;
  case CMT:
    WritePosition(stderr, l_scan_Attribute.Position);
    fprintf(stderr, " Nicht abgecshlossener mehrzeiliger Kommentar\n");
    break;
  case STR:
    WritePosition(stderr, l_scan_Attribute.Position);
    fprintf(stderr, " Nicht abgeschlossener String\n");
    break;
  default:
    Message (" OOPS: that should not happen!!",
	     xxFatal, l_scan_Attribute.Position);
    break;
  }

  /* implicit: return the EofToken */
} // EOF

DEFINE  /* some abbreviations */
  digit  = {0-9}       .
  letter = {a-z_A-Z}    .

/* define start states, note STD is defined by default, separate several states by a comma */
/* START STRING */
START CMT, STR

RULE

/* Integers */
#STD# digit+ :
	{l_scan_Attribute.int_const.Value = malloc (l_scan_TokenLength+1);
	 l_scan_GetWord (l_scan_Attribute.int_const.Value);
	 return tok_int_const;
	}

/* Please add rules for: (don't forget to adapt main()) */
/* Float numbers */
#STD# digit+.digit+ :
  {
    l_scan_Attribute.float_const.Value = malloc (l_scan_TokenLength+1);
    l_scan_GetWord(l_scan_Attribute.float_const.Value);
    return tok_float_const;
  }

/* case insensitive keywords: BEGIN PROCEDURE END CASE */
#STD# {Bb}{Ee}{Gg}{Ii}{Nn}:{ return tok_begin; }
#STD# {Ee}{Nn}{Dd}:{return tok_end;}

#STD# "+":{return tok_plus;}

/* identifiers */

/* comment up to end of line */
#STD# "#" ANY*:
  {
    WritePosition(stdout, l_scan_Attribute.Position);
    printf(" Ich habe einen Kommentar gefunden\n");
  }

/* Multiline comment */
#STD# "'''": 
  {
    yyStart(CMT);
    WritePosition(stdout, l_scan_Attribute.Position);
    printf(" Begin eines mehrzeigligen Kommentars\n");
  }

#CMT# -{\t\n'}+ | ' | '':
  {
    
  }

#CMT# "'''":
  {
    yyStart(STD);
    WritePosition(stdout, l_scan_Attribute.Position);
    printf(" Ende des Kommentars\n");
  }



/* double quote delimited strings */
#STD# \" :
  {
    yyStart(STR);
    len = 0;
  }

#STR# (digit|letter|" ")+ : 
  {
    if (len + l_scan_TokenLength+1 >= MAX_STRING_LEN) {
	    WritePosition (stderr, l_scan_Attribute.Position);
	    fprintf (stderr, " String zu lang\n");
	    len = 0;
	  }

    len += l_scan_GetWord(&string[len]);
  }

#STR# \" :
  {
    yyStart(STD);
    string[len] = '\0';
    l_scan_Attribute.string_const.Value = malloc(len);
    strcpy(l_scan_Attribute.string_const.Value, string);
    return tok_string_const;
  }


/**********************************************************************/
