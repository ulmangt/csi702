#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "convert.h"

/////////////////////////////////////////////////////
// convert.c
//
// This file contains helper functions for performing
// basic unit conversions. It is not intended to be
// a comprehensive unit conversion library, simply
// to provide enough functionality for the quantities
// this particular code deals with.
/////////////////////////////////////////////////////

// angle system units : radians
// time system units : seconds
// distance system units : meters

// convert from system units to radians
float toRadians( float angle )
{
  return angle;
}

// convert from system units to degrees
float toDegrees( float angle )
{
  return RAD_TO_DEG * angle;
}

// convert from system units to navy degrees
// 0 degrees is 'up', values increase CW
float toNavyDegrees( float angle )
{
  float degrees = toDegrees( angle );
  return 90.0 - degrees;
}

// convert to system units from radians
float fromRadians( float angle )
{
  return angle;
}

// convert to system units from degrees
float fromDegrees( float angle )
{
  return DEG_TO_RAD * angle;
}
