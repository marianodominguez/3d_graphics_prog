#include <allegro5/allegro5.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_primitives.h>
#include <stdbool.h>

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

float k=-0.25,r=200,l=0.2,th=0;
float x,y,xp,yp;
char buffer[100];

void draw() {
    for (int i=0; i<800; i++) {
        x=r*( (1-k)*cos(th) + l*k*cos((1-k)/k*th) );
        y=r*((1-k)*sin(th) - l*k * sin((1-k)/k*th));
        if(i>0) {
            al_draw_line(x+400,y+300,xp+400,yp+300, al_map_rgb(255, 120, 255), 0);
        }
        xp=x;
        yp=y;
        th+=M_PI/30;
    }
}

int main() {

    al_init();
    al_install_keyboard();
    al_init_primitives_addon();

    ALLEGRO_TIMER* timer = al_create_timer(1.0 / 3.0);
    ALLEGRO_EVENT_QUEUE* queue = al_create_event_queue();
    ALLEGRO_DISPLAY* disp = al_create_display(800, 600);
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
        else if(event.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
            break;

        if(redraw && al_is_event_queue_empty(queue))
        {
            al_clear_to_color(al_map_rgb(0, 0, 0));
            sprintf(buffer,"2D shapes: k=%.3f,l=%.3f", k,l);
            al_draw_text(font, al_map_rgb(255, 255, 255), 0, 0, 0, buffer);

            al_lock_bitmap(al_get_backbuffer(disp), ALLEGRO_PIXEL_FORMAT_ANY, 0);
            draw();
            al_unlock_bitmap(al_get_backbuffer(disp));
            
            th=0;
            //r2+=1;

            k+=0.02;
            if (k>1.0) {
                k=-0.25;
                l+=0.1;
            }

            if (l>1.0) {
                l=-1.0;
            }

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
