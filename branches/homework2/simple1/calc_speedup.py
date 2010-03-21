#!/bin/python

import sys

fslow = open(sys.argv[1],"r")
ffast = open(sys.argv[2],"r")

vslow = map( lambda s:float(s.strip()) , fslow.readlines() )
vfast = map( lambda s:float(s.strip()) , ffast.readlines() )

fslow.close()
ffast.close()

def speedup( slow, fast ):
  if fast == 0.0:
    return 0.0;
  
  return slow / fast;

print vslow
print vfast

print ",".join(map( lambda d:"{0:.2f}".format(d), map ( speedup , vslow , vfast ) ) )
