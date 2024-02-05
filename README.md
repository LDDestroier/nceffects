# NCEffects (NCurses Effects)

```
WINDOW *nce_scale_corners(
    WINDOW *win,
    int x1, int y1,
    int x2, int y2,
    int x3, int y3,
    int x4, int y4,
    int *transpose_x, int *transpose_y
);
```

Returns a new curses window that is transformed according to the points given, relative to window origin and NOT absolute screen coordinates. Points are in order of Top Left, Top Right, Bottom Right, Bottom Left.

transpose_x and transpose_y are set to values representing how much the window moved to accomodate the transformation.
Any other functions that use transpose_x and transpose_y will do the same thing.


`WINDOW *nce_rotate( WINDOW *win,  int angle_deg, int *transpose_x, int *transpose_y );`

Returns a new curses window that is rotated by angle_deg degrees (not radians).


`WINDOW *nce_rotate_via_scale( WINDOW *win, int angle_deg, int *transpose_x, int *transpose_y );`

Returns a new curses window that is rotated by angle_deg degrees. Wrapper of nce_scale_corners(), so it will look different from nce_rotate().
This one is intended to be the main way of rotating windows once I can get it to look good enough.


`WINDOW *nce_scale_box( WINDOW *win, int width, int height );`

Returns a new curses window stretched to a given width and height. Wrapper of nce_scale_corners().
