#include <stdio.h>
#include <stdlib.h>
#include <curses.h>
#include <math.h>
#include "nceffects.h"
#include "minmax.h"

#define PRINT_INFO true

int main( int argc, char **argv)
{
	int angle = 0;	
	int transpose_x, transpose_y;
	int new_height, new_width; // original window height
	int ori_height, ori_width; // original window width
	int wx, wy;
	int point_select = 0;
	bool running = true;

	initscr();
	timeout(1000);
	curs_set(0);
	noecho();
	keypad(stdscr, TRUE);

	WINDOW *win = newwin(15, 15, 10, 10);
	for (int y = 0; y < 15; y++) {
		for (int x = 0; x < 25; x++) {
			mvwaddch(win, y, x, ':');
		}
	}
	refresh();

	wborder(win, '|', '|', '-', '-', '+', '+', '+', '+');
	mvwprintw(win, 2, 2, "Hello,");
	mvwprintw(win, 3, 3, "world!");
	
	Point corners[4];
	corners[0].x = 0,  corners[0].y = 0;
	corners[1].x = 15, corners[1].y = 0;
	corners[2].x = 15, corners[2].y = 15;
	corners[3].x = 0,  corners[3].y = 15;

	while (running)
	{
		clear();
		refresh();
		WINDOW *rwin, *swin;
		
		rwin = nce_rotate_via_scale(win, angle, &transpose_x, &transpose_y);
		rwin = nce_scale_corners(rwin,
			corners[0].x + transpose_x, corners[0].y + transpose_y,
			corners[1].x - transpose_x, corners[1].y + transpose_y,
			corners[2].x - transpose_x, corners[2].y - transpose_y,
			corners[3].x + transpose_x, corners[3].y - transpose_y,
			&transpose_x, &transpose_y	
		);
//		rwin = nce_scale_box(rwin, corners[2].x, corners[2].y);
		wrefresh(rwin);

		if (PRINT_INFO)
		{
			getmaxyx(rwin, new_height, new_width);
			getbegyx(rwin, wy, wx);
			mvprintw(30, 2, "size of window: %d x %d  ", new_width, new_height);
			mvprintw(31, 2, "transpose: %d x %d  ", transpose_x, transpose_y);
			mvprintw(32, 2, "window position: %d, %d  ", wx, wy);
			mvprintw(33, 2, "angle: %d degrees (%f radians)  ", angle, (double)angle * (M_PI / 180));
		}
		switch (getch())
		{
			case 'q': --angle; break;
			case 'w': ++angle; break;
			case '1': point_select = 0; break;
			case '2': point_select = 1; break;
			case '3': point_select = 2; break;
			case '4': point_select = 3; break;
			case KEY_UP:    corners[point_select].y--; break;
			case KEY_LEFT:  corners[point_select].x--; break;
			case KEY_DOWN:  corners[point_select].y++; break;
			case KEY_RIGHT: corners[point_select].x++; break;
			case KEY_BACKSPACE: running = false; break;
		};
	}

	endwin();
}
