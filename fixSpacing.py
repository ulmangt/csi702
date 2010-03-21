#!/usr/bin/python

import sys

argc = len(sys.argv)

if argc == 1:
	print "Usage: infile [outfile]"
	exit()
else:
	infile = sys.argv[1]
	if argc == 2:
		outfile = infile + ".out"
	else:
		outfile = sys.argv[2]

fin = open( infile , 'r' )
fout = open( outfile , 'w' )

for line in fin.readlines( ):
	fout.write( " " + line )

fin.close()
fout.close()
