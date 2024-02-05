#include "minmax.h"
#include <stdlib.h>
#include <float.h>
#include <limits.h>
#include <string.h>

/*
typedef struct Point {
	int x;
	int y;
} Point;

typedef struct DPoint {
	double x;
	double y;
} DPoint;
*/

int round_double(double number)
{
    if (number > 0) return (int)(number + 0.5); else return (int)(number - 0.5);
}

Point dpoint_to_point(DPoint p)
{
	return (Point){
		round_double(p.x),
		round_double(p.y)
	};
}

int i_max(int i1, int i2)
{
	if (i1 > i2) return i1; else return i2;
}

int i_min(int i1, int i2)
{
	if (i1 < i2) return i1; else return i2;
}

double d_max(double d1, double d2)
{
	if (d1 > d2) return d1; else return d2;
}

double d_min(double d1, double d2)
{
	if (d1 < d2) return d1; else return d2;
}

double d_max4(double d1, double d2, double d3, double d4)
{
	double output = DBL_MIN;
	output = d_max(output, d1);
	output = d_max(output, d2);
	output = d_max(output, d3);
	output = d_max(output, d4);
	return output;
}

double d_min4(double d1, double d2, double d3, double d4)
{
	double output = DBL_MAX;
	output = d_min(output, d1);
	output = d_min(output, d2);
	output = d_min(output, d3);
	output = d_min(output, d4);
	return output;
}

int i_max4(int i1, int i2, int i3, int i4)
{
	int output = INT_MIN;
	output = i_max(output, i1);
	output = i_max(output, i2);
	output = i_max(output, i3);
	output = i_max(output, i4);
	return output;
}

int i_min4(int i1, int i2, int i3, int i4)
{
	int output = INT_MAX;
	output = i_min(output, i1);
	output = i_min(output, i2);
	output = i_min(output, i3);
	output = i_min(output, i4);
	return output;
}
