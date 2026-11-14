#ifndef MAP_H
#define MAP_H

#include "game.h"

int load_map(const char *filename, Game *game);
void free_map(Game *game);

#endif
