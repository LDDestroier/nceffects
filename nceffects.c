
/*
 *  NCEFFECTS
 *  Rotation + scaling library for Curses for C
 *
 *  TODO:
 *  + Rotation is wonky if the input window has a dimension that isn't an even number
 *  + Add scaling (has gotta be easier than this rotation shit
 */


#include <stdlib.h>
#include <stdio.h>
#include "minmax.h"
#include <curses.h>
#include <locale.h>
#include <math.h>
#include <limits.h>
#include <float.h>
#include <string.h>

#define ERR_CHAR -1
#define PRINT_INFO true
#define COMPUTE_SCALE_FORWARDS true
#define COMPUTE_SCALE_BACKWARDS true

char *NCE_REPEATABLE_CHARS = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890!?,.'\"` +";

// Checks if character should be repeated during rotation
bool check_repeatable(char ch)
{
	for (int i = 0; i < strlen(NCE_REPEATABLE_CHARS); i++) {
		if (NCE_REPEATABLE_CHARS[i] == ch) return false;
	}
	return true;
}

// Returns visually rotated character based on angle input (in degrees)
char rotate_char(char ch, int angle_deg)
{
	char output;
	char twirl[9];
	int deg_index = (((angle_deg + 22) % 360) / 45);
	switch(ch) {
		case '|': strcpy(twirl, "|/-\\|/-\\"); break;
		case '/': strcpy(twirl, "/-\\|/-\\|"); break;
		case '-': strcpy(twirl, "-\\|/-\\|/"); break;
		case '\\': strcpy(twirl, "\\|/-\\|/-"); break;
		case '(': strcpy(twirl, "((()))(("); break;
		case ')': strcpy(twirl, ")))((())"); break;
		case '[': strcpy(twirl, "[[[]]][["); break;
		case ']': strcpy(twirl, "]]][[[]]"); break;
		case '_': strcpy(twirl, "_\\|/_\\|/"); break;
		case '>': strcpy(twirl, ">>v<<<^>"); break;
		case '<': strcpy(twirl, "<<^>>>v<"); break;
		case '^': strcpy(twirl, "^^>>v<<^"); break;
		default: return ch;
	}
	return twirl[deg_index];
}

// if num is very close to -1, 0, or 1, returns those exact values
// this is meant to mitigate floating point errors
double neaten_double(double num)
{
	const double threshold = 0.0001;
	if (num > -threshold     && num < threshold) num = 0;
	if (num > -threshold - 1 && num < threshold - 1) num = -1;
	if (num > -threshold + 1 && num < threshold + 1) num = 1;
	return num;
}

DPoint rotate_dpoint(double cx, double cy, int angle_deg, double point_x, double point_y, bool adjust)
{
	double angle = (double)angle_deg * (M_PI / 180.0); // convert degrees to radian
	DPoint p = (DPoint){point_x, point_y};
	double s = sin(angle);
	double c = cos(angle);

	// round sin and cos values that are very close to -1, 0, or 1
	s = neaten_double(s);
	c = neaten_double(c);

	// adjust origin depending on angle
	if (adjust) {
		// maybe do something
	}

	// translate point back to origin:
	p.x -= cx;
	p.y -= cy;

	// rotate point
	double xnew = p.x * c - p.y * s;
	double ynew = p.x * s + p.y * c;

	// translate point back:
	p.x = xnew + cx;
	p.y = ynew + cy;
	return p;
}

// wrapper for rotate_dpoint that returns rounded values
Point rotate_point(double cx, double cy, int angle_deg, double point_x, double point_y, bool adjust)
{
	DPoint dpoint = rotate_dpoint(cx, cy, angle_deg, point_x, point_y, adjust);
	return (Point){round_double(dpoint.x), round_double(dpoint.y)};
}

void get_center(int width, int height, double *origin_x, double *origin_y)
{
	// i kept it so wonky so that I could experiment with having origin points at different
	// offsets depending on if height or width was even or odd
	if (width % 2)
		// odd
		*origin_x = (double)(width / 2);
	else
		// even
		*origin_x = (double)(width / 2);
	
	if (height % 2)
		// odd
		*origin_y = (double)(height / 2);
	else
		// even
		*origin_y = (double)(height / 2);
	
	*origin_x = (double)round_double(*origin_x);
	*origin_y = (double)round_double(*origin_y);
	
}

void get_rotated_corners( WINDOW *win, int angle_deg, Point *topleft, Point *bottomright, Point points[4] )
{
	int wx, wy, width, height;
	double origin_x, origin_y;

	getparyx(win, wy, wx);
	getmaxyx(win, height, width);

	get_center(width, height, &origin_x, &origin_y);
	
	DPoint dpoints[4];
	dpoints[0] = rotate_dpoint(origin_x, origin_y, angle_deg, 0,             0,              false);
	dpoints[1] = rotate_dpoint(origin_x, origin_y, angle_deg, (double)width, 0,              false);
	dpoints[2] = rotate_dpoint(origin_x, origin_y, angle_deg, (double)width, (double)height, false);
	dpoints[3] = rotate_dpoint(origin_x, origin_y, angle_deg, 0,             (double)height, false);

	for (int i = 0; i < 4; i++)
		points[i] = dpoint_to_point(dpoints[i]);

	topleft->x     = i_min4(points[0].x, points[1].x, points[2].x, points[3].x);
	topleft->y     = i_min4(points[0].y, points[1].y, points[2].y, points[3].y);
	bottomright->x = i_max4(points[0].x, points[1].x, points[2].x, points[3].x);
	bottomright->y = i_max4(points[0].y, points[1].y, points[2].y, points[3].y);
}

WINDOW *nce_rotate(WINDOW *win, int angle_deg, int *transpose_x, int *transpose_y)
{
	Point topleft, bottomright;
	Point points[4];
	get_rotated_corners(win, angle_deg, &topleft, &bottomright, points);
	angle_deg %= 360;
	if (angle_deg < 0) angle_deg += 360;

	double angle_rad = (double)angle_deg * (M_PI / 180);
	double origin_x, origin_y;
	int wx, wy;
	int old_width, old_height;
	getmaxyx(win, old_height, old_width);
	getbegyx(win, wy, wx);

	*transpose_x = topleft.x;
	*transpose_y = topleft.y;
	int new_width  = bottomright.x - topleft.x;
	int new_height = bottomright.y - topleft.y;

	// increase window size to deal with cutoff
	// it's sloppy and makes drawing over other windows shitty but it has to do
	++new_width;
	++new_height;

	WINDOW *outwin = newwin(new_height, new_width, wy + *transpose_y, wx + *transpose_x);
	Point rpos;
	char wch;
	
	// Iterate over new window, and rotate in reverse to map to base window
	// This repeats characters (useful for border characters)

	for (int y = 0; y < new_height; y++)
	{
		for (int x = 0; x < new_width; x++)
		{
			rpos = rotate_point(origin_x, origin_y, -angle_deg, x + *transpose_x, y + *transpose_y, true);
			wch = mvwinch(win, rpos.y, rpos.x);
			wch = rotate_char(wch, angle_deg);

			// check if character should be repeated
			if ( check_repeatable(wch) && wch != ERR_CHAR) {
				mvwaddch(outwin, y, x, wch);
			}
			
		}
	}

	// Iterate over base window, and rotate it forwards to map onto new window
	// This will not repeat characters (useful for alphanumeric)

	for (int y = 0; y < old_height; y++) {
		for (int x = 0; x < old_width; x++) {
			rpos = rotate_point(origin_x, origin_y, angle_deg, x, y, true);
			wch = mvwinch(win, y, x);

			// check if character should not be repeated
			if ( !check_repeatable(wch) && wch != ERR_CHAR ) {
				mvwaddch(outwin, rpos.y - *transpose_y, rpos.x - *transpose_x, wch);
			}
		}
	}
	return outwin;

}

// adapted from SO answer on https://stackoverflow.com/a/36372812
// t ranges from 0.0 to 1.0
DPoint lerp(DPoint p1, DPoint p2, double t)
{
	return (DPoint){
		.x = p1.x + (p2.x - p1.x) * t,
		.y = p1.y + (p2.y - p1.y) * t
	};
}

int get_angle_of_points(DPoint p1, DPoint p2)
{
	int angle = round_double(
		(atan2( p2.x - p1.x, p2.y - p1.y ) * (-180 / M_PI) + 90)
	) % 360;
	if (angle < 0) angle += 360;
	return angle;
}

// Scales and skews a window according to four corners given
WINDOW *nce_scale_corners(WINDOW *win, int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4, int *transpose_x, int *transpose_y)
{	
	int old_width, old_height;
	int old_winx, old_winy;
	getmaxyx(win, old_height, old_width);
	getbegyx(win, old_winy, old_winx);

	// calculate new window X,Y position
	// also new window height and width
	int new_x1 = i_min4(x1, x2, x3, x4) + old_winx;
	int new_y1 = i_min4(y1, y2, y3, y4) + old_winy;
	int new_x2 = i_max4(x1, x2, x3, x4) + old_winx;
	int new_y2 = i_max4(y1, y2, y3, y4) + old_winy;

	DPoint corners[4];
	corners[0] = (DPoint){(double)x1, (double)y1}; // top left
	corners[1] = (DPoint){(double)x2, (double)y2}; // top right
	corners[2] = (DPoint){(double)x3, (double)y3}; // bottom right
	corners[3] = (DPoint){(double)x4, (double)y4}; // bottom left

	// transpose window if X or Y are below zero
	{
		*transpose_x = new_x1 - old_winx;
		*transpose_y = new_y1 - old_winy;
		for (int i = 0; i < 4; i++) {
			corners[i].x -= *transpose_x;
			corners[i].y -= *transpose_y;
		}
	}

	int new_width = new_x2 - new_x1;
	int new_height = new_y2 - new_y1;

	WINDOW *outwin = newwin(new_height, new_width, new_y1, new_x1);	

	DPoint leftpoint, rightpoint, inpoint, winpoint;
	char ch;
	int slope_deg;
	DPoint p;
	int fordirx, fordiry;

	// Iterate over new window, and scale back onto base window

	if (COMPUTE_SCALE_BACKWARDS) {
	fordiry = 1;
	for (int y = 0; y < new_height; y++) {

		leftpoint =  lerp(corners[0], corners[3], (double)y / new_height);
		rightpoint = lerp(corners[1], corners[2], (double)y / new_height);
		slope_deg = get_angle_of_points(leftpoint, rightpoint);

		fordirx = 1;
		for (int x = 0; x < new_width; x++) {

			inpoint = lerp(leftpoint, rightpoint, (double)x / new_width);

			winpoint.x = round_double(((double)x / new_width  - 0) * old_width);
			winpoint.y = round_double(((double)y / new_height - 0) * old_height);
			
			if (
				winpoint.x >= 0 &&
		        	winpoint.x < old_width &&
				winpoint.y >= 0 &&
				winpoint.y < old_height) 
			{
				ch = mvwinch(win, winpoint.y, winpoint.x);
//				ch = rotate_char(ch, slope_deg);
				
				if (
					ch != ' '
					&& ch != ERR_CHAR
					&& check_repeatable(ch)
				) mvwaddch(outwin, round_double(inpoint.y), round_double(inpoint.x), ch);
			}
		}
	}}

	// Iterate over base window, and scale onto new window

	if (COMPUTE_SCALE_FORWARDS)
	for (int y = 0; y < old_height; y++) {
		leftpoint  = lerp(corners[0], corners[3], (double)y / old_height);
		rightpoint = lerp(corners[1], corners[2], (double)y / old_height);
		slope_deg = get_angle_of_points(leftpoint, rightpoint);

		for (int x = 0; x < old_width; x++) {		
			inpoint = lerp(leftpoint, rightpoint, (double)x / old_width);
			inpoint.x = round_double(inpoint.x);
			inpoint.y = round_double(inpoint.y);

			ch = mvwinch(win, y, x);
//			ch = rotate_char(ch, slope_deg);
			
			if (
				ch != ' '
				&& ch != ERR_CHAR
				&& !check_repeatable(ch)
			) mvwaddch(outwin, inpoint.y, inpoint.x, ch);	
		}
	}

	return outwin;
}

WINDOW *nce_rotate_via_scale(WINDOW *win, int angle_deg, int *transpose_x, int *transpose_y)
{
	Point points[4];
	Point topleft;
	Point bottomright;
	get_rotated_corners(win, angle_deg, &topleft, &bottomright, points);
	
	return nce_scale_corners(win,
		points[0].x, points[0].y,
		points[1].x, points[1].y,
		points[2].x, points[2].y,
		points[3].x, points[3].y,
		transpose_x, transpose_y
	);
}

WINDOW *nce_scale_box(WINDOW *win, int width, int height)
{
	int transpose_x, transpose_y;
	int winx, winy;
	getbegyx(win, winy, winx);
	return nce_scale_corners(win,
		0,     0,
		width, 0,
		width, height,
		0,     height,
		&transpose_x, &transpose_y
	);
}
