#!/bin/python

import sys

f = open(sys.argv[1],"r")

line = ",".join( map( lambda s:s.strip() , f.readlines() ) );

f.close();

out = open(sys.argv[1]+".formatted","w");

out.write( line );

out.close();
