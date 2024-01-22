/**
 * Back face culling, remove triangles facing away from camera
 *
 */

#include <allegro5/allegro5.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_primitives.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define N_VERTICES 32 * 16

const int nvert = 4;
const int nfaces = 6;
const float sqrt2 = 1.414;
const float sqrt6 = 2.449;

unsigned int idx;
unsigned int screen, row, col;

float th = M_PI;
int X_MAX = 800, Y_MAX = 600;

typedef struct {
    float x, y, z;
} Vec3D;

Vec3D M[N_VERTICES];
Vec3D VIEWPOINT = {3.5, -4.0, 6.0};
ALLEGRO_COLOR color;

Vec3D normalize(Vec3D v) {
    Vec3D r;
    float length = sqrt(v.x * v.x + v.y * v.y + v.z * v.z);

    r.x = v.x / length;
    r.y = v.y / length;
    r.z = v.z / length;

    return r;
}

Vec3D cross(Vec3D a, Vec3D b) {
    Vec3D r;
    r.x = (a.y * b.z - a.z * b.y);
    r.y = -(a.x * b.z - a.z * b.x);
    r.z = (a.x * b.y - a.y * b.x);
    return r;
}

float dot(Vec3D a, Vec3D b) { return a.x * b.x + a.y * b.y + a.z * b.z; }

void split(int a[], char *s) {
    char *token;
    const char delim[2] = ",";

    token = strtok(s, delim);
    int i = 0;
    while (token != NULL) {
        a[i] = atoi(token);
        i++;
        token = strtok(NULL, delim);
    }
}

void read_model(char *fn) {
    char tmpx[12], tmpy[12], tmpz[12];
    char *dummy;
    char nv[5], line[255];
    int patch[16][32], points[16];
    Vec3D P[306];

    FILE *f = fopen(fn, "r");
    if (f == NULL) {
        printf("Unable to read model");
        exit(1);
    }
    dummy = fgets(nv, sizeof(nv), f);
    if (atoi(nv) * 16 != N_VERTICES) {
        printf("Invalid model, wrong number of vertices");
        exit(1);
    }

    // read patch vertices
    for (int i = 0; i < 32; i++) {
        dummy = fgets(line, sizeof(line), f);
        split(points, line);
        for (int j = 0; j < 16; j++) {
            patch[j][i] = points[j];
        }
    }

    // Read points

    dummy = fgets(line, sizeof(line), f);
    if (atoi(line) != 306) {
        printf("Invalid model, wrong number of points");
        exit(1);
    }

    for (int i = 0; i < 306; i++) {
        dummy = fgets(line, sizeof(line), f);
        if (strlen(line) < 5) dummy = fgets(line, sizeof(line), f);
        sscanf(line, "%f,%f,%f", &P[i].x, &P[i].y, &P[i].z);
        // printf("%f %f %f\n",P[i].x,P[i].y,P[i].z);
    }
    int idx = 0;
    int v;
    for (int j = 0; j < 32; j++) {
        for (int i = 0; i < 16; i++) {
            v = patch[i][j] - 1;
            // printf("%d :",v);
            M[idx].x = P[v].x;
            M[idx].y = P[v].y;
            M[idx].z = P[v].z;
            // printf("%f %f %f\n",M[idx].x,M[idx].y,M[idx].z);
            idx++;
        }
    }
}

void put_pixel(int x, int y, ALLEGRO_COLOR color) {
    al_draw_pixel(x, y, color);
}

void line(int x, int y, int x1, int y1, ALLEGRO_COLOR color) {
    if (x > X_MAX || y > Y_MAX || x < 0 || y < 0) return;
    if (x1 > X_MAX || y1 > Y_MAX || x1 < 0 || y1 < 0) return;

    int x0 = x;
    int y0 = y;
    int dx = abs(x1 - x0);
    int dy = abs(y1 - y0);
    int sx = -1;
    int sy = -1;
    int e2, error;

    if (x0 < x1) {
        sx = 1;
    }
    if (y0 < y1) {
        sy = 1;
    }
    error = dx - dy;
    while (x0 != x1 || y0 != y1) {
        put_pixel(x0, y0, color);
        e2 = 2 * error;
        if (e2 > -dy) {
            error -= dy;
            x0 += sx;
        }
        if (e2 < dx) {
            error += dx;
            y0 += sy;
        }
    }
}

Vec3D isometric_projection(float x, float y, float z) {
    Vec3D result;
    result.x = (x - z) / sqrt2;
    result.y = (x + 2 * y + z) / sqrt6;
    result.z = 0;
    return result;
}

Vec3D camera_projection(float x, float y, float z, float d) {
    Vec3D r;

    r.x = x * d / z;
    r.y = y * d / z;
    r.z = 0;

    // printf("(%f,%f)",r.x,r.y);
    return r;
}

Vec3D translate(float x, float y, float z, Vec3D d) {
    Vec3D result;
    result.x = x + d.x;
    result.y = y + d.y;
    result.z = z + d.z;
    return result;
}

Vec3D rotate_x(float x, float y, float z, float th) {
    Vec3D result;
    result.x = x;
    result.y = y * cos(th) - z * sin(th);
    result.z = y * sin(th) + z * cos(th);
    return result;
}

Vec3D rotate_y(float x, float y, float z, float th) {
    Vec3D result;
    result.x = x * cos(th) + z * sin(th);
    result.y = y;
    result.z = -x * sin(th) + z * cos(th);
    return result;
}

Vec3D rotate_z(float x, float y, float z, float th) {
    Vec3D result;
    result.x = x * cos(th) - y * sin(th);
    result.y = x * sin(th) + y * cos(th);
    result.z = z;
    return result;
}

void draw_polygon(Vec3D t[], int n) {
    for (int i = 0; i < n - 1; i++) {
        line(t[i].x, t[i].y, t[i + 1].x, t[i + 1].y, color);
    }
    line(t[n - 1].x, t[n - 1].y, t[0].x, t[0].y, color);
}

Vec3D bezier(Vec3D C[4][4], float t, float s) {
    Vec3D p = {0, 0, 0};
    float b[4], c[4];
    b[0] = (1 - t) * (1 - t) * (1 - t);
    b[1] = 3 * t * (1 - t) * (1 - t);
    b[2] = 3 * t * t * (1 - t);
    b[3] = t * t * t;

    c[0] = (1 - s) * (1 - s) * (1 - s);
    c[1] = 3 * s * (1 - s) * (1 - s);
    c[2] = 3 * s * s * (1 - s);
    c[3] = s * s * s;

    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            p.x = p.x + b[i] * c[j] * C[i][j].x;
            p.y = p.y + b[i] * c[j] * C[i][j].y;
            p.z = p.z + b[i] * c[j] * C[i][j].z;
        }
    }
    return p;
}

int visible(Vec3D p[]) {
    Vec3D c, v0, v1, n;

    v0.x = p[1].x - p[0].x;
    v0.y = p[1].y - p[0].y;
    v0.z = p[1].z - p[0].z;

    v1.x = p[2].x - p[0].x;
    v1.y = p[2].y - p[0].y;
    v1.z = p[2].z - p[0].z;

    n = normalize(cross(v0, v1));

    c.x = -p[0].x;
    c.y = -p[0].y;
    c.z = -p[0].z;

    if (dot(c, n) >= 0) return 0;

    return 1;
}

Vec3D bezier_curve(Vec3D B[], float t, float s) {
    static Vec3D p;
    Vec3D C[4][4];
    for (int j = 0; j < 4; j++) {
        for (int i = 0; i < 4; i++) C[i][j] = B[4 * j + i];
    }
    p = bezier(C, t, s);
    return p;
}

Vec3D *projection(Vec3D p[]) {
    static Vec3D poly[4];
    float xs, ys, x, y;
    Vec3D pp;
    for (int i = 0; i < 4; i++) {
        // pp=camera_projection(p[i].x,p[i].y,p[i].z, -7.0);
        pp = isometric_projection(p[i].x, p[i].y, p[i].z);
        x = pp.x;
        y = pp.y;
        xs = 80 * x + X_MAX / 2;
        ys = 80 * y + Y_MAX / 2;
        poly[i].x = xs;
        poly[i].y = ys;
        // printf("%f,%f ",xs,ys);
    }
    return poly;
}

void interpolate_mesh(Vec3D C[], float n) {
    float t = 0, s = 0;
    Vec3D *poly;
    Vec3D patch[4];

    for (s = 0; s < 1.0; s += 1 / n) {
        for (t = 0; t < 1.0; t += 1 / n) {
            patch[0] = bezier_curve(C, t, s);
            patch[1] = bezier_curve(C, t + (1 / n), s);
            patch[2] = bezier_curve(C, t + (1 / n), s + (1 / n));

            if (visible(patch)) {
                poly = projection(patch);
                draw_polygon(poly, 3);
            }
            patch[0] = bezier_curve(C, t, s);
            patch[1] = bezier_curve(C, t + (1 / n), s + (1 / n));
            patch[2] = bezier_curve(C, t, s + (1 / n));

            if (visible(patch)) {
                poly = projection(patch);
                draw_polygon(poly, 3);
            }
        }
    }
}
int draw(void) {
    float x, y, z;
    unsigned int i, j;
    Vec3D pp;
    Vec3D patch[16];
    Vec3D trv = {-VIEWPOINT.x, -VIEWPOINT.y, -VIEWPOINT.z};

    idx = 0;
    al_clear_to_color(al_map_rgb(0, 0, 0));
    // N_VERTICES
    color = al_map_rgb(32, 32, 32);
    for (i = 0; i < N_VERTICES / 16; i++) {
        for (j = 0; j < 16; j++) {
            color = al_map_rgb(132, 132, 132);
            if (i == 4) color = al_map_rgb(64, 64, 255);
            if (i == 5) color = al_map_rgb(255, 255, 255);
            pp.x = M[idx].x;
            pp.y = M[idx].y;
            pp.z = M[idx].z;
            idx++;

            // rotation
            pp = rotate_x(pp.x, pp.y, pp.z, th);
            pp = rotate_z(pp.x, pp.y, pp.z, th / 5.0);
            // translate to center

            pp = translate(pp.x, pp.y, pp.z, trv);
            patch[j].x = pp.x;
            patch[j].y = pp.y;
            patch[j].z = pp.z;
        }

        interpolate_mesh(patch, 8);
    }
    return EXIT_SUCCESS;
}

int main() {
    al_init();
    al_install_keyboard();
    al_init_primitives_addon();
    read_model("models/teapot");

    ALLEGRO_TIMER *timer = al_create_timer(1.0);
    ALLEGRO_EVENT_QUEUE *queue = al_create_event_queue();
    ALLEGRO_DISPLAY *disp = al_create_display(X_MAX, Y_MAX);
    ALLEGRO_FONT *font = al_create_builtin_font();

    al_register_event_source(queue, al_get_keyboard_event_source());
    al_register_event_source(queue, al_get_display_event_source(disp));
    al_register_event_source(queue, al_get_timer_event_source(timer));

    bool redraw = true;
    ALLEGRO_EVENT event;

    al_start_timer(timer);
    while (1) {
        th = th + M_PI / 20;
        // if(th>=2*M_PI) th=-2*M_PI;

        al_wait_for_event(queue, &event);
        if (event.type == ALLEGRO_EVENT_TIMER)
            redraw = true;
        else if ((event.type == ALLEGRO_EVENT_DISPLAY_CLOSE))
            break;

        if (redraw && al_is_event_queue_empty(queue)) {
            al_clear_to_color(al_map_rgb(0, 0, 0));
            al_draw_text(font, al_map_rgb(255, 255, 255), 0, 0, 0,
                         "I'm a teapot");

            al_lock_bitmap(al_get_backbuffer(disp), ALLEGRO_PIXEL_FORMAT_ANY, 0);
            draw();
            al_unlock_bitmap(al_get_backbuffer(disp));
            al_flip_display();

            redraw = false;
        }
    }

    al_destroy_font(font);
    al_destroy_display(disp);
    al_destroy_timer(timer);
    al_destroy_event_queue(queue);

    return 0;
}
