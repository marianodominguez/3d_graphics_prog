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
BufferLine row_buffer[1];
ALLEGRO_DISPLAY* disp;
float Z_BUFFER[1][Y_MAX];

void draw() {
    color = al_map_rgb(128, 128, 255);
    // for(int x=0; x<X_MAX; x+=1) {
    //     line(x,0,x,Y_MAX-1, lc);
    //     //al_draw_line(x,0,X_MAX-x,Y_MAX, lc, 1.0);
    // }
    // al_clear_to_color(al_map_rgb(0, 0, 0));
    al_lock_bitmap(al_get_backbuffer(disp), ALLEGRO_PIXEL_FORMAT_ANY, 0);
    for(int x=0; x<X_MAX; x+=3) {
        line(x,0,X_MAX-x,Y_MAX,color);
    }
    al_unlock_bitmap(al_get_backbuffer(disp));
}

int main() {
    al_init();
    al_install_keyboard();
    al_init_primitives_addon();
    color = al_map_rgb(64, 64, 255);

    ALLEGRO_TIMER* timer = al_create_timer(1.0 / 3.0);
    ALLEGRO_EVENT_QUEUE* queue = al_create_event_queue();
    disp = al_create_display(X_MAX, Y_MAX);
    ALLEGRO_FONT* font = al_create_builtin_font();

    al_register_event_source(queue, al_get_keyboard_event_source());
    al_register_event_source(queue, al_get_display_event_source(disp));
    al_register_event_source(queue, al_get_timer_event_source(timer));

    bool redraw = true;
    ALLEGRO_EVENT event;
    al_start_timer(timer);
    while(1)
    {

        al_wait_for_event(queue, &event);

        if(event.type == ALLEGRO_EVENT_TIMER)
            redraw = true;
        else if( event.type == ALLEGRO_EVENT_DISPLAY_CLOSE )
            break;

        if(redraw && al_is_event_queue_empty(queue))
        {
            al_clear_to_color(al_map_rgb(0, 0, 0));
            al_draw_text(font, al_map_rgb(255, 255, 255), 0, 0, 0, "Line fill");

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
