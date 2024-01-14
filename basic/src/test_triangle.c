#include <allegro5/allegro5.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_primitives.h>
#include <stdbool.h>

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "auxiliar.h"

ALLEGRO_COLOR color;
int X_MAX = 800, Y_MAX = 600;

Point M[3];
ALLEGRO_DISPLAY* disp;
BufferLine row_buffer[600];

void draw() {
    al_lock_bitmap(al_get_backbuffer(disp), ALLEGRO_PIXEL_FORMAT_ANY, 0);
    ALLEGRO_COLOR lc = al_map_rgb(255, 255, 255);
    line(M[0].x, M[0].y, M[1].x, M[1].y, lc);
    line(M[0].x, M[0].y, M[2].x, M[2].y, lc);
    line(M[1].x, M[1].y, M[2].x, M[2].y, lc);
    fill_triangle(M);
    al_unlock_bitmap(al_get_backbuffer(disp));
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

    M[0]=(Point) {10,5,0};
    M[1]=(Point) {300,485,0};
    M[2]=(Point) {185,400,0};

    int dy1=5,dy0=5,dx2=5;

    al_start_timer(timer);
    while(1)
    {

        al_wait_for_event(queue, &event);

        if(event.type == ALLEGRO_EVENT_TIMER)
            redraw = true;
        else if( event.type == ALLEGRO_EVENT_DISPLAY_CLOSE )
            break;

        if (M[1].y<=0) dy1=5;
        if (M[1].y>=Y_MAX-5) dy1=-5;
        M[1].y+=dy1;

        if (M[0].y<=0) dy0=5;
        if (M[0].y>=Y_MAX-5) dy0=-5;
        M[0].y+=dy0;

        if (M[2].x<=0) dx2=5;
        if (M[2].x>=X_MAX-5) dx2=-5;
        M[2].x+=dx2;

        // printf("%f,%f ", M[0].x, M[0].y);
        // printf("%f,%f ", M[1].x, M[1].y);
        // printf("%f,%f \n", M[2].x, M[2].y);

        if(redraw && al_is_event_queue_empty(queue))
        {
            al_clear_to_color(al_map_rgb(0, 0, 0));
            al_draw_text(font, al_map_rgb(255, 255, 255), 0, 0, 0, "Triangle fill");

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
