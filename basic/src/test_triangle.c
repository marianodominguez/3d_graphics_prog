#include <allegro5/allegro5.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_primitives.h>
#include <stdbool.h>

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "auxiliar.h"

ALLEGRO_COLOR color;

Point M[1];
Vec3D T[3];
ALLEGRO_DISPLAY* disp;
BufferLine row_buffer[600];
float Z_BUFFER[1][Y_MAX];

void draw() {
    ALLEGRO_COLOR lc = al_map_rgb(255, 255, 255);
    line(T[0].x, T[0].y, T[1].x, T[1].y, lc);
    line(T[0].x, T[0].y, T[2].x, T[2].y, lc);
    line(T[1].x, T[1].y, T[2].x, T[2].y, lc);
    flat_triangle(T);
}

int main() {
    al_init();

    for(int i=0; i<Y_MAX; i++) {
        row_buffer[i].left=X_MAX;
        row_buffer[i].right=0;
    }

    al_install_keyboard();
    al_init_primitives_addon();
    color = al_map_rgb(64, 64, 255);

    ALLEGRO_TIMER* timer = al_create_timer(1.0 / 5.0);
    ALLEGRO_EVENT_QUEUE* queue = al_create_event_queue();
    disp = al_create_display(X_MAX, Y_MAX);
    ALLEGRO_FONT* font = al_create_builtin_font();

    al_register_event_source(queue, al_get_keyboard_event_source());
    al_register_event_source(queue, al_get_display_event_source(disp));
    al_register_event_source(queue, al_get_timer_event_source(timer));

    bool redraw = true;
    ALLEGRO_EVENT event;

    T[0]=(Vec3D) {10,5,0};
    T[1]=(Vec3D) {300,485,0};
    T[2]=(Vec3D) {185,400,0};

    int dy1=5,dy0=5,dx2=5;

    al_start_timer(timer);
    while(1)
    {

        al_wait_for_event(queue, &event);

        if(event.type == ALLEGRO_EVENT_TIMER)
            redraw = true;
        else if( event.type == ALLEGRO_EVENT_DISPLAY_CLOSE )
            break;

        if (T[1].y<=0) dy1=5;
        if (T[1].y>=Y_MAX-5) dy1=-5;
        T[1].y+=dy1;

        if (T[0].y<=0) dy0=5;
        if (T[0].y>=Y_MAX-5) dy0=-5;
        T[0].y+=dy0;

        if (T[2].x<=0) dx2=5;
        if (T[2].x>=X_MAX-5) dx2=-5;
        T[2].x+=dx2;

        // printf("%f,%f ", T[0].x, T[0].y);
        // printf("%f,%f ", T[1].x, T[1].y);
        // printf("%f,%f \n", T[2].x, T[2].y);

        if(redraw && al_is_event_queue_empty(queue))
        {
            al_clear_to_color(al_map_rgb(0, 0, 0));
            al_draw_text(font, al_map_rgb(255, 255, 255), 0, 0, 0, "Triangle fill");

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
