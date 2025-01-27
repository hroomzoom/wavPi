#ifndef UI_H
#define UI_H

#include <ncurses.h>
#include <stdlib.h>

#define NUM_CHANNELS  2

#define ITEM_COLOR    1
#define LEVEL_COLOR   2
#define CHANNEL_COLOR 3

int levels[NUM_CHANNELS] = {50, 50};
const char *channel_names[NUM_CHANNELS] = {"LFO_DEPTH", "LFO_FREQ"};

void draw_ui(int selected) {
    
    clear();

    attron(COLOR_PAIR(ITEM_COLOR));
    mvprintw(1, 0, "Item: %s", channel_names[selected]);
    attroff(COLOR_PAIR(ITEM_COLOR));

    for (int i = 0; i < NUM_CHANNELS; i++) {
        int x = 5 + i * 15;
        int y = 5;

        if (i == selected) {
            attron(COLOR_PAIR(CHANNEL_COLOR));
            mvprintw(y - 2, x - 1, "[%s]", channel_names[i]);
            attroff(COLOR_PAIR(CHANNEL_COLOR));
        } 
        else
            mvprintw(y - 2, x - 1, " %s ", channel_names[i]);

        for (int j = 0; j < 10; j++) 
            mvprintw(y + j, x, "|");

        attron(COLOR_PAIR(LEVEL_COLOR));
        for (int j = 10 - levels[i] / 10; j < 10; j++)
            mvprintw(y + j, x, "|");
        attroff(COLOR_PAIR(LEVEL_COLOR));

        mvprintw(y + 11, x - 2, "[%3d%%]", levels[i]);
    }
    refresh();
}


#endif