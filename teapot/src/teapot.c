#include <allegro5/allegro5.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_primitives.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "auxiliar.h"

const int nvert = 4;
const int nfaces = 6;
const float sqrt2 = 1.414;
const float sqrt6 = 2.449;

unsigned int idx;
unsigned int screen, row, col;

float th = M_PI;
int X_MAX = 800, Y_MAX = 600;

Point M[N_VERTICES];

Point VIEWPOINT = {3.5, -4.0, 6.0};
ALLEGRO_COLOR color;

Point isometric_projection(float x, float y, float z) {
    Point result;
    result.x = (x - z) / sqrt2;
    result.y = (x + 2 * y + z) / sqrt6;
    result.z = 0;
    return result;
}

Point camera_projection(float x, float y, float z, float d) {
    Point r;

    r.x = x * d / z;
    r.y = y * d / z;
    r.z = 0;

    // printf("(%f,%f)",r.x,r.y);
    return r;
}

Point translate(float x, float y, float z, Point d) {
    Point result;
    result.x = x + d.x;
    result.y = y + d.y;
    result.z = z + d.z;
    return result;
}

Point rotate_x(float x, float y, float z, float th) {
    Point result;
    result.x = x;
    result.y = y * cos(th) - z * sin(th);
    result.z = y * sin(th) + z * cos(th);
    return result;
}

Point rotate_y(float x, float y, float z, float th) {
    Point result;
    result.x = x * cos(th) + z * sin(th);
    result.y = y;
    result.z = -x * sin(th) + z * cos(th);
    return result;
}

Point rotate_z(float x, float y, float z, float th) {
    Point result;
    result.x = x * cos(th) - y * sin(th);
    result.y = x * sin(th) + y * cos(th);
    result.z = z;
    return result;
}

void fill_bottom(Point p[]) {
    if ((p[1].y - p[0].y)==0 || (p[2].y - p[0].y)==0) return;

    float m1 = (p[1].x - p[0].x) / (p[1].y - p[0].y);
    float m2 = (p[2].x - p[0].x) / (p[2].y - p[0].y);
    // float m3=(p[2].x-p[1].y)/(p[2].y-p[1].y);

    float cx1 = p[0].x;
    float cx2 = p[0].x;

    for (int y = p[0].y; y <= p[2].y; y++) {
        line(cx1, y, cx2, y, color);
        cx1 += m1;
        cx2 += m2;
    }
}

void fill_top(Point p[]) {
    if ((p[2].y - p[0].y)==0 || (p[2].y - p[1].y)==0) return;

    float m1 = (p[2].x - p[0].x) / (p[2].y - p[0].y);
    float m2 = (p[2].x - p[1].x) / (p[2].y - p[1].y);
    // float m3=(p[2].x-p[1].y)/(p[2].y-p[1].y);

    float cx1 = p[2].x;
    float cx2 = p[2].x;

    for (int y = p[2].y; y >= p[0].y; y--) {
        line(cx1, y, cx2, y, color);
        cx1 -= m1;
        cx2 -= m2;
    }
}

void draw_triangle(Point t[]) {
    Point p[3];
    copy_t(t,p);
    Point tmp;
    Point v3;

    if (p[1].y < p[0].y) {
        tmp.x  = p[0].x;
        tmp.y  = p[0].y;
        p[0].x = p[1].x;
        p[0].y = p[1].y;
        p[1].x = tmp.x;
        p[1].y = tmp.y;
    }
    if (p[2].y < p[0].y) {
        tmp.x  = p[0].x;
        tmp.y  = p[0].y;
        p[0].x = p[2].x;
        p[0].y = p[2].y;
        p[2].x = tmp.x;
        p[2].y = tmp.y;
    }
    if (p[2].y < p[1].y) {
        tmp.x  = p[1].x;
        tmp.y  = p[1].y;
        p[1].x = p[2].x;
        p[1].y = p[2].y;
        p[2].x = tmp.x;
        p[2].y = tmp.y;
    }
    if (p[1].y == p[2].y) {
        fill_bottom(p);
    } else if (p[0].y == p[1].y) {
        fill_top(p);
    }
    else {

        v3.x =
            p[0].x + ((p[1].y - p[0].y) / (p[2].y - p[0].y)) * (p[2].x - p[0].x);
        v3.y = p[1].y;
        Point t1[3] = {p[0], p[1], v3};
        Point t2[3] = {p[1], v3, p[2]};

        fill_bottom(t1);
        fill_top(t2);
    }
}

int visible(Point p[]) {
    Point c, v0, v1, n;

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

Point *projection(Point p[]) {
    static Point poly[4];
    float xs, ys, x, y;
    Point pp;
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

void interpolate_mesh(Point C[], float n) {
    float t = 0, s = 0;
    Point *poly;
    Point patch[4];

    for (s = 0; s < 1.0; s += 1 / n) {
        for (t = 0; t < 1.0; t += 1 / n) {
            patch[0] = bezier_curve(C, t, s);
            patch[1] = bezier_curve(C, t + (1 / n), s);
            patch[2] = bezier_curve(C, t + (1 / n), s + (1 / n));

            if (visible(patch)) {
                poly = projection(patch);
                draw_triangle(poly);
            }
            patch[0] = bezier_curve(C, t, s);
            patch[1] = bezier_curve(C, t + (1 / n), s + (1 / n));
            patch[2] = bezier_curve(C, t, s + (1 / n));

            if (visible(patch)) {
                poly = projection(patch);
                draw_triangle(poly);
            }
        }
    }
}

int draw(void) {
    float x, y, z;
    unsigned int i, j;
    Point pp;
    Point patch[16];
    Point trv = {-VIEWPOINT.x, -VIEWPOINT.y, -VIEWPOINT.z};

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
    read_model("teapot/models/teapot");

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

            draw();
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
