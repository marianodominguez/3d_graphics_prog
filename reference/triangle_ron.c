/**
 * Code interpolation example by Ron Hasting
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MARK do{printf("%d\n", __LINE__);fflush(stdout);}while(0)

const int WIDTH = 1024;
const int HEIGHT = 768;
const int EMPTY = 0xffffffff;
typedef int channel;
int miny, maxy;

typedef struct {
  channel r, g, b;
} color;

unsigned char channel_to_char(channel chan) {
  return (chan >> 16) & 0xff;
}

channel char_to_channel(unsigned int chr) {
  return ((channel)chr) << 16;
}

unsigned char buffer[HEIGHT][WIDTH][3];

typedef struct {
  int x, y;
  color c;
} Vec3D;

typedef struct {
  int x0, x1;
  color c0, c1;
} Row;

Row rows[HEIGHT];

void set_bound(int x, int y, color * c) {
  //buffer[y][x][0] = channel_to_char(c->r);
  //buffer[y][x][1] = channel_to_char(c->g);
  //buffer[y][x][2] = channel_to_char(c->b);
  if(y<miny) {
    miny = y;
  }
  if(y>maxy) {
    maxy = y;
  }

  if(rows[y].x0 == EMPTY) {
    rows[y].x0 = x;
    rows[y].c0 = *c;
  } else {
    if(rows[y].x0 >= x) {
      rows[y].x1 = rows[y].x0;
      rows[y].x0 = x;
      rows[y].c1 = rows[y].c0;
      rows[y].c0 = *c;
    } else {
      rows[y].x1 = x;
      rows[y].c1 = *c;
    }
  }
}

void plot_point(int x, int y, color * c) {
  buffer[y][x][0] = channel_to_char(c->r);
  buffer[y][x][1] = channel_to_char(c->g);
  buffer[y][x][2] = channel_to_char(c->b);
}

void plot_line_low(Vec3D * p0, Vec3D * p1) {
  color c = p0->c;

  int dx = p1->x - p0->x;
  int dy = p1->y - p0->y;
  int yi = 1;
  channel dr = (p1->c.r - p0->c.r)/dx;
  channel dg = (p1->c.g - p0->c.g)/dx;
  channel db = (p1->c.b - p0->c.b)/dx;

  if(dy < 0) {
    yi = -1;
    dy = -dy;
  }
  int D = (2*dy) - dx;
  int y = p0->y;

  for(int x=p0->x; x<=p1->x; x++) {
    set_bound(x, y, &c);
    if(D>0) {
      y += yi;
      D += (2 * (dy - dx));
    } else {
      D += 2*dy;
    }
    c.r += dr;
    c.g += dg;
    c.b += db;
  }
}

void plot_line_high(Vec3D * p0, Vec3D * p1) {
  color c = p0->c;

  int dx = p1->x - p0->x;
  int dy = p1->y - p0->y;
  int xi = 1;

  channel dr = (p1->c.r - p0->c.r)/dy;
  channel dg = (p1->c.g - p0->c.g)/dy;
  channel db = (p1->c.b - p0->c.b)/dy;

  if(dx < 0) {
    xi = -1;
    dx = -dx;
  }
  int D = (2*dx) - dy;
  int x = p0->x;

  for(int y=p0->y; y<=p1->y; y++) {
    set_bound(x, y, &c);
    if(D>0) {
      x += xi;
      D += (2 * (dx - dy));
    } else {
      D += 2*dx;
    }
    c.r += dr;
    c.g += dg;
    c.b += db;
  }
}

void plot_line(Vec3D * p0, Vec3D * p1) {
  if(abs(p1->y - p0->y) < abs(p1->x - p0->x)) {
    if(p0->x > p1->x) {
      plot_line_low(p1, p0);
    } else {
      plot_line_low(p0, p1);
    }
  } else {
    if(p0->y > p1->y) {
      plot_line_high(p1, p0);
    } else {
      plot_line_high(p0, p1);
    }
  }
}

void fill() {
  color c;
  int dr, dg, db;
  for(int y=miny; y<=maxy; y++) {
    c = rows[y].c0;
    int dx = rows[y].x1 - rows[y].x0;
    if(dx > 0) {
      dr = (rows[y].c1.r - rows[y].c0.r) / dx;
      dg = (rows[y].c1.g - rows[y].c0.g) / dx;
      db = (rows[y].c1.b - rows[y].c0.b) / dx;
    }
    for(int x=rows[y].x0; x<=rows[y].x1; x++) {
      plot_point(x, y, &c);
      c.r += dr;
      c.g += dg;
      c.b += db;
    }
    rows[y].x0 = rows[y].x1 = EMPTY;
  }
}

int main() {
  FILE* out;
  Vec3D p0, p1, p2;

  p0.x = 10;   p0.y = 10;
  p1.x = 600;  p1.y = 80;
  p2.x = 200;  p2.y = 600;
  p0.c.r = 255 << 16; p0.c.g = 0; p0.c.b = 0;
  p1.c.r = 0; p1.c.g = 255 << 16; p1.c.b = 0;
  p2.c.r = 0; p2.c.g = 0; p2.c.b = 255 << 16;

  for(int y=0; y<HEIGHT; y++) {
    rows[y].x0 = rows[y].x1 = EMPTY;
  }

  out = fopen("out.ppm", "wb");
  fprintf(out, "P6 %d %d 255\n", WIDTH, HEIGHT);

  miny = HEIGHT; maxy = -1;
  plot_line(&p0, &p1);
  plot_line(&p0, &p2);
  plot_line(&p2, &p1);

  fill();

  for(int y=0; y<HEIGHT; y++) {
    for(int x=0; x<WIDTH; x++) {
      fputc(buffer[y][x][0], out);
      fputc(buffer[y][x][1], out);
      fputc(buffer[y][x][2], out);
    }
  }
  fclose(out);
}