###############################################################################
# Project:  COCKTAIL training
# Descr:    A simple pocket computer (scanner, parser, evaluator)
# Kind:     Makefile
# Author:   Prof. Dr. Juergen Vollmer <vollmer@dhbw-karlsruhe.de>
# $Id: Makefile,v 1.14 2011/05/05 13:59:52 vollmer Exp $
###############################################################################

# The executable to produce:
MAIN		= expr-pars

# Source files
SRC_C		= main.c
SRC_H		=
SRC		= expr$(SOLUTION).scan expr$(SOLUTION).pars

# Generated files
GEN_C		= expr_scan.c expr_scanSource.c expr_pars.c
GEN_H		= expr_scan.h expr_scanSource.h expr_pars.h
GEN		= expr.rex expr_scan.rpp expr_pars.lrk

###############################################################################

include ../config.mk

###############################################################################

# Create your parser
all: $(MAIN)

# Run the test suite
test:
	for f in test*; 	\
	do			\
	  echo "$$f ...";	\
	  ./$(MAIN) $$f; echo;	\
	done


###############################################################################

# Create the Parser
expr_pars.lrk expr_scan.rpp: expr$(SOLUTION).pars
	lpp -cxzj  expr$(SOLUTION).pars

LARK_OPTS = -cdi -Dw -5  # -5 ==> create the graphic LR-browser
LARK_OPTS = -cdi -Dw     #    ==> see ../config.mk
expr_pars.c expr_pars.h: expr_pars.lrk
	lark $(LARK_OPTS) expr_pars.lrk

# Create the Scanner
expr.rex:	expr$(SOLUTION).scan expr_scan.rpp
	rpp < expr$(SOLUTION).scan expr_scan.rpp > expr.rex

expr_scan.c expr_scan.h expr_scanSource.c expr_scanSource.h: expr.rex
	rex -cdis -y expr.rex
	rm -f expr_scanDrv.c

###############################################################################

include ../common.mk

###############################################################################
