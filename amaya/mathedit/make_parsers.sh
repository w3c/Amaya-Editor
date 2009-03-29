#!/bin/sh
#   (c) COPYRIGHT INRIA and W3C, 2009
#   Please first read the full copyright statement in file COPYRIGHT.
#
#  Author: F. Wang

# Chemistry mode
MODE=chemistry

#quex -i $MODE.qx --token-prefix TKN_
flex --prefix=$MODE --outfile=$MODE.flex.c $MODE.l 
bison --name-prefix=$MODE --output=$MODE.bison.cpp $MODE.ypp

