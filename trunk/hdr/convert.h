#ifndef CONVERT_H_INCLUDED
#define CONVERT_H_INCLUDED

#define PI acos(-1)
#define RAD_TO_DEG 90.0 / PI
#define DEG_TO_RAD 1.0 / RAD_TO_DEG

float toRadians( float );
float toDegrees( float );
float fromRadians( float );
float fromDegrees( float );

#endif
