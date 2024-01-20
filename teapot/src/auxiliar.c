#include "auxiliar.h"

void copy_t(Vec3D src[],Vec3D d[]) {
    for(int i=0; i<3; i++) {
        d[i].x=src[i].x;
        d[i].y=src[i].y;
    }
}

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

float dot(Vec3D a, Vec3D b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

void split(int a[], char *s) {
    char *token;
    const char delim[2] = ",";

    token = strtok(s, delim);
    int i = 0;
    while (token != NULL)
    {
        a[i] = atoi(token);
        i++;
        token = strtok(NULL, delim);
    }
}

Vec3D bezier_2d(Vec3D C[4], float t) {
    Vec3D p = {0, 0, 0};
    float b[4];
    b[0] = (1 - t) * (1 - t) * (1 - t);
    b[1] = 3 * t * (1 - t) * (1 - t);
    b[2] = 3 * t * t * (1 - t);
    b[3] = t * t * t;

    for (int i = 0; i < 4; i++) {
        p.x = p.x + b[i] * C[i].x;
        p.y = p.y + b[i] * C[i].y;
        p.z = p.z + b[i] * C[i].z;
    }
    return p;
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

Vec3D bezier_curve(Vec3D B[], float t, float s) {
    static Vec3D p;
    Vec3D C[4][4];
    for (int j = 0; j < 4; j++) {
        for (int i = 0; i < 4; i++)
            C[i][j] = B[4 * j + i];
    }
    p = bezier(C, t, s);
    return p;
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
        for (int j = 0; j < 16; j++)
        {
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
        if (strlen(line) < 5)
            dummy = fgets(line, sizeof(line), f);
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
    if (x > X_MAX || y > Y_MAX || x < 0 || y < 0)
        return;
    if (x1 > X_MAX || y1 > Y_MAX || x1 < 0 || y1 < 0)
        return;

    if (x==x1 && y==y1) {
        put_pixel(x,y,color);
        return;
    }

    int x0 = x;
    int y0 = y;
    int dx = abs(x1 - x0);
    int dy = -abs(y1 - y0);
    int sx,sy;
    int e2, error;

    sx = x0 < x1 ? 1 : -1;
    sy = y0 < y1 ? 1 : -1;
    error = dx + dy;

    while ( x0 != x1 || y0 != y1 ) {
        put_pixel(x0, y0, color);

        e2 = 2 * error;
        if (e2 >= dy) {
            error += dy;
            x0 += sx;
        }
        if (e2 <= dx) {
            error += dx;
            y0 += sy;
        }
    }
}

void draw_triangle(Vec3D t[]) {
    line(t[0].x,t[0].y,t[1].x,t[1].y, color);
    line(t[1].x,t[1].y,t[2].x,t[2].y, color);
    line(t[2].x,t[2].y,t[0].x,t[0].y, color);
}

void measure_side(Vec3D v0, Vec3D v1) {
    int y0=v0.y,y1=v1.y,x0=v0.x,x1=v1.x;

    //order with increasing y
    if (v1.y < v0.y) {
        y0=v1.y;
        x0=v1.x;
        y1=v0.y;
        x1=v0.x;
    }

    int dx=abs(x1-x0);
    int sx= x0 < x1 ? 1 :-1;
    int dy=-abs(y1-y0);
    int sy= y0 < y1 ? 1 :-1;
    int error=dx+dy;

    if(x0==x1 && y0==y1) {
        row_buffer[y0].left=x1;
        row_buffer[y0].right=x1;
        return;
    }
    while(x0!=x1 || y0!=y1) {
        if (x0<row_buffer[y0].left)
        {
            if (y0>=0 && y0<Y_MAX) row_buffer[y0].left=x0;
            if (row_buffer[y0].left<0) row_buffer[y0].left=0;
        }

        if (x0>row_buffer[y0].right) {
            if (y0>=0 && y0<Y_MAX) row_buffer[y0].right=x0;
            if (row_buffer[y0].right>X_MAX-1) row_buffer[y0].right=X_MAX-1;
        }
        int e2= 2*error;
        if (e2 >= dy) {
            error+=dy;
            x0+=sx;
        }
        if (e2 <= dx) {
            error+=dx;
            y0+=sy;
        }
    }
}

void h_line(int x1, int y1, int x2, int y2) {

    for(int x=x1; x<=x2; x++) {
        put_pixel(x,y1,color);
    }
}

void b_coordinates(int x, int y, Point v[], float t[]) {

    float d= (v[1].y - v[2].y) * (v[0].x - v[2].x) + (v[2].x - v[1].x)*(v[0].y-v[2].y);
    float lambda1=(v[1].y-v[2].y) *(x - v[2].x) + (v[2].x - v[1].x)*(y-v[2].y);
    float lambda2=(v[2].y-v[0].y) *(x - v[2].x) + (v[0].x - v[2].x)*(y-v[2].y);

    t[0]=lambda1/d;
    t[1]=lambda2/d;
    t[2]=1-lambda1/d-lambda2/d;
}

float calculate_w(float x,float y,Point v[],float t[]) {
    return v[0].z*t[0]+v[1].z*t[1]+v[2].z*t[2];
}

void color_line(int x1, int y1, int x2, int y2, Point v[], bool enable_zbuffer) {
    int r,g,b;
    float w;
    float t[3];
    if (x2>= X_MAX) x2=X_MAX-1;
    if (x1<0) x1=0;
    if (y1<0) return;
    if (y1>= Y_MAX) return;

    for(int x=x1; x<x2; x++) {
        b_coordinates(x,y1,v,t);
        if (enable_zbuffer) {
            w = calculate_w(x,y1,v,t);
            if (w > Z_BUFFER[x][y1]) {
                Z_BUFFER[x][y1] = w;
                r=floor(v[0].r * t[0] + v[1].r * t[1] + v[2].r * t[2]);
                g=floor(v[0].g * t[0] + v[1].g * t[1] + v[2].g * t[2]);
                b=floor(v[0].b * t[0] + v[1].b * t[1] + v[2].b * t[2]);
                color = al_map_rgb(r, g, b);
                put_pixel(x,y1,color);
            }
        } else {
            r=floor(v[0].r * t[0] + v[1].r * t[1] + v[2].r * t[2]);
            g=floor(v[0].g * t[0] + v[1].g * t[1] + v[2].g * t[2]);
            b=floor(v[0].b * t[0] + v[1].b * t[1] + v[2].b * t[2]);
            color = al_map_rgb(r, g, b);
            put_pixel(x,y1,color);
        }
    }
}

void flat_triangle(Vec3D t[]) {
    measure_side(t[0], t[1]);
    measure_side(t[1], t[2]);
    measure_side(t[2], t[0]);

    int top=Y_MAX;
    int bottom=0;

    for(int i=0; i<3;i++) {
        if (t[i].y<top && t[i].y>=0) top=t[i].y;
        if (bottom < t[i].y && t[i].y<Y_MAX) bottom=t[i].y;
    }

    for(int y=top; y<=bottom; y++) {
        h_line(row_buffer[y].left,y,row_buffer[y].right,y);

        row_buffer[y].left=X_MAX-1;
        row_buffer[y].right=0;
    }
}

void fill_triangle(Point v[],bool enable_zbuffer) {
    Vec3D t[3];
    for (int i=0; i<3; i++) {
        t[i].x=v[i].x;
        t[i].y=v[i].y;
        t[i].z=v[i].z;
    }

    if (t[0].x==t[1].x && t[0].y==t[1].y) return;
    if (t[0].x==t[2].x && t[0].y==t[2].y) return;
    if (t[1].x==t[2].x && t[1].y==t[2].y) return;

    measure_side(t[0], t[1]);
    measure_side(t[1], t[2]);
    measure_side(t[2], t[0]);

    int top=0;
    int bottom=0;

    for(int i=0; i<3;i++) {
        if (t[i].y<top) top=t[i].y;
        if (bottom < t[i].y) bottom=t[i].y;
    }
    if (top<0) top=0;
    if (bottom>=Y_MAX) bottom=Y_MAX-1;

    for(int y=top; y<=bottom; y++) {
        color_line(row_buffer[y].left,y,row_buffer[y].right,y, v, enable_zbuffer);

        row_buffer[y].left=X_MAX-1;
        row_buffer[y].right=0;
    }
}