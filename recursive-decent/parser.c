/* Project:  COCKTAIL training
 * Descr:    A recursive Decent Parser for expressions
 * Kind:     The parser (solution)
 * Author:   Dr. Juergen Vollmer <juergen.vollmer@informatik-vollmer.de>
 * $Id: parser.c.in,v 1.4 2013/05/06 12:09:14 vollmer Exp $
 */

# include <stdlib.h>
# include "expr_scan.h"
# include "parser.h"
# include "Errors.h"
      	/* contains: `Message', `MessageI', `xxFatal', `xxError' */

tToken CurToken;

/*****************************************************************************
 * helpers
 *****************************************************************************/

/* Debugging:  if the C-compiler is called with `-DDEBUG',
 *             some output will be generated
 */
# ifdef DEBUG
# define DEBUG_show(msg) printf ("%-5s cur = `%s'\n", msg, token2string (CurToken));
# else
# define DEBUG_show(msg)  /* no output */
# endif

static const char* token2string (tToken token)
/* maps token-code to token textual representation */
{
  switch (token) {
  case '+':
    return "+";
  case '*':
    return "*";
  case '(':
    return "(";
  case ')':
    return ")";
  case tok_identifier:
    return "identifier";
  case expr_scan_EofToken:
    return "EOF";
  default:
    /* this should never happen */
    MessageI ("FATAL ERROR: unknown token", xxFatal,
	      expr_scan_Attribute.Position, xxInteger, (char*) &token);
    abort(); /* aborts the program */
  }
}

/******************************************************************************/

bool match (tToken token)
/* Instead of writing for each token 't' a function 'f_t()', we use
 * 'match (t)'.
 */
{
  DEBUG_show ("MATCH");
# ifdef DEBUG
  printf ("      cur = `%s' expected = `%s'\n", token2string(CurToken), token2string (token));
# endif
  if (CurToken == token) {
    if (CurToken != expr_scan_EofToken) CurToken = expr_scan_GetToken();
    DEBUG_show ("  new");
    return true;
  } else {
    char msg[255];
    sprintf (msg, "Syntax Error: found: `%s' expected: `%s'",
	     token2string (CurToken), token2string (token));
    Message (msg, xxError, expr_scan_Attribute.Position);
    return false;  /* Syntax Error */
  }
}



/*****************************************************************************
 * First-Follow-set Tests
 * 1)  E  ::= T E1         FIRST (T E1    FOLLOW (E))  = { ( id    }
 * 2)  E1 ::= + T E1       FIRST (+ T E1  FOLLOW (E1)) = { +       }
 * 3)  E1 ::= epsilon      FIRST (epsilon FOLLOW (E1)) = { eof )   }
 * 4)  T  ::= F T1         FIRST (F T1    FOLLOW (T))  = { ( id    }
 * 5)  T1 ::= * F T1       FIRST (* F T1  FOLLOW (T1)) = { *       }
 * 6)  T1 ::= epsilon      FIRST (epsilon FOLLOW (T1)) = { eof ) + }
 * 7)  F  ::= ( E )        FIRST ( ( E )  FOLLOW (F))  = { (       }
 * 8)  F  ::= id           FIRST (id      FOLLOW (F))  = { id      }
 *
 * The function 'FirstFollow_i ()' returns true, if and only if 'CurToken'
 * is element of the appropriate set.
 ******************************************************************************/

static bool FirstFollow_1 (void)
{
  switch (CurToken) {
  case '(': case tok_identifier:
    return true;
  default:
    return false;
  }
}


static bool FirstFollow_2 (void)
{
  switch (CurToken) {
  case '+':
    return true;
  default:
    return false;
  }
}


static bool FirstFollow_3 (void)
{
  switch (CurToken) {
  case expr_scan_EofToken: case ')':
    return true;
  default:
    return false;
  }
}

static bool FirstFollow_4 (void)
{
  switch (CurToken) {
  case '(': case tok_identifier:
    return true;
  default:
    return false;
  }
}

static bool FirstFollow_5 (void)
{
  switch (CurToken) {
  case '*': 
    return true;
  default:
    return false;
  }
}

static bool FirstFollow_6 (void)
{
  switch (CurToken) {
    case expr_scan_EofToken: case ')': case '+':
    return true;
  default:
    return false;
  }
}

static bool FirstFollow_7 (void)
{
  switch (CurToken) {
    case '(': 
    return true;
  default:
    return false;
  }
}

static bool FirstFollow_8 (void)
{
  switch (CurToken) {
    case tok_identifier: 
    return true;
  default:
    return false;
  }
}

/* fill in the other FirstFollow-functions: FirstFollow_2() .. FirstFollow_8() */

/******************************************************************************
 * We use the following grammar for arithmetic expressions:
 * 1)  E  ::= T E1
 * 2)  E1 ::= + T E1       3)  E1 ::= epsilon
 * 4)  T  ::= F T1
 * 5)  T1 ::= * F T1       6)  T1 ::= epsilon
 * 7)  F  ::= ( E )        8)  F  ::= id
 ******************************************************************************/

/* The function 'f_nt()' for the non-terminal 'nt' returns true, iff
 * 'CurToken' starts a sequence of input-tokens which may be generated by 'nt'.
 * 'CurToken' refers after that to the next token to be processed.
 * Note: eof = end-of-file  is a token too.
 */
static bool f_E1(void);
static bool f_T(void);
static bool f_T1(void);
static bool f_F(void);

bool f_E (void)
{ 
  DEBUG_show("E");

  if (FirstFollow_1()) return f_T() && f_E1();
  Message ("Syntax Error in <E>", xxError, expr_scan_Attribute.Position);
  return false; /* Syntax Error */
}

bool f_E1 (void)
{
  DEBUG_show("E1");

  if (FirstFollow_2()) return match ('+') && f_T() && f_E1();
  if (FirstFollow_3()) return true;
  Message ("Syntax Error in <E1>", xxError, expr_scan_Attribute.Position);
  return false; /* Syntax Error */
}

bool f_T (void)
{
  DEBUG_show("T");

  if (FirstFollow_4()) return  f_F() && f_T1();
  Message ("Syntax Error in <T>", xxError, expr_scan_Attribute.Position);
  return false; /* Syntax Error */
}

bool f_T1 (void)
{
  DEBUG_show("T1");

  if (FirstFollow_5()) return match ('*') && f_F() && f_T1();
  if (FirstFollow_6()) return true;
  Message ("Syntax Error in <T1>", xxError, expr_scan_Attribute.Position);
  return false; /* Syntax Error */
}

bool f_F (void)
{
  DEBUG_show("F");

  if (FirstFollow_7()) return match ('(') && f_E() && match (')');
  if (FirstFollow_8()) return match (tok_identifier);
  Message ("Syntax Error in <F>", xxError, expr_scan_Attribute.Position);
  return false; /* Syntax Error */
}




/* fill in the functions for the other non-terminals */

/***********************  E  N  D  ***********************************************/
