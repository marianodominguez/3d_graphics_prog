#include "auxiliar.h"

Point normalize(Point v)
{
    Point r;
    float length = sqrt(v.x * v.x + v.y * v.y + v.z * v.z);

    r.x = v.x / length;
    r.y = v.y / length;
    r.z = v.z / length;

    return r;
}

Point cross(Point a, Point b)
{
    Point r;
    r.x = (a.y * b.z - a.z * b.y);
    r.y = -(a.x * b.z - a.z * b.x);
    r.z = (a.x * b.y - a.y * b.x);
    return r;
}

float dot(Point a, Point b)
{
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

void split(int a[], char *s)
{
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

void read_model(char *fn)
{
    char tmpx[12], tmpy[12], tmpz[12];
    char *dummy;
    char nv[5], line[255];
    int patch[16][32], points[16];
    Point P[306];

    FILE *f = fopen(fn, "r");
    if (f == NULL)
    {
        printf("Unable to read model");
        exit(1);
    }
    dummy = fgets(nv, sizeof(nv), f);
    if (atoi(nv) * 16 != N_VERTICES)
    {
        printf("Invalid model, wrong number of vertices");
        exit(1);
    }

    // read patch vertices
    for (int i = 0; i < 32; i++)
    {
        dummy = fgets(line, sizeof(line), f);
        split(points, line);
        for (int j = 0; j < 16; j++)
        {
            patch[j][i] = points[j];
        }
    }

    // Read points

    dummy = fgets(line, sizeof(line), f);
    if (atoi(line) != 306)
    {
        printf("Invalid model, wrong number of points");
        exit(1);
    }

    for (int i = 0; i < 306; i++)
    {
        dummy = fgets(line, sizeof(line), f);
        if (strlen(line) < 5)
            dummy = fgets(line, sizeof(line), f);
        sscanf(line, "%f,%f,%f", &P[i].x, &P[i].y, &P[i].z);
        // printf("%f %f %f\n",P[i].x,P[i].y,P[i].z);
    }
    int idx = 0;
    int v;
    for (int j = 0; j < 32; j++)
    {
        for (int i = 0; i < 16; i++)
        {
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

void put_pixel(int x, int y, ALLEGRO_COLOR color)
{
    al_draw_pixel(x, y, color);
}

void line(int x, int y, int x1, int y1, ALLEGRO_COLOR color)
{
    if (x > X_MAX || y > Y_MAX || x < 0 || y < 0)
        return;
    if (x1 > X_MAX || y1 > Y_MAX || x1 < 0 || y1 < 0)
        return;

    int x0 = x;
    int y0 = y;
    int dx = abs(x1 - x0);
    int dy = abs(y1 - y0);
    int sx = -1;
    int sy = -1;
    int e2, error;

    if (x0 < x1)
    {
        sx = 1;
    }
    if (y0 < y1)
    {
        sy = 1;
    }
    error = dx - dy;
    while (x0 != x1 || y0 != y1)
    {
        put_pixel(x0, y0, color);
        e2 = 2 * error;
        if (e2 > -dy)
        {
            error -= dy;
            x0 += sx;
        }
        if (e2 < dx)
        {
            error += dx;
            y0 += sy;
        }
    }
}
