#include <allegro5/allegro5.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>


#define N_VERTICES 32 * 16

extern int Y_MAX,X_MAX;

typedef struct
{
    float x, y, z;
} Point;

typedef struct
{
    int left, right, r,g,b;
} BufferLine;


extern Point M[];
extern ALLEGRO_COLOR color;
extern BufferLine row_buffer[];

void copy_t(Point src[], Point dest[]);
Point normalize(Point v);
Point cross(Point a, Point b);
float dot(Point a, Point b);
void split(int a[], char *s);
void read_model(char *fn);
void put_pixel(int x, int y, ALLEGRO_COLOR color);
void line(int x, int y, int x1, int y1, ALLEGRO_COLOR color);
Point bezier(Point C[4][4], float t, float s);
Point bezier_curve(Point B[], float t, float s);
void draw_triangle(Point t[]);
void fill_triangle(Point t[]);