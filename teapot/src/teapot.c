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

Point M[N_VERTICES];
BufferLine row_buffer[Y_MAX];

const Vec3D VIEWPOINT = {5.5, -5.0, 6.0};
const Vec3D LIGHT_SOURCE = {10, 10.0, 6.0};
const float LIGHT_INTENSITY = 0.99;
float Z_BUFFER[X_MAX][Y_MAX];

ALLEGRO_COLOR color;
ALLEGRO_DISPLAY *disp;

Vec3D isometric_projection(float x, float y, float z) {
    Vec3D result;
    result.x = (x - z) / sqrt2;
    result.y = (x + 2 * y + z) / sqrt6;
    result.z = 1/z;
    return result;
}

Vec3D camera_projection(float x, float y, float z, float d) {
    Vec3D r;
    r.x = x * d / z;
    r.y = y * d / z;
    r.z = 1/z;
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

Point *projection(Point p[]) {
    static Point poly[3];
    float xs, ys, x, y;
    Vec3D pp;
    for (int i = 0; i < 3; i++) {
        //pp = camera_projection(p[i].x,p[i].y,p[i].z, -1.0);
        pp = isometric_projection(p[i].x, p[i].y, p[i].z);
        x = pp.x;
        y = pp.y;
        xs = 100 * x + X_MAX / 2;
        ys = 100 * y + Y_MAX / 2;
        poly[i].x = round(xs);
        poly[i].y = round(ys);
        poly[i].z = pp.z;

        poly[i].r= p[i].r;
        poly[i].g= p[i].g;
        poly[i].b= p[i].b;

        //printf("%f,%f ",xs,ys);
    }
    return poly;
}

Point* lightModel(Vec3D patch[3], Vec3D normals[]) {
    static Point m[3];

    Vec3D v0={patch[0].x, patch[0].y, patch[0].z};
    Vec3D v1={patch[1].x, patch[1].y, patch[1].z};
    Vec3D v2={patch[2].x, patch[2].y, patch[2].z};

    for(int i=0; i<3; i++) {
        Vec3D normal = normals[i];

        m[i].x=patch[i].x;
        m[i].y=patch[i].y;
        m[i].z=patch[i].z;

        m[i].r=(dot(normal, normalize(LIGHT_SOURCE))*LIGHT_INTENSITY)*120+150;
        m[i].g=(dot(normal, normalize(LIGHT_SOURCE))*LIGHT_INTENSITY)*120+150;
        m[i].b=(dot(normal, normalize(LIGHT_SOURCE))*LIGHT_INTENSITY)*120+150;

        if (m[i].r<0) m[i].r =0;
        if (m[i].g<0) m[i].g =0;
        if (m[i].b<0) m[i].b =0;
    }
    return m;
}

float derivativeBezier(float u , float x0, float x1, float x2, float x3) {
    return -3 * (1 - u) * (1 - u) * x0 +
       (3 * (1 - u) * (1 - u) - 6 * u * (1 - u)) * x1 +
       (6 * u * (1 - u) - 3 * u * u) * x2 +
       3 * u * u * x3;
}

Vec3D dUBezier(Vec3D C[], float u, float v) {
    Vec3D P[4];
    Vec3D vCurve[4];
    Vec3D r;
    for (int i = 0; i < 4; ++i) {
       P[0] = C[i];
       P[1] = C[4 + i];
       P[2] = C[8 + i];
       P[3] = C[12 + i];
       vCurve[i] = bezier_2d(P, v);
    }
    r.x=derivativeBezier(u, vCurve[0].x,vCurve[1].x,vCurve[2].x,vCurve[3].x);
    r.y=derivativeBezier(u, vCurve[0].y,vCurve[1].y,vCurve[2].y,vCurve[3].y);
    r.z=derivativeBezier(u, vCurve[0].z,vCurve[1].z,vCurve[2].z,vCurve[3].z);

    return r;
}

Vec3D dVBezier(Vec3D C[], float u, float v) {
    Vec3D uCurve[4];
    Vec3D r;
    for (int i = 0; i < 4; ++i) {
       uCurve[i] = bezier_2d(C + 4 * i, u);
    }

    r.x=derivativeBezier(v, uCurve[0].x,uCurve[1].x,uCurve[2].x,uCurve[3].x);
    r.y=derivativeBezier(v, uCurve[0].y,uCurve[1].y,uCurve[2].y,uCurve[3].y);
    r.z=derivativeBezier(v, uCurve[0].z,uCurve[1].z,uCurve[2].z,uCurve[3].z);

    return r;
}

Vec3D bezier_normal(Vec3D C[], float u, float v) {
    Vec3D N;
    Vec3D dU = dUBezier(C, u, v);
    Vec3D dV = dVBezier(C, u, v);
    N = normalize(cross(dU,dV));
    return N;
}

bool invalid_triangle(Vec3D t[3]) {
    if (t[0].x==t[1].x && t[0].y==t[1].y) return true; 
    if (t[0].x==t[2].x && t[0].y==t[2].y) return true;
    if (t[1].x==t[2].x && t[1].y==t[2].y) return true;
    return false;
}

void interpolate_mesh(Vec3D C[], float n) {
    float t = 0, s = 0;
    Point *poly;
    Vec3D patch[3];
    Vec3D normals[3];
    float delta=1.0/n;
    Point *mpatch;

    for (s = 0; s < 1.0; s += delta) {
        for (t = 0; t < 1.0; t += delta) {
            patch[0] = bezier_curve(C, t, s);
            patch[1] = bezier_curve(C, t + delta, s);
            patch[2] = bezier_curve(C, t + delta, s + delta);

            normals[0] = bezier_normal(C, t, s);
            normals[1] = bezier_normal(C, t + delta, s);
            normals[2] = bezier_normal(C, t + delta, s + delta);

            if (visible(patch) && !invalid_triangle(patch) ) {
                mpatch =lightModel(patch, normals);
                poly = projection(mpatch);
                fill_triangle(poly, true);
            }
            patch[0] = bezier_curve(C, t, s);
            patch[1] = bezier_curve(C, t + delta, s + delta);
            patch[2] = bezier_curve(C, t, s + delta);

            normals[0] = bezier_normal(C, t, s);
            normals[1] = bezier_normal(C, t + delta, s + delta);
            normals[2] = bezier_normal(C, t , s + delta);

            if (visible(patch) && !invalid_triangle(patch)) {
                mpatch = lightModel(patch, normals);
                poly = projection(mpatch);
                fill_triangle(poly, true);
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

    //idx = 15*16;
    idx = 0;
    al_clear_to_color(al_map_rgb(0, 0, 0));
    // N_VERTICES
    //for (i = 0; i < 1; i++) {
    for (i = 0; i < N_VERTICES / 16; i++) {
        for (j = 0; j < 16; j++) {
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

        interpolate_mesh(patch, 10.0);
    }
    return EXIT_SUCCESS;
}

int main() {

    for(int i=0; i<Y_MAX; i++) {
        row_buffer[i].left=X_MAX-1;
        row_buffer[i].right=0;
    }

    al_init();
    al_install_keyboard();
    al_init_primitives_addon();
    read_model("teapot/models/teapot");

    ALLEGRO_TIMER *timer = al_create_timer(1.0);
    ALLEGRO_EVENT_QUEUE *queue = al_create_event_queue();
    disp = al_create_display(X_MAX, Y_MAX);
    ALLEGRO_FONT *font = al_create_builtin_font();

    al_register_event_source(queue, al_get_keyboard_event_source());
    al_register_event_source(queue, al_get_display_event_source(disp));
    al_register_event_source(queue, al_get_timer_event_source(timer));

    bool redraw = true;
    ALLEGRO_EVENT event;

    al_start_timer(timer);
    while (1) {
        th = th + M_PI / 20;
        if(th/5.0>=2*M_PI) th=0;

        al_wait_for_event(queue, &event);
        if (event.type == ALLEGRO_EVENT_TIMER)
            redraw = true;
        else if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
            break;

        if (redraw && al_is_event_queue_empty(queue)) {
            for(int i=0; i<X_MAX; i++)
                for(int j=0; j<Y_MAX; j++)
                Z_BUFFER[i][j]=-9999;

            al_clear_to_color(al_map_rgb(0, 0, 0));
            al_draw_text(font, al_map_rgb(255, 255, 255), 0, 0, 0,
                         "I'm a teapot");
            //printf("%f\n", th);
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
