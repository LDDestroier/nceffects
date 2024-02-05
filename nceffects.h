#ifndef _NCEFFECTS_H
#define _NCEFFECTS_H

#include <curses.h>
#include <stdlib.h>

WINDOW *nce_rotate(WINDOW *win, int angle_deg, int *transpose_x, int *transpose_y);

WINDOW *nce_rotate_via_scale(WINDOW *win, int angle_deg, int *transpose_x, int *transpose_y);

WINDOW *nce_scale_box(WINDOW *win, int width, int height);

WINDOW *nce_scale_corners(WINDOW *win, int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4, int *transpose_x, int *transpose_y);

#endif
