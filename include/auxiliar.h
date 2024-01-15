#include <allegro5/allegro5.h>
//#include <allegro5/allegro_primitives.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>


#define N_VERTICES 32 * 16

extern int Y_MAX,X_MAX;

typedef struct
{
    float x, y, z;
    int r,g,b;
} Point;

typedef struct
{
    float x, y, z;
} Vec3D;


typedef struct
{
    int left, right;
    int r,g,b;
} BufferLine;


extern Point M[];
extern ALLEGRO_COLOR color;
extern BufferLine row_buffer[];

void copy_t(Vec3D src[], Vec3D dest[]);
Vec3D normalize(Vec3D v);
Vec3D cross(Vec3D a, Vec3D b);
float dot(Vec3D a, Vec3D b);
void split(int a[], char *s);
void read_model(char *fn);
void put_pixel(int x, int y, ALLEGRO_COLOR color);
void line(int x, int y, int x1, int y1, ALLEGRO_COLOR color);
Vec3D bezier(Vec3D C[4][4], float t, float s);
Vec3D bezier_curve(Vec3D B[], float t, float s);
void draw_triangle(Vec3D t[]);
void flat_triangle(Vec3D t[]);
void fill_triangle(Point v[]);
Vec3D bezier_2d(Vec3D C[4], float t);