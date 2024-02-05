#ifndef _MINMAX_H
#define _MINMAX_H

#include <stdlib.h>

typedef struct Point {
	int x;
	int y;
} Point;

typedef struct DPoint {
	double x;
	double y;
} DPoint;

int round_double(double number);

Point dpoint_to_point(DPoint p);

int i_max(int i1, int i2);

int i_min(int i1, int i2);

double d_max(double d1, double d2);

double d_min(double d1, double d2);

double d_max4(double d1, double d2, double d3, double d4);

double d_min4(double d1, double d2, double d3, double d4);

int i_max4(int i1, int i2, int i3, int i4);

int i_min4(int i1, int i2, int i3, int i4);

#endif
