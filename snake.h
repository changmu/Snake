#ifndef _SNAKE_H
#define _SNAKE_H

#include <stdio.h>
#include <string.h>
#include <conio.h>
#include <windows.h>
#include <time.h>

typedef struct Point {
    int x, y;
} Point;

// public
void game_init();       // print game interface
int game_move();       // change direction
void game_end();
// private
void appear_star();
void adjust_interval_time();
void move_point(Point *pos, int dir);
int check_pos(int, int);
void move_tail();
int move_head();
void print_at(const char *str, const Point pos);
void SetCursor(const Point p);
void HideCursor();

#endif
